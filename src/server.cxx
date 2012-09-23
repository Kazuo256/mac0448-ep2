
#include <cstdlib>

#include <string>
#include <iostream>
#include <tr1/functional>

#include "connection.h"
#include "TCPconnection.h"
#include "UDPconnection.h"
#include "command.h"
#include "serverhandler.h"
#include "eventmanager.h"
#include "serverdata.h"

using std::string;
using std::cin;
using std::cout;
using std::cerr;
using std::tr1::bind;

using ep2::Connection;
using ep2::TCPConnection;
using ep2::UDPConnection;
using ep2::EventManager;
using ep2::Command;
using ep2::ServerHandler;
using ep2::ServerData;

// Gerenciador de eventos de input.
static EventManager     manager;
// Dados do servidor.
static ServerData       serverdata;
// Tratador de comandos recebidos pelo servidor.
static ServerHandler    serverhandler(&serverdata);
// Usadas para receber tanto conexões TCP quanto UDP do clientes.
static TCPConnection    tcp_server;
static UDPConnection    udp_server;

//// Eventos de input do servidor ////

// Quando o usuário digita algo no terminal do servidor.
// Na verdade, o servidor não tem terminal: ele ignora tudo menos o EOF.
// Assim é possível encerrar normalmente o programa sem usar interrupções.
static EventManager::Status prompt_event () {
  string ignored;
  cin >> ignored;
  return cin.eof() ? EventManager::EXIT : EventManager::CONTINUE;
}

// Quando o servidor recebe algum comando dos clientes.
// Usa-se std::tr1::bind para garantir que cada instância desse evento esteja
// associada a um cliente diferente.
static EventManager::Status command_event (Connection* client) {
  // Pega o comando.
  Command cmd = client->receive();
  // Se for o comando de disconectar, remove as informações do cliente
  // dos dados do servidor e deleta as conexões usadas.
  if (cmd.opcode() == Command::DISCONNECT) {
    string nick = serverdata.get_link(client->sockfd());
    serverdata.remove_link(client->sockfd());
    serverdata.erase_connection(client);
    if (nick.size()) {
      serverdata.erase_user(nick);
      cout << "[Cliente com nick '" << nick << "' desconectando]\n";
    } else
      cout << "[Cliente anônimo ou conexão secundária sendo encerrada]\n";
    delete client;
    // Para de receber comandos dese cliente.
    return EventManager::STOP;
  }
  // Se não for o ocmando de disconectar, delega o trabalho para o
  // ServerHandler.
  serverhandler.handle(client, cmd);
  // Continua esperando novos comandos desse cliente.
  return EventManager::CONTINUE;
}

// Quando um cliente pede para se conectar com o servidor.
// Um novo evento de tratar comandos desse cliente é adicionado ao gerenciador
// de eventos.
static EventManager::Status accept_event (Connection *serv) {
  // Aceita a conexão do cliente.
  Connection *client = serv->accept();
  // Adiciona nosdados do servidor.
  serverdata.add_connection(client);
  // Cria um novo evento para lidar com os comandos desse cliente, criando um
  // elo entre a função command_event() e o cliente que pediu a conexão.
  manager.add_event(client->sockfd(), bind(command_event, client));
  // Continua esperando conexões de mais clientes.
  return EventManager::CONTINUE;
}

//// MAIN ////

int main (int argc, char **argv) {
	if (argc != 2) {
    cerr << "Uso: " << argv[0] << " <porta>\n";
		exit(1);
	}
  // Abre para receber tanto conexões TCP quanto UDP.
  tcp_server.host(atoi(argv[1]));
  udp_server.host(atoi(argv[1]));
  // Registra eventos: um para capturar CTRL+D do usuário, e outros dois para
  // receber os requisitos de conexões dos clientes e estabelecer conexões
  // individuais para cada um.
  manager.add_event(STDIN_FILENO, EventManager::Callback(prompt_event));
  manager.add_event(tcp_server.sockfd(), bind(accept_event, &tcp_server));
  manager.add_event(udp_server.sockfd(), bind(accept_event, &udp_server));
  // Algumas mensagens informativas.
  cout << "[Servidor aberto na porta " << atoi(argv[1]) << "]\n";
  cout << "[Para encerrar o programa use CTRL+D]\n";
  // Deixa o gerenciador de eventos cuidar do resto.
  manager.loop();
	return 0;
}

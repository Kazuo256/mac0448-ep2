
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <tr1/unordered_map>

#include "prompt.h"
#include "connection.h"
#include "TCPconnection.h"
#include "UDPconnection.h"
#include "eventmanager.h"
#include "command.h"

using std::string;
using std::stringstream;
using std::ifstream;
using std::ofstream;
using std::ios_base;
using std::cout;
using std::tr1::unordered_map;

using ep2::Prompt;
using ep2::Connection;
using ep2::TCPConnection;
using ep2::UDPConnection;
using ep2::EventManager;
using ep2::Command;

// AVISO: esse arquivo acabou ficando muito grande pois não tivemos tempo de
// refatorá-lo mais. A ideia era ter implementado um ClientHandler a partir da
// interface CommandHandler, mas não deu.

// Informações sobre o host.
static string                       hostname;
static unsigned short               port;
// Gerenciador de eventos de input.
static EventManager                 manager;
// Prompt para o usuário.
static Prompt                       prompt;
// Conexões com o servidor.
//  server_input:   conexão primária, usada para enviar comandos para o servidor
//                  e obter as respostas quando houver.
//  server_output:  conexão secundŕia, usada para receber comandos do servidor.
static Connection                   *server_output, *server_input;
// ID desse cliente no servidor.
static int                          ID = -1;
// Lista de pedidos pendentes de transferências vindas de outros clientes
// conectados no servidor.
static unordered_map<string,string> senders;

// Cria as conexões usando TCP ou UDP de acordo com o parâmetro passado.
static void create_server_connections (bool udp) {
  if (udp) {
    server_input = new UDPConnection;
    server_output = new UDPConnection;
  } else {
    server_input = new TCPConnection;
    server_output = new TCPConnection;
  }
}

//// Eventos de input do cliente ////

// Quando o usuário digita algo no terminal.
static EventManager::Status prompt_event () {
  // Delega trabalho de tratar o input para o prompt.
  // Se devolver false é porque o usuário quer encerrar o programa.
  return prompt.check_input()
    ? EventManager::CONTINUE  // continua ouvindo o terminal
    : EventManager::EXIT;     // para de tratar todos os eventos
}

// Quando o servidor manda um comando pela conexão secundária.
static EventManager::Status server_event () {
  // Pega o comando do servidor.
  Command cmd = server_output->receive();
#ifdef EP2_DEBUG
  cout << "\n[Comando recebido: " << static_cast<string>(cmd) << "]";
#endif
  // Pode ser um comando de mensagem de outro cliente ou de transferência de
  // arquivo de outro cliente.
  switch (cmd.opcode()) {
    case Command::MSG:
    {
      // Verifica integridade do comando.
      if (cmd.num_args() < 2)
        cout << "\n[Bad message from server]\n";
      else {
        // Se estiver OK, exibe para o usuário.
        cout << "\n[Message from '" << cmd.arg(0) << "':] ";
        cout << cmd.arg(1) << "\n";
        prompt.init();
      }
    } break;
    case Command::SEND:
    {
      // Verifica integridade do comando
      if (cmd.num_args() < 2)
        cout << "\n[Bad send command from server]\n";
      else {
        // Se estiver OK, avisa o usuário e adiciona na lista de transferências
        // pendentes.
        senders[cmd.arg(0)] = cmd.arg(1);
        cout << "\n['" << cmd.arg(0) << "' wants to send file '" << cmd.arg(1) << "']";
        cout << "\n[To accept it use '/accept <user>']\n";
        prompt.init();
      }
    } break;
  }
  // Sempre continua ouvindo comandos do servidor.
  return EventManager::CONTINUE;
}

// EVENTOS DE PROMPT

static bool     secondary_connected = false;
static string   current_nick = "";
static ifstream current_file;

static void nick_event (const string& nick, const string& unused) {
  if (nick.empty())
    cout << "Nick vazio inválido.\n";
  else {
    if (!secondary_connected) {
      // Abre conexão secundária
      server_output->connect(hostname, port);
      secondary_connected = true;
    }
    // Pede para associar um novo nick ao ID
    stringstream ID_string;
    ID_string << ID;
    server_output->send(Command::nick(nick, ID_string.str()));
    Command response = server_output->receive();
    if (response.opcode() == Command::REFUSE_NICK)
      cout << "[Nick '" << nick << "' was refused]\n";
    else if (response.opcode() == Command::ACCEPT_NICK) {
      cout << "[Now using nick '" << nick << "'].\n";
      manager.add_event(server_output->sockfd(), server_event);   
      current_nick = nick;
    }
    else
      cout << "[Unexpected answer from server]\n";
  }
} 

static void list_event (const string& unused1, const string& unused2) {
  server_input->send(Command::list_request());
  Command response = server_input->receive();
  if (response.opcode() == Command::LIST_RESPONSE) {
    cout << "[Currently on-line users:]\n";
    for (size_t i = 0; i < response.num_args(); ++i)
      cout << "  " << response.arg(i) << "\n";
  }
  else
    cout << "[Unexpected answer from server]\n";
}

static void msg_event (const string& target, const string& msg) {
  if (current_nick.empty())
    cout << "[You cannot send messages without a nick!]\n";
  else if (target.empty() || msg.empty())
    cout << "[Invalid empty target nick or message]\n";
  else {
    server_input->send(Command::msg(target, msg));
    Command response = server_input->receive();
    if (response.opcode() == Command::MSG_OK)
      cout << "[Message sent to '" << target << "']\n";
    else if (response.opcode() == Command::MSG_FAIL)
      cout << "[Failed to send message to '" << target << "']\n";
    else
      cout << "[Unexpected answer from server]\n";
  }
}

static void transfer_event (const string& target, const string& filepath) {
  if (current_nick.empty()) {
    cout << "[Você não pode enviar arquivos sem um nick!]\n";
    return;
  }
  if (target.empty() || filepath.empty()) {
    cout << "[Usuário alvo ou arquivo a ser enviado não definidos]\n";
    return;
  }
  current_file.open(filepath.c_str(), ios_base::in | ios_base::binary);
  if (current_file.fail()) {
    cout << "[Não foi possível acessar o arquivo '" << filepath << "']\n";
    return;
  }
  server_input->send(Command::send(target, filepath));
  cout << "[Tentando enviar arquivo para o usuário '" << target << "']\n";
  Command response = server_output->receive();
  switch (response.opcode()) {
    case Command::SEND_OK:
      if (response.num_args() < 2)
        cout << "[Resposta inesperada do servidor]\n";
      else {
        TCPConnection transfer;
        unsigned short port = atoi(response.arg(1).c_str());
        cout << "[Enviando para " << response.arg(0) << ":" << port << "]\n";
        transfer.connect(response.arg(0), 8080);
        char chunk[8*255+1];
        while (current_file.good()) {
          current_file.read(chunk, 8*255);
          size_t n = current_file.gcount();
          chunk[n] = '\0';
          cout << "[Enviando chunk de tamanho " << n << "]\n";
          transfer.send(Command::chunk(chunk));
          Command check = transfer.receive();
          if (check.opcode() != Command::CONTINUE) break;
        }
      }
      break;
    case Command::SEND_FAIL:
      cout << "[Não foi possível enviar o arquivo para '" << target << "']\n";
      if (response.num_args() > 0)
        cout << "[" << response.arg(0) << "]\n";
      break;
    default:
      cout << "[Resposta inesperada do servidor]\n";
      break;
  }
  current_file.close();
}

static void show_senders () {
  unordered_map<string,string>::iterator it;
  if (senders.empty())
    cout << "[No momento, ninguém está tentando te enviar nenhum arquivo]\n";
  else {
    cout << "[Usuários tentando te enviar arquivos no momento:]\n";
    for (it = senders.begin(); it != senders.end(); ++it)
      cout << "  " << it->first << ": "<< it->second << "\n";
  }
}

static void accept_event (const string& sender, const string& unused) {
  if (current_nick.empty()) {
    cout << "[Você não pode receber arquivos sem um nick!]\n";
    return;
  }
  unordered_map<string,string>::iterator it;
  if (sender.empty()) {
    cout << "[Especifique de quem quer aceitar o arquivo]\n";
    show_senders();
    return;
  }
  if ((it = senders.find(sender)) == senders.end()) {
    cout << "[Nenhuma transferência pendente de '" << sender << "']\n";
    return;
  }
  server_input->send(Command::accept(sender));
  TCPConnection temp;
  temp.host(8080);
  Connection *download = temp.accept();
  ofstream file((string("downloads/")+it->second).c_str(),
                ios_base::out | ios_base::binary);
  while (true) {
    Command bytes = download->receive();
    if (bytes.opcode() == Command::DISCONNECT) break;
    if (bytes.opcode() != Command::CHUNK) {
      cout << "[Outro usuário enviou pacote desconhecido "<<bytes.opcode()<<"]\n";
      break;
    }
    if (bytes.num_args() < 1) {
      cout << "[Dados corrompidos?]\n";
      break;
    }
    for (size_t i = 0; i < bytes.num_args(); ++i)
      file << bytes.arg(i);
    download->send(Command::cont());
  }
  cout << "[Arquivo recebido com sucesso]\n";
  file.close();
  delete download;
  senders.erase(sender);
}

static void refuse_event (const string& sender, const string& unused) {
  if (current_nick.empty()) {
    cout << "[Você não pode recusar arquivos sem um nick!]\n";
    return;
  }
  unordered_map<string,string>::iterator it;
  if (sender.empty()) {
    cout << "[Especifique de quem quer recusar o arquivo]\n";
    show_senders();
    return;
  }
  if ((it = senders.find(sender)) == senders.end()) {
    cout << "[Nenhuma transferência pendente de '" << sender << "']\n";
    return;
  }
  server_input->send(Command::refuse(sender));
  senders.erase(sender);
}

// MAIN

int main(int argc, char **argv) {

	if (argc < 3 || argc > 4) {
      fprintf(stderr,"Uso: %s <Endereco IP|Nome> <Porta> [tcp|udp]\n",argv[0]);
		exit(1);
	}
  
  hostname = argv[1];
  port = atoi(argv[2]);
  bool udp = argc == 4 && !strcmp(argv[3], "udp");
  create_server_connections(udp);
  // Conexão primária
  server_input->connect(hostname, port);
  // Pede ID
  server_input->send(Command::request_id());
  Command cmd = server_input->receive();
  if (cmd.opcode() == Command::GIVE_ID)
    ID = atoi(cmd.arg(0).c_str());
  printf("Recebido ID %d\n", ID);
  // Prepara e entra em loop
  prompt.add_command("/nick", nick_event);
  prompt.add_command("/list", list_event);
  prompt.add_command("/msg", msg_event);
  prompt.add_command("/send", transfer_event);
  prompt.add_command("/accept", accept_event);
  prompt.add_command("/refuse", refuse_event);
  manager.add_event(STDIN_FILENO, EventManager::Callback(prompt_event));
  //  TODO mensagens informativas
  prompt.init();
  manager.loop();
  if (current_file.is_open()) current_file.close();
  server_input->send(Command::disconnect());
  server_output->send(Command::disconnect());
  delete server_input;
  delete server_output;
  return 0;
}


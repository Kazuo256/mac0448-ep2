
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <tr1/functional>

#include "connection.h"
#include "TCPconnection.h"
#include "eventlistener.h"
#include "command.h"
#include "serverhandler.h"

using std::vector;
using std::map;
using std::string;
using std::cin;

using ep2::Connection;
using ep2::TCPConnection;
using ep2::EventListener;
using ep2::Command;
using ep2::ServerHandler;

typedef map<int, Connection*> ConnectionTable;

static TCPConnection    server;
static ConnectionTable  table;
static EventListener    listener;

static void clear_clients (ConnectionTable& table) {
  for (ConnectionTable::iterator it = table.begin(); it != table.end(); ++it)
    delete it->second;
  table.clear();
}

// EVENTS

static EventListener::Status prompt_event () {
  string garbage;
  cin >> garbage;
  return cin.eof() ? EventListener::EXIT : EventListener::CONTINUE;
}

static EventListener::Status command_event (Connection* client) {
  string packet = client->receive();
  // Check end of client msgs
  if (!packet.size()) {
    table.erase(client->sockfd());
    delete client;
    return EventListener::STOP;
  }
  /* Lê a linha enviada pelo cliente e escreve na saída padrão */
  Command cmd = Command::from_packet(packet);
  ServerHandler().handle(cmd);
  if ((fputs(packet.c_str(),stdout)) == EOF) {
     perror("fputs error");
     exit (1);
  }
  /* Agora re-envia a linha para o cliente */
  client->send(packet);
  return EventListener::CONTINUE;
}

static EventListener::Status accept_event () {
  Connection *client = server.accept();
  table[client->sockfd()] = client;
  listener.add_input(client->sockfd(), std::tr1::bind(command_event, client));
  return EventListener::CONTINUE;
}

// MAIN

int main (int argc, char **argv) {
	if (argc != 2) {
      fprintf(stderr,"Uso: %s <Porta>\n",argv[0]);
		exit(1);
	}
  server.host(atoi(argv[1]));
  listener.add_input(STDIN_FILENO, EventListener::Callback(prompt_event));
  listener.add_input(server.sockfd(), EventListener::Callback(accept_event));
  listener.listen();
  clear_clients(table);
	return 0;
}


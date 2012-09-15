
#include <vector>
#include <string>
#include <iostream>
#include <tr1/unordered_map>
#include <tr1/functional>

#include "connection.h"
#include "TCPconnection.h"
#include "command.h"
#include "serverhandler.h"
#include "eventmanager.h"
#include "serverdata.h"

using std::vector;
using std::string;
using std::cin;
using std::cout;
using std::tr1::unordered_map;

using ep2::Connection;
using ep2::TCPConnection;
using ep2::EventManager;
using ep2::Command;
using ep2::ServerHandler;
using ep2::ServerData;

static EventManager     manager;
static ServerData       serverdata(manager);
static ServerHandler    serverhandler(&serverdata);
static TCPConnection    server;
// EVENTS

static EventManager::Status prompt_event () {
  string garbage;
  cin >> garbage;
  return cin.eof() ? EventManager::EXIT : EventManager::CONTINUE;
}

static EventManager::Status command_event (Connection* client) {
  Command cmd = client->receive();
  // Check end of client msgs
  if (cmd.opcode() == Command::DISCONNECT) {
    serverdata.erase_connection(client->sockfd());
    delete client;
    return EventManager::STOP;
  }
  /* Lê a linha enviada pelo cliente e escreve na saída padrão */
  serverhandler.handle(client, cmd);
  return EventManager::CONTINUE;
}

static EventManager::Status accept_event () {
  Connection *client = server.accept();
  serverdata.set_connection(client);
  manager.add_event(client->sockfd(), std::tr1::bind(command_event, client));
  return EventManager::CONTINUE;
}

// MAIN

int main (int argc, char **argv) {
	if (argc != 2) {
      fprintf(stderr,"Uso: %s <Porta>\n",argv[0]);
		exit(1);
	}
  server.host(atoi(argv[1]));
  manager.add_event(STDIN_FILENO, EventManager::Callback(prompt_event));
  manager.add_event(server.sockfd(), EventManager::Callback(accept_event));
  manager.loop();
	return 0;
}

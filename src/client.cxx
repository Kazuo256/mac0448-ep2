
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>

#include "prompt.h"
#include "TCPconnection.h"
#include "eventmanager.h"
#include "command.h"

#define MAXLINE 4096

using std::string;
using std::stringstream;
using std::cout;

using ep2::Prompt;
using ep2::TCPConnection;
using ep2::EventManager;
using ep2::Command;

static string         hostname;
static unsigned short port;
static EventManager   manager;
static Prompt         prompt;
static TCPConnection  server_output, server_input;
static int            ID = -1;

// EVENTOS DE INPUT

static EventManager::Status prompt_event () {
  return prompt.check_input()
    ? EventManager::CONTINUE
    : EventManager::EXIT;
}

static EventManager::Status server_event () {
  Command cmd = server_output.receive();
  cout << "Received command: " << static_cast<string>(cmd) << "\n";
  return EventManager::CONTINUE;
}

// EVENTOS DE PROMPT

static void nick_event (const string& nick, const string& unused) {
  if (nick.empty())
    cout << "Nick vazio inválido.\n";
  else {
    // Abre conexão secundária
    server_output.connect(hostname, port);
    // Pede para associar um novo nick ao ID
    stringstream ID_string;
    ID_string << ID;
    server_output.send(Command::nick(nick, ID_string.str()));
    manager.add_event(server_output.sockfd(), server_event);   
  }
} 

static void msg_event (const string& nick, const string& msg) {
  if (nick.empty() || msg.empty())
    cout << "Nick ou mensagem vazios\n";
  else
    server_input.send(Command::msg(nick, msg));
}

// MAIN

int main(int argc, char **argv) {

	if (argc != 3) {
      fprintf(stderr,"Uso: %s <Endereco IP|Nome> <Porta>\n",argv[0]);
		exit(1);
	}
  
  hostname = argv[1];
  port = atoi(argv[2]);

  // Conexão primária
  server_input.connect(hostname, port);
  // Pede ID
  server_input.send(Command::request_id());
  Command cmd = server_input.receive();
  if (cmd.opcode() == Command::GIVE_ID)
    ID = atoi(cmd.arg(0).c_str());
  printf("Recebido ID %d\n", ID);
  // Prepara e entra em loop
  prompt.init();
  prompt.add_command("/nick", nick_event);
  manager.add_event(STDIN_FILENO, EventManager::Callback(prompt_event));
  manager.loop();
   
  return 0;
}


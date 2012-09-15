
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

// EVENTOS DE PROMPT

static void nick_event (const string& nick, const string& unused) {
  stringstream ID_string;
  ID_string << ID;
  server_input.send(Command::nick(nick, ID_string.str()));
  Command response = server_input.receive();
  cout << (string)response << "\n";
} 

// MAIN

int main(int argc, char **argv) {

	if (argc != 3) {
      fprintf(stderr,"Uso: %s <Endereco IP|Nome> <Porta>\n",argv[0]);
		exit(1);
	}

  // Conexão primária
  server_input.connect(argv[1], atoi(argv[2]));
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


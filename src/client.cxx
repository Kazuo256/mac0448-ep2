
#include <cstdlib>
#include <cstdio>

#include "prompt.h"
#include "TCPconnection.h"
#include "eventmanager.h"
#include "command.h"

#define MAXLINE 4096

using ep2::Prompt;
using ep2::TCPConnection;
using ep2::EventManager;
using ep2::Command;

static EventManager   manager;
static Prompt         prompt;
static TCPConnection  server_output, server_input;
static int            ID = -1;

static EventManager::Status prompt_event () {
  return prompt.send_command(&server_output)
    ? EventManager::CONTINUE
    : EventManager::EXIT;
}

int main(int argc, char **argv) {

	if (argc != 3) {
      fprintf(stderr,"Uso: %s <Endereco IP|Nome> <Porta>\n",argv[0]);
		exit(1);
	}

  // Conexão primária
  server_output.connect(argv[1], atoi(argv[2]));
  // Pede ID
  server_output.send(Command::request_id());
  Command cmd = server_output.receive();
  if (cmd.opcode() == Command::GIVE_ID)
    ID = atoi(cmd.arg(0).c_str());
  printf("Recebido ID %d\n", ID);
  // Prepara e entra em loop
  prompt.init();
  manager.add_event(STDIN_FILENO, EventManager::Callback(prompt_event));
  manager.loop();
   
  return 0;
}



#include <cstdlib>
#include <cstdio>

#include "prompt.h"
#include "TCPconnection.h"
#include "eventmanager.h"

#define MAXLINE 4096

using ep2::Prompt;
using ep2::TCPConnection;
using ep2::EventManager;

static EventManager   manager;
static Prompt         prompt;
static TCPConnection  server;

static EventManager::Status prompt_event () {
  return prompt.send_command(&server)
    ? EventManager::CONTINUE
    : EventManager::EXIT;
}

int main(int argc, char **argv) {

	if (argc != 3) {
      fprintf(stderr,"Uso: %s <Endereco IP|Nome> <Porta>\n",argv[0]);
		exit(1);
	}

  server.connect(argv[1], atoi(argv[2]));
  prompt.init();
  manager.add_event(STDIN_FILENO, EventManager::Callback(prompt_event));
  manager.loop();
   
  return 0;
}


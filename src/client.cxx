
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
  cout << "\n[Received command: " << static_cast<string>(cmd) << "]";
  if (cmd.opcode() == Command::MSG) {
    if (cmd.num_args() < 2)
      cout << "\n[Bad message from server]\n";
    else {
      cout << "\n[Message from '" << cmd.arg(0) << "':] ";
      cout << cmd.arg(1) << "\n> ";
      cout.flush();
    }
  }
  return EventManager::CONTINUE;
}

// EVENTOS DE PROMPT

static bool   secondary_connected = false;
static string current_nick = "";

static void nick_event (const string& nick, const string& unused) {
  if (nick.empty())
    cout << "Nick vazio inválido.\n";
  else {
    if (!secondary_connected) {
      // Abre conexão secundária
      server_output.connect(hostname, port);
      secondary_connected = true;
    }
    // Pede para associar um novo nick ao ID
    stringstream ID_string;
    ID_string << ID;
    server_output.send(Command::nick(nick, ID_string.str()));
    Command response = server_output.receive();
    if (response.opcode() == Command::REFUSE_NICK)
      cout << "[Nick '" << nick << "' was refused]\n";
    else if (response.opcode() == Command::ACCEPT_NICK) {
      cout << "[Now using nick '" << nick << "'].\n";
      manager.add_event(server_output.sockfd(), server_event);   
      current_nick = nick;
    }
    else
      cout << "[Unexpected answer from server]\n";
  }
} 

static void list_event (const string& unused1, const string& unused2) {
  server_input.send(Command::list_request());
  Command response = server_input.receive();
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
    server_input.send(Command::msg(target, msg));
    Command response = server_input.receive();
    if (response.opcode() == Command::MSG_OK)
      cout << "[Message sent to '" << target << "']\n";
    else if (response.opcode() == Command::MSG_FAIL)
      cout << "[Failed to send message to '" << target << "']\n";
    else
      cout << "[Unexpected answer from server]\n";
  }
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
  prompt.add_command("/list", list_event);
  prompt.add_command("/msg", msg_event);
  manager.add_event(STDIN_FILENO, EventManager::Callback(prompt_event));
  manager.loop();
   
  return 0;
}


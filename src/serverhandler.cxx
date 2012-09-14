
#include "serverhandler.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

#include "command.h"
#include "connection.h"

namespace ep2 {

using std::cout;
using std::string;
using std::stringstream;

ServerHandler::ServerHandler () :
  CommandHandler() {}

void ServerHandler::handle (Connection *connection, const Command& cmd) {
  //cout << static_cast<string>(cmd) << "\n";
  Command response = Command::null_cmd();
  stringstream args;
  switch(cmd.opcode()) {
    case Command::REQUEST_ID:
      args << connection->sockfd();
      response = Command::give_id(args.str());
      break;
    default:
      response = cmd;
      break;
  }
  /* Agora envia a resposta para o cliente */
  connection->send(response);
}

} // namespace ep2


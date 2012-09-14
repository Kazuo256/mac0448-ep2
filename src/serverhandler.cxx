
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
  cout << static_cast<string>(cmd) << "\n";
  string packet;
  stringstream args;
  switch(cmd.opcode()) {
    case Command::REQUEST_ID:
      args << connection->sockfd();
      packet = Command::give_id(args.str()).make_packet();
      break;
    default:
      packet = cmd.make_packet();
      if ((fputs(packet.c_str(),stdout)) == EOF) {
         perror("fputs error");
         exit (1);
      }
      break;
  }
  /* Agora envia a resposta para o cliente */
  connection->send(packet);
}

} // namespace ep2


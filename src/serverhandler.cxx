
#include "serverhandler.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>

#include "command.h"
#include "connection.h"

namespace ep2 {

using std::cout;
using std::string;

ServerHandler::ServerHandler () :
  CommandHandler() {}

void ServerHandler::handle (Connection *connection, const Command& cmd) {
  cout << static_cast<string>(cmd) << "\n";
  switch(cmd.opcode()) {
    case Command::REQUEST_ID:
      
      //break;
    default:
      string packet = cmd.make_packet();
      if ((fputs(packet.c_str(),stdout)) == EOF) {
         perror("fputs error");
         exit (1);
      }
      /* Agora re-envia a linha para o cliente */
      connection->send(packet);
      break;
  }
}

} // namespace ep2


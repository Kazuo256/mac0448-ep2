
#include "serverhandler.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

#include "command.h"
#include "connection.h"
#include "serverdata.h"

namespace ep2 {

using std::cout;
using std::string;
using std::stringstream;

ServerHandler::ServerHandler (ServerData* serverdata) :
  CommandHandler(),
  serverdata_(serverdata) {}

void ServerHandler::handle (Connection *client, const Command& cmd) {
  cout << static_cast<string>(cmd) << "\n";
  Command response = Command::null_cmd();
  Command::ArgList arg_list;
  stringstream args;
  switch(cmd.opcode()) {
    case Command::REQUEST_ID:
      args << client->sockfd();
      response = Command::give_id(args.str());
      break;
    case Command::NICK:
      if (serverdata_->used(cmd.arg(0))) {
        response = Command::refuse_nick();
      } else {
        serverdata_->set_user(cmd.arg(0), client);
        response = Command::accept_nick();
      }
      break;
    case Command::LIST_REQUEST:
      serverdata_->get_list(arg_list);
      response = Command::list_response(arg_list);
      break;
    default:
      response = cmd;
      break;
  }
  /* Agora envia a resposta para o cliente */
  client->send(response);
}

} // namespace ep2


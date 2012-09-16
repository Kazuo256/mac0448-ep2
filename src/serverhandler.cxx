
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
  int id = -1;
  Command response = Command::null_cmd();
  Command::ArgList arg_list;
  Connection* resp_connec;
  string sender, oldnick;
  stringstream args;
  switch(cmd.opcode()) {
    case Command::REQUEST_ID:
      args << client->sockfd();
      client->send(Command::give_id(args.str()));
      break;
    case Command::NICK:
      if (serverdata_->used(cmd.arg(0))) {
		    client->send(Command::refuse_nick());
      } else {
        id = atoi(cmd.arg(1).c_str());
        oldnick = serverdata_->get_link(id);
        if (oldnick.size()) {
          serverdata_->erase_user(oldnick);
          serverdata_->remove_link(id);
        }
        serverdata_->set_user(cmd.arg(0), client);
        serverdata_->link_connections(id, cmd.arg(0));
        client->send(Command::accept_nick());
      }
      break;
    case Command::LIST_REQUEST:
      serverdata_->get_list(arg_list);
      client->send(Command::list_response(arg_list));
      break;
    case Command::MSG:
      if (serverdata_->used(cmd.arg(0))) {
        resp_connec = serverdata_->get_connection(cmd.arg(0));
        sender = serverdata_->get_link(client->sockfd());
        resp_connec->send(Command::msg(sender, cmd.arg(1)));
        client->send(Command::msg_ok());
      } else {
        client->send(Command::msg_fail());
      }
      break;
    case Command::SEND:
      sender = serverdata_->get_link(client->sockfd());
      if (serverdata_->used(cmd.arg(0))) {
        resp_connec = serverdata_->get_connection(cmd.arg(0));
        resp_connec->send(Command::send(sender, cmd.arg(1)));
      } else {
        serverdata_->get_connection(sender)
          ->send(Command::send_fail("Usuário não existe"));
      }
      break;
    case Command::ACCEPT:
      if (serverdata_->used(cmd.arg(0))) {
        resp_connec = serverdata_->get_connection(cmd.arg(0));
        sender = serverdata_->get_link(client->sockfd());
        resp_connec->send(Command::accept(sender));
      }
      break;
    case Command::REFUSE:
      if (serverdata_->used(cmd.arg(0))) {
        resp_connec = serverdata_->get_connection(cmd.arg(0));
        sender = serverdata_->get_link(client->sockfd());
        resp_connec->send(Command::send_fail("Usuário recusou o arquivo"));
      }
      break;
    default:
      response = cmd;
      break;
  }
}

} // namespace ep2


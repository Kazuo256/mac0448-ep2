
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
#ifdef EP2_DEBUG
  cout << "[Comando recebido: " << static_cast<string>(cmd) << "]\n";
#endif
  Connection* resp_connec;
  string sender;
  switch(cmd.opcode()) {
    case Command::REQUEST_ID: {
        stringstream args;
        args << client->sockfd();
        client->send(Command::give_id(args.str()));
      }
      break;
    case Command::NICK:
      if (serverdata_->used(cmd.arg(0))) {
		    client->send(Command::refuse_nick());
      } else {
        int id = atoi(cmd.arg(1).c_str());
        string oldnick = serverdata_->get_link(id);
        if (oldnick.size()) {
          serverdata_->erase_user(oldnick);
          serverdata_->remove_link(id);
        }
        serverdata_->set_user(cmd.arg(0), client);
        serverdata_->link_connections(id, cmd.arg(0));
        client->send(Command::accept_nick());
      }
      break;
    case Command::LIST_REQUEST: {
        Command::ArgList arg_list;
        serverdata_->get_list(arg_list);
        client->send(Command::list_response(arg_list));
      }
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
        resp_connec = serverdata_->get_connection(sender);
        if (resp_connec)
          resp_connec->send(Command::send_fail("Usuário não existe"));
      }
      break;
    case Command::ACCEPT:
      if (serverdata_->used(cmd.arg(0))) {
        resp_connec = serverdata_->get_connection(cmd.arg(0));
        sender = serverdata_->get_link(client->sockfd());
        stringstream port;
        port << client->remote_port();
        cout << "PORTA " << port.str() << "/" << client->remote_port() << "\n";
        resp_connec->send(Command::send_ok(client->remote_address(), port.str()));
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
#ifdef EP2_DEBUG
      cout << "[Comando inesperado]\n";
#endif
      break;
  }
}

} // namespace ep2


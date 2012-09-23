
#include "serverhandler.h"

#include <cstdlib>
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
  // Esse código não é exatamente elegante, mas não tivemos tempo de fazer nada melhor.
  switch(cmd.opcode()) {
    // ID requisitado: envia o descritor do socket da conexão como ID.
    case Command::REQUEST_ID: {
        stringstream args; // para transformar o descritor em string
        args << client->sockfd();
        client->send(Command::give_id(args.str()));
      }
      break;
    // Nick requisitado.
    case Command::NICK:
      // Verifica se o nick é válido, isso é, se já não está snedo usado.
      if (serverdata_->used(cmd.arg(0)))
		    client->send(Command::refuse_nick()); // recusa nick inválido
      else {
        // Nick válido, o segundo argumento do comando é o ID da conexão
        // primária do cliente
        int id = atoi(cmd.arg(1).c_str());
        // Remove nick antigo se houver.
        string oldnick = serverdata_->get_link(id);
        if (oldnick.size()) {
          serverdata_->erase_user(oldnick);
          serverdata_->remove_link(id);
        }
        // Associa novo nick ao cliente.
        serverdata_->set_user(cmd.arg(0), client);
        serverdata_->link_connections(id, cmd.arg(0));
        // Envia confirmação para o cliente.
        client->send(Command::accept_nick());
      }
      break;
    case Command::LIST_REQUEST: {
        // Pega a lista de nicks e envia para o cliente.
        Command::ArgList arg_list;
        serverdata_->get_list(arg_list);
        client->send(Command::list_response(arg_list));
      }
      break;
    case Command::MSG:
      // Verifica se o alvo da mensagem existe.
      if (serverdata_->used(cmd.arg(0))) {
        // Pega o alvo e o nick do remetente.
        Connection *target = serverdata_->get_connection(cmd.arg(0));
        string sendernick = serverdata_->get_link(client->sockfd());
        // Manda a mensagem para o alvo e uma confirmação para o remetente.
        target->send(Command::msg(sendernick, cmd.arg(1)));
        client->send(Command::msg_ok());
      } else // alvo não exite, avisa cliente que a mensagem falhou.
        client->send(Command::msg_fail());
      break;
    case Command::SEND: {
        // Pega nick do remetente.
        string sendernick = serverdata_->get_link(client->sockfd());
        // Verifica se o alvo realmente existe.
        if (serverdata_->used(cmd.arg(0))) {
          // Repassa para o alvo o pedido de transferência.
          Connection *target = serverdata_->get_connection(cmd.arg(0));
          target->send(Command::send(sendernick, cmd.arg(1)));
        } else {
          // Alvo não existe, envia de volta pro remetente a falha no envio.
          Connection *sender = serverdata_->get_connection(sendernick);
          if (sender)
            sender->send(Command::send_fail("Usuário não existe"));
        }
      }
      break;
    case Command::ACCEPT:
      // Verifica se o alvo realmente existe.
      if (serverdata_->used(cmd.arg(0))) {
        // Pega o alvo e repassa a confirmação de transferência, junto com as
        // informações necessárias para que os clientes estabeleçam uma conexão
        // entre si.
        Connection *target = serverdata_->get_connection(cmd.arg(0));
        stringstream port; // para transformar unsigned short em string
        port << client->remote_port();
        target->send(Command::send_ok(client->remote_address(), port.str()));
      }
      // O próprio cliente não permite que o usuário envie accept's semd que
      // haja transferências pendentes, então não precisa fazer nada caso o
      // alvo não exista.
      break;
    case Command::REFUSE:
      // Verifica se o alvo realmente existe.
      if (serverdata_->used(cmd.arg(0))) {
        // Pega o alvo e avisa ele que a transferência foi recusada.
        Connection *target = serverdata_->get_connection(cmd.arg(0));
        target->send(Command::send_fail("Usuário recusou o arquivo"));
      }
      // Anolagamente ao ACCEPT, não precisa fazer nada se o alvo não existir.
      break;
    default:
#ifdef EP2_DEBUG
      cout << "[Comando inesperado]\n";
#endif
      break;
  }
}

} // namespace ep2



#include "command.h"

#include <iostream>
#include <sstream>
#include <algorithm>

namespace ep2 {

using std::string;
using std::stringstream;
using std::make_pair;
using std::min;

string Command::make_packet () const {
  // Algoritmo para fazer um pacote a partir de um comando. Ver mais no LEIAME.
  string packet;
  packet += opcode_;
  packet += static_cast<byte>(data_.size());
  for (ArgList::const_iterator it = data_.begin(); it != data_.end(); ++it) {
    packet += static_cast<byte>(it->size());
    packet += *it;
  }
  packet += '\n';
  return packet;
}

#ifdef EP2_DEBUG
Command::operator string () const {
  switch(opcode_) {
    case REQUEST_ID:
      return "requisito de ID.";
    case NICK:
      return string("requisito de nick: ")
             + (data_.size() ? data_[0] : "<empty>");
    case REFUSE_NICK:
      return "nick recusado";
    case ACCEPT_NICK:
      return "nick aceito";
    case DISCONNECT:
    case LIST_REQUEST:
    case CHUNK:
    case ACCEPT:
    case REFUSE:
    case CONTINUE:
    case GIVE_ID:
    case LIST_RESPONSE:
    case MSG_FAIL:
    case MSG_OK:
    case SEND_FAIL:
    case SEND_OK:
    case MSG:
    case SEND:
      return "comando OK";
      // não houve necessiade de depurar esses comandos
    default:
      return "command desconhecido";
  }
}
#endif

Command Command::from_packet (const string& packet) {
  // Algoritmo para fazer um comando a partir de um pacote. Ver mais no LEIAME.
  if (packet.size() <= 0) return disconnect();
  if (packet[0] >= MAX_COMMAND) return Command(255);
  Command cmd(packet[0]);
  size_t args = static_cast<size_t>(packet[1]);
  byte size = 0;
  for (size_t pos = 2, count = 0;
       count < args && pos < packet.size();
       pos += size, ++pos, ++count) {
    size = packet[pos];
    cmd.data_.push_back(
      packet.substr(pos+1, size)
    );
  }
  return cmd;
}

// Comandos usados pelo cliente

Command Command::request_id () {
  return Command(REQUEST_ID);
}

Command Command::nick (const string& name, const string& id) {
  return generic_cmd<NICK>(name, id);
}

Command Command::disconnect () {
  return Command(DISCONNECT);
}

Command Command::list_request () {
  return Command(LIST_REQUEST);
}

Command Command::chunk (const string& data) {
  // Comandos do tipo CHUNK usam os argumentos para passar pedaços de 255 bytes
  // do arquivo sendo transferido.
  Command cmd(CHUNK);
  stringstream args(data);
  int size = data.size();
  char buffer[255+1];
  for (int count = size; count > 0; count -= 255) {
    args.read(buffer, 255);
    size_t n = args.gcount();
    cmd.data_.push_back(string(buffer, n));
  }
  return cmd;
}

Command Command::accept (const string& sender) {
  return generic_cmd<ACCEPT>(sender);
}

Command Command::refuse (const string& sender) {
  return generic_cmd<REFUSE>(sender);
}

Command Command::cont () {
  return Command(CONTINUE);
}

// Comandos usados pelo servidor

Command Command::give_id (const string& id) {
  return generic_cmd<GIVE_ID>(id);
}

Command Command::refuse_nick () {
  return Command(REFUSE_NICK);
}

Command Command::accept_nick () {
  return Command(ACCEPT_NICK);
}

Command Command::list_response (const ArgList& users) {
  // Cada argumento é o nick de um usuário on-line no servidor.
  return Command(LIST_RESPONSE, users);
}

Command Command::msg_fail () {
  return Command(MSG_FAIL);
}

Command Command::msg_ok () {
  return Command(MSG_OK);
}

Command Command::send_fail (const string& info) {
  return generic_cmd<SEND_FAIL>(info);
}

Command Command::send_ok (const string& addr, const string& port) {
  return generic_cmd<SEND_OK>(addr, port);
}

// Comandos usados por ambos

Command Command::msg (const string& nick, const string& msg) {
  return generic_cmd<MSG>(nick, msg);
}

Command Command::send (const string& name, const string& filepath) {
  return generic_cmd<SEND>(name, filepath);
}

} // namespace ep2


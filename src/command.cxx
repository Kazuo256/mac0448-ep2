
#include "command.h"

namespace ep2 {

using std::string;
using std::make_pair;

string Command::make_packet () const {
  string packet;
  packet += opcode_;
  packet += data_.size();
  for (ArgList::const_iterator it = data_.begin(); it != data_.end(); ++it) {
    packet += static_cast<byte>(it->size());
    packet += *it;
  }
  packet += '\n';
  return packet;
}

Command::operator string () const {
  switch(opcode_) {
    case REQUEST_ID:
      return "ID requested.";
    case NICK:
      return string("Nick requested: ")
             + (data_.size() ? data_[0] : "<empty>");
    case REFUSE_NICK:
      return "Nick refused";
    case ACCEPT_NICK:
      return "Nick accepted";
    default:
      return "Unknown command";
  }
}

Command Command::from_packet (const string& packet) {
  Command cmd(packet[0]);
  if (!packet.size()) return disconnect();
  if (packet[0] >= MAX_COMMAND) return Command(255);
  for (size_t pos = 2, count = 0;
       count < static_cast<size_t>(packet[1]) && pos < packet.size();
       pos += packet[pos]+1, ++count)
    cmd.data_.push_back(packet.substr(pos+1, packet[pos]));

  return cmd;
}

// Client commands

Command Command::request_id () {
  return Command(REQUEST_ID, ArgList());
}

Command Command::nick (const string& name, const string& id) {
  return generic_cmd<NICK>(name, id);
}

Command Command::msg (const string& nick, const string& msg) {
  return generic_cmd<MSG>(nick, msg);
}

Command Command::disconnect () {
  return Command(DISCONNECT, ArgList());
}

Command Command::list_request () {
  return Command(LIST_REQUEST, ArgList());
}

// Server Commands

Command Command::give_id (const string& id) {
  return generic_cmd<GIVE_ID>(id);
}

Command Command::refuse_nick () {
  return Command(REFUSE_NICK, ArgList());
}

Command Command::accept_nick () {
  return Command(ACCEPT_NICK, ArgList());
}

Command Command::list_response (const ArgList& arg_list) {
  return Command(LIST_RESPONSE, arg_list);
}

} // namespace ep2


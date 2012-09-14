
#include "command.h"

namespace ep2 {

using std::string;
using std::make_pair;

string Command::make_packet () const {
  string packet;
  packet += opcode_;
  packet += data_.size();
  for (arg_list::const_iterator it = data_.begin(); it != data_.end(); ++it) {
    packet += static_cast<byte>(it->size());
    packet += *it;
  }
  packet += '\n';
  return packet;
}

Command::operator string () const {
  switch(opcode_) {
    case REQUEST_ID:
      return string("ID requested.");
    case NICK:
      return string("Nick requested: ")+data_[0];
    default:
      return "Unknown command";
  }
}

Command Command::from_packet (const string& packet) {
  Command cmd(packet[0]);
  if (packet[0] != NICK && packet[0] != REQUEST_ID &&
      packet[0] != GIVE_ID) return Command(255);
  for (size_t pos = 2, count = 0;
       count < static_cast<size_t>(packet[1]) && pos < packet.size();
       pos += packet[pos]+1, ++count)
    cmd.data_.push_back(packet.substr(pos+1, packet[pos]));

  return cmd;
}

Command Command::request_id () {
  return Command(REQUEST_ID, arg_list());
}

Command Command::nick (const string& name) {
  return Command(NICK, arg_list(1, name));
}

Command Command::disconnect () {
  return Command(DISCONNECT, arg_list());
}

Command Command::give_id (const string& id) {
  return Command(GIVE_ID, arg_list(1, id));
}

} // namespace ep2


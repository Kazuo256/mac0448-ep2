
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
  return from_packet(packet.c_str(), packet.size());
}

Command Command::from_packet (const char* packet, size_t n) {
  if (n <= 0) return disconnect();
  if (packet[0] >= MAX_COMMAND) return Command(255);
  Command cmd(packet[0]);
  size_t args = static_cast<size_t>(packet[1]);
  string packet_str(packet, packet+n);
  byte size = 0;
  for (size_t pos = 2, count = 0;
       count < args && pos < n;
       pos += size, ++pos, ++count) {
    size = packet[pos];
    cmd.data_.push_back(
      packet_str.substr(pos+1, size)
    );
  }
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

Command Command::send (const string& name, const string& filepath) {
  return generic_cmd<SEND>(name, filepath);
}

Command Command::list_request () {
  return Command(LIST_REQUEST, ArgList());
}

Command Command::chunk (const string& data) {
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

} // namespace ep2



#include "prompt.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include <sstream>
#include <iostream>

#include "command.h"
#include "connection.h"

#define MAXLINE 4096

namespace ep2 {

using std::string;
using std::vector;
using std::map;
using std::cin;
using std::cout;
using std::stringstream;

static Command handle_disconnect (const string& arg, const string& data) {
  puts(string("ADEUS OTÁRIOS").c_str());
  return Command::null_cmd();
}

static Command handle_nick (const string& arg, const string& data) {
  puts((string("Loging with nick '")+arg+"'").c_str());
  return Command::nick(arg);
}

static Command handle_msg (const string& arg, const string& data) {
  puts((string("Falando com o brother '")+arg+"' msg '"+data+"'").c_str());
  //return "msg\n";
  return Command::request_id();
}

static Command handle_send (const string& arg, const string& data) {
  puts((string("Aceita o role ai brother '")+arg+"' dado '"+data+"'").c_str());
  return Command::null_cmd();
}

static Command handle_list (const string& arg, const string& data) {
  puts((string("Manda os brothers logado ai tio ")).c_str());
  return Command::null_cmd();
}

static Command handle_exit (const string& arg, const string& data) {
  puts(string("ADEUS OTÁRIOS, EXITEI").c_str());
  return Command::null_cmd();
}

static Command handle_accept (const string& arg, const string& data) {
  puts(string("Pode manda os role").c_str());
  return Command::null_cmd();
}

static Command handle_refuse (const string& arg, const string& data) {
  puts(string("Quero sabe disso ai n tio, flwz").c_str());
  return Command::null_cmd();
}

void Prompt::init () {
  cmd_map_["/disconnect"] = handle_disconnect;
  cmd_map_["/nick"] = handle_nick;
  cmd_map_["/msg"] = handle_msg;
  cmd_map_["/send"] = handle_send;
  cmd_map_["/list"] = handle_list;
  cmd_map_["/exit"] = handle_exit;
  cmd_map_["/accept"] = handle_accept;
  cmd_map_["/refuse"] = handle_refuse;
}

bool Prompt::send_command (Connection *server) {
  string line;
  string cmd, arg, data;
  getline(cin, line);
  if (cin.eof()) return false;
  stringstream tokens(line);
  tokens >> cmd >> arg >> data;
  /* Escreve a linha lida no socket */
  server->send(check_cmd(cmd, arg, data));
  Command response = server->receive();
  if (response.opcode() == Command::GIVE_ID)
    cout << "Recebeu ID " << response.arg(0) << "\n";
  cout << (string)response << "\n";
  return true;
}

Command Prompt::check_cmd (const string& cmd, const string& arg,
                        const string& data) {
  CommandMap::iterator it = cmd_map_.find(cmd);
  if (it == cmd_map_.end()) return Command::null_cmd();
  return it->second(arg, data);
}

} // namespace ep2


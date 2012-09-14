
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

static string handle_disconnect (const string& arg, const string& data) {
  puts(string("ADEUS OTÁRIOS").c_str());
  return "disconnect\n";
}

static string handle_nick (const string& arg, const string& data) {
  puts((string("Loging with nick '")+arg+"'").c_str());
  return Command::nick(arg).make_packet();
}

static string handle_msg (const string& arg, const string& data) {
  puts((string("Falando com o brother '")+arg+"' msg '"+data+"'").c_str());
  //return "msg\n";
  return Command::request_id().make_packet();
}

static string handle_send (const string& arg, const string& data) {
  puts((string("Aceita o role ai brother '")+arg+"' dado '"+data+"'").c_str());
  return "send\n";
}

static string handle_list (const string& arg, const string& data) {
  puts((string("Manda os brothers logado ai tio ")).c_str());
  return "list\n";
}

static string handle_exit (const string& arg, const string& data) {
  puts(string("ADEUS OTÁRIOS, EXITEI").c_str());
  return "exit\n";
}

static string handle_accept (const string& arg, const string& data) {
  puts(string("Pode manda os role").c_str());
  return "accept\n";
}

static string handle_refuse (const string& arg, const string& data) {
  puts(string("Quero sabe disso ai n tio, flwz").c_str());
  return "refuse\n";
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
  string packet, response;
  char cmdline[MAXLINE+1];
  string line;
  string cmd, arg, data;
  getline(cin, line);
  if (cin.eof()) return false;
  stringstream tokens(line);
  tokens >> cmd >> arg >> data;
  packet = check_cmd(cmd, arg, data);
  /* Escreve a linha lida no socket */
  server->send(packet);
  response = server->receive();
  Command resp_cmd = Command::from_packet(response);
  if (resp_cmd.opcode() == Command::GIVE_ID)
    cout << "Recebeu ID " << resp_cmd.arg(0) << "\n";
  cout << (string)resp_cmd << "\n";
  return true;
}

string Prompt::check_cmd (const string& cmd, const string& arg,
                        const string& data) {
  CommandMap::iterator it = cmd_map_.find(cmd);
  if (it == cmd_map_.end()) return "oi\n";
  return it->second(arg, data);
}

} // namespace ep2


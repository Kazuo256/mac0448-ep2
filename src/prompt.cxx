
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
using std::cin;
using std::cout;
using std::stringstream;

//static Command handle_disconnect (const string& arg, const string& data) {
//  puts(string("ADEUS OTÁRIOS").c_str());
//  return Command::null_cmd();
//}
//
//static Command handle_nick (const string& arg, const string& data) {
//  puts((string("Loging with nick '")+arg+"'").c_str());
//  return Command::nick(arg);
//}
//
//static Command handle_msg (const string& arg, const string& data) {
//  puts((string("Falando com o brother '")+arg+"' msg '"+data+"'").c_str());
//  //return "msg\n";
//  return Command::request_id();
//}
//
//static Command handle_send (const string& arg, const string& data) {
//  puts((string("Aceita o role ai brother '")+arg+"' dado '"+data+"'").c_str());
//  return Command::null_cmd();
//}
//
//static Command handle_list (const string& arg, const string& data) {
//  puts((string("Manda os brothers logado ai tio ")).c_str());
//  return Command::null_cmd();
//}
//
//static Command handle_exit (const string& arg, const string& data) {
//  puts(string("ADEUS OTÁRIOS, EXITEI").c_str());
//  return Command::null_cmd();
//}
//
//static Command handle_accept (const string& arg, const string& data) {
//  puts(string("Pode manda os role").c_str());
//  return Command::null_cmd();
//}
//
//static Command handle_refuse (const string& arg, const string& data) {
//  puts(string("Quero sabe disso ai n tio, flwz").c_str());
//  return Command::null_cmd();
//}
//
void Prompt::init () {
  printf("> ");
  fflush(stdout);
}

void Prompt::add_command (const string& cmd, const CommandHandler& handler) {
  cmd_map_[cmd] = handler;
}

bool Prompt::check_input () {
  string line;
  string cmd, arg, data;
  getline(cin, line);
  if (cin.eof()) {
    putchar('\n');
    return false;
  }
  stringstream tokens(line);
  stringstream msg;
  tokens >> cmd >> arg;
  tokens.get(*msg.rdbuf());
  data = msg.str();
  run_cmd(cmd, arg, data);
  printf("> ");
  fflush(stdout);
  return true;
}

void Prompt::run_cmd (const string& cmd, const string& arg,
                      const string& data) {
  CommandMap::iterator it = cmd_map_.find(cmd);
  if (it == cmd_map_.end())
    cout << "Unknown command " << cmd << "\n";
  else
    it->second(arg, data);
}

} // namespace ep2


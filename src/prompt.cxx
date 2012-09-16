
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
  while (tokens.peek() == ' ') tokens.get();
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


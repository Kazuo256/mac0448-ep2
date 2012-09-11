
#include "prompt.h"

#include <cstdio>
#include <cstring>
#include <vector>

#define MAXLINE 4096

namespace ep2 {

using std::string;
using std::vector;
using std::map;

static void handle_nick (const string& arg, const string& data) {
  puts((string("Loging with nick '")+arg+"'").c_str());
}

void Prompt::init () {
  cmd_map["/nick"] = handle_nick;
}

void Prompt::run () {
  while (true) {
    char cmdline[MAXLINE+1];
    char *saveptr = NULL;
    char *cmd, *arg, *data;
    fgets(cmdline, MAXLINE, stdin);
    cmd = strtok_r(cmdline, " \t\n", &saveptr);
    arg = strtok_r(NULL, " \t\n", &saveptr);
    data = strtok_r(NULL, "\n", &saveptr);
    if (cmd)
      cmd_map[cmd] (arg ? arg : "", data ? data : "");
  }
}

} // namespace ep2


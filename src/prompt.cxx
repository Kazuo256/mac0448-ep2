
#include "prompt.h"

#include <cstdlib>
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
    if (!fgets(cmdline, MAXLINE, stdin)) break;
    cmd = strtok_r(cmdline, " \t\n", &saveptr);
    arg = strtok_r(NULL, " \t\n", &saveptr);
    data = strtok_r(NULL, "\n", &saveptr);
    if (!check_cmd(cmd?cmd:"", arg?arg:"", data?data:"")) {
      /* Escreve a linha lida no socket */
      write(sockfd_, "oi\n", strlen(cmdline));

      /* Lê a linha reenviada pelo servidor e escreve na saída padrão */
      int n=read(sockfd_, cmdline, MAXLINE);
      cmdline[n]=0;
      if ((fputs(cmdline,stdout)) == EOF) {
        perror("fputs error");
        exit (1);
      }
    }
  }
}

bool Prompt::check_cmd (const string& cmd, const string& arg,
                        const string& data) {
  CommandMap::iterator it = cmd_map.find(cmd);
  if (it == cmd_map.end()) return false;
  it->second(arg, data);
  return true;
}

} // namespace ep2


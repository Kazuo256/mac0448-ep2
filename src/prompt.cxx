
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

static void handle_disconnect (const string& arg, const string& data) {
  puts(string("ADEUS OTÁRIOS").c_str());
}

static void handle_nick (const string& arg, const string& data) {
  puts((string("Loging with nick '")+arg+"'").c_str());
}

static void handle_msg (const string& arg, const string& data) {
  puts((string("Falando com o brother '")+arg+"' msg '"+data+"'").c_str());
}

static void handle_send (const string& arg, const string& data) {
  puts((string("Aceita o role ai brother '")+arg+"' dado '"+data+"'").c_str());
}

static void handle_list (const string& arg, const string& data) {
  puts((string("Manda os brothers logado ai tio ")).c_str());
}

static void handle_exit (const string& arg, const string& data) {
  puts(string("ADEUS OTÁRIOS, EXITEI").c_str());
}

static void handle_accept (const string& arg, const string& data) {
  puts(string("Pode manda os role").c_str());
}

static void handle_refuse (const string& arg, const string& data) {
  puts(string("Quero sabe disso ai n tio, flwz").c_str());
}

void Prompt::init () {
  cmd_map["/disconnect"] = handle_disconnect;
  cmd_map["/nick"] = handle_nick;
  cmd_map["/msg"] = handle_msg;
  cmd_map["/send"] = handle_send;
  cmd_map["/list"] = handle_list;
  cmd_map["/exit"] = handle_exit;
  cmd_map["/accept"] = handle_accept;
  cmd_map["/refuse"] = handle_refuse;
}

void Prompt::run () {
  while (true) {
    char cmdline[MAXLINE+1];
    char *saveptr = NULL;
    char *cmd, *arg, *data;
    if (!fgets(cmdline, MAXLINE, stdin)) break;
    cmd = strtok_r(cmdline, " \t\n", &saveptr);
    if (cmd == NULL) continue;
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


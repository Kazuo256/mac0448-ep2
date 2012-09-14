
#include <vector>
#include <map>
#include <string>

#include "connection.h"
#include "TCPconnection.h"
#include "inputlistener.h"
#include "command.h"
#include "serverhandler.h"

using std::vector;
using std::map;
using std::string;

using ep2::Connection;
using ep2::TCPConnection;
using ep2::InputListener;
using ep2::Command;
using ep2::ServerHandler;

typedef map<int, Connection*> client_table;

static Connection* get_client (const client_table& table, int fd) {
  client_table::const_iterator seek = table.find(fd);
  return (seek != table.end()) ? seek->second : NULL;
}

static void clear_clients (client_table& table) {
  for (client_table::iterator it = table.begin(); it != table.end(); ++it)
    delete it->second;
  table.clear();
}

int main (int argc, char **argv) {
	if (argc != 2) {
      fprintf(stderr,"Uso: %s <Porta>\n",argv[0]);
		exit(1);
	}
  TCPConnection server;
  server.host(atoi(argv[1]));
  //server.accept();

  client_table table;
  InputListener listener;
  listener.add_input(STDIN_FILENO);
  listener.add_input(server.sockfd());
  while (true) {
    vector<int> fds;
    listener.poll(fds);
    for (vector<int>::iterator it = fds.begin(); it != fds.end(); ++it) {
      Connection *client;
      if (*it == server.sockfd()) {
        client = server.accept();
        table[client->sockfd()] = client;
        listener.add_input(client->sockfd());
      }
      else if ((client = get_client(table, *it)) != NULL) {
        string packet = client->receive();
        // Check end of client msgs
        if (!packet.size()) {
          table.erase(*it);
          delete client;
        }
        else {
          /* Lê a linha enviada pelo cliente e escreve na saída padrão */
          Command cmd = Command::from_packet(packet);
          ServerHandler().handle(cmd);
          if ((fputs(packet.c_str(),stdout)) == EOF) {
             perror("fputs error");
             exit (1);
          }
          /* Agora re-envia a linha para o cliente */
          client->send(packet);
        }
      }
      else if (*it == STDIN_FILENO) {
        clear_clients(table);
        return 0;
      }
    }
	}
	return 0;
}


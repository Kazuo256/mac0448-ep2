
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
    //pid_t childpid;
    //if ( (childpid = fork()) == 0) { /* Se for zero está no processo filho */
    //  string packet;
    //  /* Processa tudo que for enviado do cliente conectado */
    //  while ((packet = client->receive()).size()) {
    //     /* Lê a linha enviada pelo cliente e escreve na saída padrão */
    //     Command cmd = Command::from_packet(packet);
    //     ServerHandler().handle(cmd);
    //     if ((fputs(packet.c_str(),stdout)) == EOF) {
    //        perror("fputs error");
    //        exit (1);
    //     }
    //     /* Agora re-envia a linha para o cliente */
    //     client->send(packet);
    //  }
    //  delete client;
    //  return 0;
    //  /******************************************************/
    //  //printf(
    //  //   "Dados do socket remoto: (IP: %s, PORTA: %d desconectou)\n",
    //  //   inet_ntop(AF_INET, &(dadosRemoto.sin_addr).s_addr, enderecoRemoto,
    //  //             sizeof(enderecoRemoto)),
    //  //   ntohs(dadosRemoto.sin_port)
    //  // );
    //  //exit (0);         /* Termina o processo filho */
    //}
    ///* Se for o pai continua a execução aqui... */
    //delete client;
	}
	return 0;
}


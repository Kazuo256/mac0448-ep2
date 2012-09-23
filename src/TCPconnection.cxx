
#include "TCPconnection.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string>
#include <iostream>

#include "command.h"

#define LISTENQ 1
#define MAXDATASIZE 100
#define MAXLINE 4096

namespace ep2 {

using std::string;
using std::cout;

TCPConnection::TCPConnection () :
  Connection(socket(AF_INET, SOCK_STREAM, 0)) {}

TCPConnection::TCPConnection (int sockfd) :
  Connection(sockfd) {}

void TCPConnection::host (unsigned short port) {
  set_local_info(AF_INET, INADDR_ANY, port);
  bind();
	if (listen(sockfd(), LISTENQ) == -1) {
		perror("listen");
		exit(1);
	}
}

Connection* TCPConnection::accept () {
	int                 connfd;
  struct sockaddr_in  remote_info;
	int                 remote_info_size;
	remote_info_size = sizeof(remote_info);
	if ((connfd = ::accept(sockfd(), (struct sockaddr*) &remote_info,
                       (socklen_t *) &remote_info_size)) == -1 ) {
		perror("accept");
		exit(1);
  }
  /* Imprimindo os dados do socket remoto */
  cout  << "[Nova conexão com "
        << local_address() << ":" << local_port() << "]\n";
  // TODO comment
  TCPConnection *accepted = new TCPConnection(connfd);
  accepted->set_local_info(this);
  accepted->set_remote_info(remote_info);
  return accepted;
}

bool TCPConnection::connect (const string& hostname, unsigned short port) {
  /* Para uso com o gethostbyname */
  struct  hostent *hptr;
  char    enderecoIPServidor[INET_ADDRSTRLEN];

  if ( (hptr = gethostbyname(hostname.c_str())) == NULL) {
    fprintf(stderr,"gethostbyname :(\n");
    exit(1);
  }
  /* Verificando se de fato o endereço é AF_INET */
  if (hptr->h_addrtype != AF_INET) {
    fprintf(stderr,"h_addrtype :(\n");
    exit(1);
  }
  /* Salvando o IP do servidor (Vai pegar sempre o primeiro IP
  * retornado pelo DNS para o nome passado no shell)*/
  if (inet_ntop(AF_INET, hptr->h_addr_list[0], enderecoIPServidor,
                sizeof(enderecoIPServidor)) == NULL) {
    fprintf(stderr,"inet_ntop :(\n");
    exit (1);
  }

  /* A partir deste ponto o endereço IP do servidor estará na string
  * enderecoIPServidor */
  printf("[Conectando no servidor no IP %s]\n",enderecoIPServidor);
  printf("[o comando 'exit' encerra a conexão]\n");

  set_remote_info(AF_INET, enderecoIPServidor, port);

	if (::connect(sockfd(), remote_info(), info_size()) < 0) {
		perror("connect error");
		exit(1);
	}
   
  /* Imprimindo os dados do socket local */
  set_local_info();

#ifdef EP2_DEBUG
  cout << "[Socket local " << local_address() << ":" << local_port() << "]\n";
#endif

  return true;
}

Command TCPConnection::receive () {
  char cmdline[MAXLINE+1];
  int n=read(sockfd(), cmdline, MAXLINE);
	if (n < 0) {
		perror("read error");
		exit(1);
	}
  cmdline[n]=0;
  return Command::from_packet(string(cmdline, n));
}

void TCPConnection::send (const Command& cmd) {
  string packet = cmd.make_packet();
  size_t size = packet.size();
  if (write(sockfd(), packet.c_str(), size) < 0) {
    perror("write error");
    exit(1);
  }
}

} // namespace ep2


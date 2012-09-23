
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
using std::cerr;

// Esse construtor cria um novo socket TCP.
TCPConnection::TCPConnection () :
  Connection(socket(AF_INET, SOCK_STREAM, 0)) {}

// Esse contrutor é apenas usado internamente para garantir a funcionalidade do
// método accept().
TCPConnection::TCPConnection (int sockfd) :
  Connection(sockfd) {}

void TCPConnection::host (unsigned short port) {
  // Inicializa endereço local e dá bind().
  set_local_info(AF_INET, INADDR_ANY, port);
  bind();
  // Prepara para ouvir conexões.
	if (listen(sockfd(), LISTENQ) == -1) {
		perror("TCP::host - listen error");
		exit(1);
	}
}

Connection* TCPConnection::accept () {
  // Usa accept() do C para receber conexões.
	int                 connfd;
  struct sockaddr_in  remote_info;
	socklen_t           remote_info_size;
	remote_info_size = sizeof(remote_info);
	if ((connfd = ::accept(sockfd(), (struct sockaddr*) &remote_info,
                         &remote_info_size)) == -1 ) {
		perror("TCP::accept - accept error");
		exit(1);
  }
  // Cria uma nova conexão TCP a partir dos dados obtidos e da conexão atual.
  TCPConnection *accepted = new TCPConnection(connfd);
  accepted->set_local_info(this);
  accepted->set_remote_info(remote_info);
#ifdef EP2_DEBUG
  // Imprime as informações remotas da nova conexão.
  cout  << "[Nova conexão com "
        << accepted->remote_address() << ":"
        << accepted->remote_port() << "]\n";
#endif
  // Devolve a nova conexão, pronta para ser usada.
  return accepted;
}

bool TCPConnection::connect (const string& hostname, unsigned short port) {
  // Usa gethostbyname() para obter o endereço verdadeiro do host.
  struct  hostent *hptr;
  char    enderecoIPServidor[INET_ADDRSTRLEN];
  if ( (hptr = gethostbyname(hostname.c_str())) == NULL) {
    cerr << "TCP::connect - gethostbyname error\n";
    exit(1);
  }
  // Verificando se de fato o endereço é AF_INET
  if (hptr->h_addrtype != AF_INET) {
    cerr << "TCP::connect - h_addrtype mismatch\n";
    exit(1);
  }
  //set_remote_info(AF_INET, hptr->h_addr_list[0], port);
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


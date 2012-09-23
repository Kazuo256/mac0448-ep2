
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

// Usado para obter o endereço do host a partir o resultado de gethostbyname().
static string get_hostaddr (const char* addr) {
  char addr_str[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, addr, addr_str, INET_ADDRSTRLEN) == NULL) {
    cerr << "get_hostaddr - inet_ntop error\n";
    exit (1);
  }
  return addr_str;
}

bool TCPConnection::connect (const string& hostname, unsigned short port) {
  // Usa gethostbyname() para obter o endereço verdadeiro do host.
  struct  hostent *hptr;
  if ( (hptr = gethostbyname(hostname.c_str())) == NULL) {
    cerr << "TCP::connect - gethostbyname error\n";
    exit(1);
  }
  // Verificando se de fato o endereço é AF_INET
  if (hptr->h_addrtype != AF_INET) {
    cerr << "TCP::connect - h_addrtype mismatch\n";
    exit(1);
  }
  // Configuramos as informações remotas da conexão usando sempre o primeiro IP
  // retornado pelo DNS para o argumento hostname.
  set_remote_info(AF_INET, get_hostaddr(hptr->h_addr_list[0]), port);
#ifdef EP2_DEBUG
  cout  << "[Conectando com "
        << remote_address() << ":" << remote_port() << "]\n";
#endif
  // Tenta conectar com o host.
	if (::connect(sockfd(), remote_info(), info_size()) < 0) {
		perror("TCP::connect - connect error");
		exit(1);
	}
  // Atualiza as informações locais da conexão
  // (a chamada a ::connect garante endereço e porta locais para o socket)
  set_local_info();
#ifdef EP2_DEBUG
  // Imprime resultados
  cout  << "[Conexão estabelicida de "
        << local_address() << ":" << local_port() << " para "
        << remote_address() << ":" << remote_port() << "]\n";
#endif
  return true;
}

Command TCPConnection::receive () {
  // Usa read() para ler pacote vindos da rede através da conexão.
  char cmdline[MAXLINE+1];
  int n=read(sockfd(), cmdline, MAXLINE);
	if (n < 0) {
		perror("TCP::receive - read error");
		exit(1);
	}
  cmdline[n]=0;
  // Transforma em Command e devolve.
  return Command::from_packet(string(cmdline, n));
}

void TCPConnection::send (const Command& cmd) {
  // Contrói pacote a partir do objeto Command.
  string packet = cmd.make_packet();
  // Usa write() para enviar o pacote para a rede através da conexão.
  size_t size = packet.size();
  if (write(sockfd(), packet.c_str(), size) < 0) {
    perror("TCP::send - write error");
    exit(1);
  }
}

} // namespace ep2


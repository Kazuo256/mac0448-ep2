
#include "UDPconnection.h"

#include <cstdlib>
#include <cstdio>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <iostream>

#include "command.h"

#define MAXLINE 4096

namespace ep2 {

using std::string;
using std::cout;

UDPConnection::UDPConnection () :
  Connection(socket(AF_INET, SOCK_DGRAM, 0)) {}

void UDPConnection::host (unsigned short port) {
  set_local_info(AF_INET, INADDR_ANY, port);
  bind();
}

Connection* UDPConnection::accept () {
  struct sockaddr_in  remote_info;
	int                 remote_info_size = sizeof(remote_info);
  int                 n;
	char                enderecoRemoto[INET_ADDRSTRLEN];
  char                recvline[MAXLINE+1];

  cout << "[Aceitando conexão UDP]\n";
  n = recvfrom(sockfd(), recvline, MAXLINE, 0, (struct sockaddr*)&remote_info,
               (socklen_t*)&remote_info_size);
  recvline[n] = '\0';
  printf("[UDP connection request: %s]\n", recvline);
  printf("[Socket data size: %d]\n", remote_info_size);
  /* Imprimindo os dados do socket remoto */
  printf("[Dados do socket remoto: (IP: %s, PORTA: %d)]\n",
         inet_ntop(
           AF_INET,
           &(remote_info.sin_addr).s_addr,
           enderecoRemoto,
           sizeof(enderecoRemoto)
          ),
         ntohs(remote_info.sin_port));
  UDPConnection *accepted = new UDPConnection();
  accepted->set_remote_info(remote_info);
  if (::connect(accepted->sockfd(), accepted->remote_info(), info_size()) < 0) {
		perror("connect error");
    delete accepted;
		exit(1);
  }
  accepted->set_local_info();
#ifdef EP2_DEBUG
  cout  << "[Socket local "
        << accepted->local_address() << ":" << accepted->local_port() << "]\n";
#endif
  unsigned short port = htons(accepted->local_port());
  if (::sendto(sockfd(), (const void*)&port, sizeof(port), 0,
               (struct sockaddr*)&remote_info, remote_info_size) < 0) {
    perror("UDP::accept - connection confirmation error");
    delete accepted;
    exit(1);
  }
#ifdef EP2_DEBUG
  cout << "[UDP conectou]\n";
#endif
  return accepted;
}

bool UDPConnection::connect (const string& hostname, unsigned short port) {
  /* Para uso com o gethostbyname */
  struct    hostent *hptr;
  char      enderecoIPServidor[INET_ADDRSTRLEN];

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
  printf("[Conectando no IP %s]\n",enderecoIPServidor);

  set_remote_info(AF_INET, string(enderecoIPServidor, INET_ADDRSTRLEN), port);

	if (::connect(sockfd(), remote_info(), info_size()) < 0) {
		perror("UDP::connect() - connect error");
		exit(1);
	}

  set_local_info();

#ifdef EP2_DEBUG
  cout << "[Socket local " << local_address() << ":" << local_port() << "]\n";
#endif

  if (::send(sockfd(), "connect_me", sizeof "connect_me", 0) < 0) {
    perror("UDP::connect() - connection request error");
    exit(1);
  }

#ifdef EP2_DEBUG
  cout << "[Enviou pedido de conexão]\n";
#endif

  char      recvline[MAXLINE+1];
  ssize_t   n;
  n = recv(sockfd(), recvline, MAXLINE, 0);
  recvline[n] = '\0';

  printf(
    "[UDP connection response: %hu]\n",
    ntohs(*(unsigned short*)recvline)
  );

  set_remote_info(AF_INET, remote_address(), ntohs(*(unsigned short*)recvline));

	if (::connect(sockfd(), remote_info(), info_size()) < 0) {
		perror("UDP::connect() - connect 2 error");
		exit(1);
	}

  return true;
}

Command UDPConnection::receive() {
  char cmdline[MAXLINE+1];
  int n=recv(sockfd(), cmdline, MAXLINE, 0);
	if (n < 0) {
		perror("read error");
		exit(1);
	}
  cmdline[n]=0;
  return Command::from_packet(string(cmdline, n));
}

void UDPConnection::send (const Command& cmd) {
  string packet = cmd.make_packet();
  size_t size = packet.size();
  if (::send(sockfd(), packet.c_str(), size, 0) < 0) {
    perror("write error");
    exit(1);
  }
}

} // namespace ep2


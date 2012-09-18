
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
#include <poll.h>

#include <string>
#include <iostream>

#include "command.h"
#include "serverhandler.h"

#define LISTENQ 1
#define MAXDATASIZE 100
#define MAXLINE 4096

namespace ep2 {

using std::string;
using std::cout;

/*
 *  struct sockaddr {
 *    sa_family_t sa_family;
 *    char        sa_data[14];
 *  };
 *
 *  struct sockaddr_in {
 *    short            sin_family;   // e.g. AF_INET
 *    unsigned short   sin_port;     // e.g. htons(3490)
 *    struct in_addr   sin_addr;     // see struct in_addr, below
 *    char             sin_zero[8];  // zero this if you want to
 *  };
 *
 *  struct in_addr {
 *    unsigned long s_addr;  // load with inet_aton()
 *  };
 *
 */

TCPConnection::TCPConnection () :
  Connection(socket(AF_INET, SOCK_STREAM, 0)) {
  bzero(&local_info_, sizeof(local_info_));
  bzero(&remote_info_, sizeof(remote_info_));  
}

TCPConnection::TCPConnection (int sockfd) :
  Connection(sockfd) {
  bzero(&local_info_, sizeof(local_info_));
  bzero(&remote_info_, sizeof(remote_info_));
}

TCPConnection::~TCPConnection () {
  if (remote_addr_.size()) {
    printf(
       "Dados do socket remoto: (IP: %s, PORTA: %d desconectou)\n",
       remote_addr_.c_str(),
       ntohs(remote_info_.sin_port)
     );
  }
}

void TCPConnection::host (unsigned short port) {
	local_info_.sin_family      = AF_INET;
	local_info_.sin_addr.s_addr = htonl(INADDR_ANY);
	local_info_.sin_port        = htons(port);
	if (bind(sockfd(),
           (struct sockaddr*)&local_info_, sizeof(local_info_)) == -1) {
		perror("bind");
		exit(1);
	}
	if (listen(sockfd(), LISTENQ) == -1) {
		perror("listen");
		exit(1);
	}
}

Connection* TCPConnection::accept () {
	int                 connfd;
  struct sockaddr_in  remote_info;
	int                 remote_info_size;
	char                enderecoRemoto[MAXDATASIZE + 1];
	remote_info_size = sizeof(remote_info_);
	if ((connfd = ::accept(sockfd(), (struct sockaddr*) &remote_info,
                       (socklen_t *) &remote_info_size)) == -1 ) {
		perror("accept");
		exit(1);
  }
  /* Imprimindo os dados do socket remoto */
  printf("[Dados do socket remoto: (IP: %s, PORTA: %d conectou)]\n",
         inet_ntop(
           AF_INET,
           &(remote_info.sin_addr).s_addr,
           enderecoRemoto,
           sizeof(enderecoRemoto)
          ),
         ntohs(remote_info.sin_port));
  TCPConnection *accepted = new TCPConnection(connfd);
  accepted->local_info_  = local_info_;
  accepted->remote_info_ = remote_info;
  accepted->remote_addr_ = enderecoRemoto;
  return accepted;
}

bool TCPConnection::connect (const string& hostname, unsigned short port) {
  int     dadosLocalLen;
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

	bzero(&remote_info_, sizeof(remote_info_));
  dadosLocalLen=sizeof(local_info_);
  bzero(&local_info_, dadosLocalLen);
	remote_info_.sin_family = AF_INET;
	remote_info_.sin_port   = htons(port);

  /* O endereço IP passado para a função é o que foi retornado na gethostbyname */
	if (inet_pton(AF_INET, enderecoIPServidor, &remote_info_.sin_addr) <= 0) {
		perror("inet_pton error");
		exit(1);
	}

	if (::connect(sockfd(), (struct sockaddr *) &remote_info_,
              sizeof(remote_info_)) < 0) {
		perror("connect error");
		exit(1);
	}
   
  /* Imprimindo os dados do socket local */
  if (getsockname(sockfd(), (struct sockaddr*)&local_info_,
                  (socklen_t*)&dadosLocalLen)) {
  	perror("getsockname error");
  	exit(1);
  }

  printf(
    "Dados do socket local: (IP: %s, PORTA: %d)\n",
    local_address().c_str(),
    ntohs(local_info_.sin_port)
  );
  /***************************************/
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
  return Command::from_packet(cmdline, n);
}

void TCPConnection::send (const Command& cmd) {
  string packet = cmd.make_packet();
  size_t size = packet.size();
  if (write(sockfd(), packet.c_str(), size) < 0) {
    perror("write error");
    exit(1);
  }
}

unsigned short TCPConnection::local_port () const {
  return ntohs(local_info_.sin_port);
}

string TCPConnection::local_address () const {
  char addr[INET_ADDRSTRLEN];
  return
    inet_ntop(AF_INET, &local_info_.sin_addr.s_addr, addr, INET_ADDRSTRLEN);
}

unsigned short TCPConnection::remote_port () const {
  return ntohs(remote_info_.sin_port);
}

string TCPConnection::remote_address () const {
  char addr[INET_ADDRSTRLEN];
  return
    inet_ntop(AF_INET, &remote_info_.sin_addr.s_addr, addr, INET_ADDRSTRLEN);
}


} // namespace ep2


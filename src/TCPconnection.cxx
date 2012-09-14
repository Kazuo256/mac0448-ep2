
#include "TCPconnection.h"

#define _GNU_SOURCE
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

#include "command.h"
#include "serverhandler.h"

#define LISTENQ 1
#define MAXDATASIZE 100
#define MAXLINE 4096

namespace ep2 {

using std::string;

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

static void wait_input (int fd) {
  struct pollfd poll_list;
  poll_list.fd = fd;
  poll_list.events = POLLIN;
  poll_list.revents = 0;
  int n = poll(&poll_list, 1, -1);
  printf("poll returned: %d\n", n);
}

Connection* TCPConnection::accept () {
	int    connfd;
	int    remote_info_size;
	char   enderecoRemoto[MAXDATASIZE + 1];
	ssize_t  n;
	pid_t  childpid;
	char	recvline[MAXLINE + 1];
	remote_info_size = sizeof(remote_info_);
	if ((connfd = ::accept(sockfd(), (struct sockaddr*) &remote_info_,
                       (socklen_t *) &remote_info_size)) == -1 ) {
		perror("accept");
		exit(1);
  }
  /* Imprimindo os dados do socket remoto */
  printf("Dados do socket remoto: (IP: %s, PORTA: %d conectou)\n",
         inet_ntop(
           AF_INET,
           &(remote_info_.sin_addr).s_addr,
           enderecoRemoto,
           sizeof(enderecoRemoto)
          ),
         ntohs(remote_info_.sin_port));
  remote_addr_ = enderecoRemoto;
  return new TCPConnection(connfd);
	//for ( ; ; ) {
  //  // Com isso não precisa usar o getpeername. Antes no lugar dos dados do
  //  // remote_info_, tinha NULL
	//	if ((connfd = ::accept(sockfd(), (struct sockaddr*) &remote_info_,
  //                       (socklen_t *) &remote_info_size)) == -1 ) {
	//		perror("accept");
	//		exit(1);
  //	}
  //  /* Imprimindo os dados do socket remoto */
  //  printf("Dados do socket remoto: (IP: %s, PORTA: %d conectou)\n",
  //         inet_ntop(
  //           AF_INET,
  //           &(remote_info_.sin_addr).s_addr,enderecoRemoto,
  //           sizeof(enderecoRemoto)
  //          ),
  //         ntohs(remote_info_.sin_port));
  //  if ( (childpid = fork()) == 0) { /* Se for zero está no processo filho */
  //     close(sockfd());  /* Fecha o socket que está escutando (só precisa de 1) */
  //     
  //     /* Processa tudo que for enviado do cliente conectado */
  //     while ((n=read(connfd, recvline, MAXLINE)) > 0) {
  //        /* Lê a linha enviada pelo cliente e escreve na saída padrão */
  //        recvline[n]=0;
  //        Command cmd = Command::from_packet(recvline);
  //        ServerHandler().handle(cmd);
  //        if ((fputs(recvline,stdout)) == EOF) {
  //           perror("fputs error");
  //           exit (1);
  //        }
  //        /* Agora re-envia a linha para o cliente */
  //        write(connfd, recvline, strlen(recvline));
  //     }
  //     /******************************************************/
  //     printf(
  //        "Dados do socket remoto: (IP: %s, PORTA: %d desconectou)\n",
  //        inet_ntop(AF_INET, &(remote_info_.sin_addr).s_addr, enderecoRemoto,
  //                  sizeof(enderecoRemoto)),
  //        ntohs(remote_info_.sin_port)
  //      );
  //    close(connfd);
  //     exit (0);         /* Termina o processo filho */
  //  }
  //  /* Se for o pai continua a execução aqui... */
  //  close(connfd);
	//}
}

bool TCPConnection::connect (const string& hostname, unsigned short port) {
  return false;
}

std::string TCPConnection::receive () {
  char cmdline[MAXLINE+1];
  int n=read(sockfd(), cmdline, MAXLINE);
	if (n < 0) {
		perror("read error");
		exit(1);
	}
  cmdline[n]=0;
  return cmdline;
}

void TCPConnection::send (const std::string& data) {
  write(sockfd(), data.c_str(), data.size()); 
}


} // namespace ep2



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

#include <string>

#include "command.h"

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
  Connection(socket(AF_INET, SOCK_STREAM, 0)) {}

void TCPConnection::host (unsigned short port) {
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port);
	if (bind(sockfd(), (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		perror("bind");
		exit(1);
	}
	if (listen(sockfd(), LISTENQ) == -1) {
		perror("listen");
		exit(1);
	}
}

void TCPConnection::accept () {
	int    connfd;
	struct sockaddr_in dadosRemoto;
	int    dadosRemotoLen;
	char   enderecoRemoto[MAXDATASIZE + 1];
	ssize_t  n;
	pid_t  childpid;
	char	recvline[MAXLINE + 1];
	dadosRemotoLen = sizeof(dadosRemoto);
	bzero(&dadosRemoto, dadosRemotoLen);
	for ( ; ; ) {
    // Com isso não precisa usar o getpeername. Antes no lugar dos dados do
    // dadosRemoto, tinha NULL
		if ((connfd = ::accept(sockfd(), (struct sockaddr*) &dadosRemoto,
                         (socklen_t *) &dadosRemotoLen)) == -1 ) {
			perror("accept");
			exit(1);
  	}
    /* Imprimindo os dados do socket remoto */
    printf("Dados do socket remoto: (IP: %s, PORTA: %d conectou)\n",
           inet_ntop(
             AF_INET,
             &(dadosRemoto.sin_addr).s_addr,enderecoRemoto,
             sizeof(enderecoRemoto)
            ),
           ntohs(dadosRemoto.sin_port));
    if ( (childpid = fork()) == 0) { /* Se for zero está no processo filho */
       close(sockfd());  /* Fecha o socket que está escutando (só precisa de 1) */
       
       /* Processa tudo que for enviado do cliente conectado */
       while ((n=read(connfd, recvline, MAXLINE)) > 0) {
          /* Lê a linha enviada pelo cliente e escreve na saída padrão */
          recvline[n]=0;
          Command cmd = Command::from_packet(recvline);
          puts(string(cmd).c_str());
          if ((fputs(recvline,stdout)) == EOF) {
             perror("fputs error");
             exit (1);
          }
          /* Agora re-envia a linha para o cliente */
          write(connfd, recvline, strlen(recvline));
       }
       /******************************************************/
       printf(
          "Dados do socket remoto: (IP: %s, PORTA: %d desconectou)\n",
          inet_ntop(AF_INET, &(dadosRemoto.sin_addr).s_addr, enderecoRemoto,
                    sizeof(enderecoRemoto)),
          ntohs(dadosRemoto.sin_port)
        );
       exit (0);         /* Termina o processo filho */
    }
    /* Se for o pai continua a execução aqui... */
    close(connfd);
	}
}

} // namespace ep2


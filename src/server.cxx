/* Código simples de um servidor de echo.
 * Não é o código ideal mas é suficiente para exemplificar os
 * conceitos da disciplina de redes de computadores.
 *
 * Prof. Daniel Batista em 28/08/2011. Baseado em código do livro do
 * Stevens.
 *
 * Bugs? Tente consertar primeiro! Depois me envie email :) batista@ime.usp.br
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

#define LISTENQ 1
#define MAXDATASIZE 100
#define MAXLINE 4096

int main (int argc, char **argv) {
	int    listenfd, connfd;
	struct sockaddr_in servaddr;
	struct sockaddr_in dadosRemoto;
	int    dadosRemotoLen;
	char   enderecoRemoto[MAXDATASIZE + 1];
	ssize_t  n;
	pid_t  childpid;
	char	recvline[MAXLINE + 1];

	if (argc != 2) {
      fprintf(stderr,"Uso: %s <Porta>\n",argv[0]);
		exit(1);
	}

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	dadosRemotoLen=sizeof(dadosRemoto);
	bzero(&dadosRemoto, dadosRemotoLen);
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
		perror("bind");
		exit(1);
	}
	
	if (listen(listenfd, LISTENQ) == -1) {
		perror("listen");
		exit(1);
	}

	for ( ; ; ) {
    // Com isso não precisa usar o getpeername. Antes no lugar dos dados do
    // dadosRemoto, tinha NULL
		if ((connfd = accept(listenfd, (struct sockaddr *) &dadosRemoto, (socklen_t *) &dadosRemotoLen)) == -1 ) {
			perror("accept");
			exit(1);
  	}
    /* Imprimindo os dados do socket remoto */
    printf("Dados do socket remoto: (IP: %s, PORTA: %d conectou)\n",inet_ntop(AF_INET, &(dadosRemoto.sin_addr).s_addr,enderecoRemoto,sizeof(enderecoRemoto)), ntohs(dadosRemoto.sin_port));

    if ( (childpid = fork()) == 0) { /* Se for zero está no processo filho */
       close(listenfd);  /* Fecha o socket que está escutando (só precisa de 1) */
       
       /* Processa tudo que for enviado do cliente conectado */
       while ((n=read(connfd, recvline, MAXLINE)) > 0) {
          /* Lê a linha enviada pelo cliente e escreve na saída padrão */
          recvline[n]=0;
          if ((fputs(recvline,stdout)) == EOF) {
             perror("fputs error");
             exit (1);
          }
          /* Agora re-envia a linha para o cliente */
          write(connfd, recvline, strlen(recvline));
       }
       /******************************************************/
       printf("Dados do socket remoto: (IP: %s, PORTA: %d desconectou)\n",inet_ntop(AF_INET, &(dadosRemoto.sin_addr).s_addr,enderecoRemoto,sizeof(enderecoRemoto)), ntohs(dadosRemoto.sin_port));
       exit (0);         /* Termina o processo filho */
    }
        
    /* Se for o pai continua a execução aqui... */
    close(connfd);
	}
	return(0);
}

/* Código simples de um cliente de echo.
 * Não é o código ideal mas é suficiente para exemplificar os
 * conceitos da disciplina de redes de computadores.
 *
 * Prof. Daniel Batista em 28/08/2011. Baseado em código do livro do
 * Stevens.
 *
 * Bugs? Tente consertar primeiro! Depois me envie email :) batista@ime.usp.br
 */

//#include <sys/socket.h>
//#include <sys/types.h>
//#include <arpa/inet.h>
//#include <netinet/in.h>
//#include <stdio.h>
//#include <netdb.h>
//#include <string.h>
//#include <errno.h>
//#include <string.h>
//#include <stdlib.h>
//#include <unistd.h>

/* Para resolver os nomes com a função gethostbyname */
//#include <netdb.h>

#include <cstdlib>
#include <cstdio>

#include "prompt.h"
#include "TCPconnection.h"

#define MAXLINE 4096

using ep2::Prompt;
using ep2::TCPConnection;

int main(int argc, char **argv) {

	if (argc != 3) {
      fprintf(stderr,"Uso: %s <Endereco IP|Nome> <Porta>\n",argv[0]);
		exit(1);
	}

  TCPConnection client;
  client.connect(argv[1], atoi(argv[2]));
  Prompt prompt(client.sockfd());
  prompt.init();
  prompt.run();
   
  return 0;
}


/* Código simples de um cliente de echo.
 * Não é o código ideal mas é suficiente para exemplificar os
 * conceitos da disciplina de redes de computadores.
 *
 * Prof. Daniel Batista em 28/08/2011. Baseado em código do livro do
 * Stevens.
 *
 * Bugs? Tente consertar primeiro! Depois me envie email :) batista@ime.usp.br
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* Para resolver os nomes com a função gethostbyname */
#include <netdb.h>

#define MAXLINE 4096

int main(int argc, char **argv) {
	int	sockfd, n;
	char	recvline[MAXLINE + 1];
	struct  sockaddr_in servaddr;
   struct  sockaddr_in dadosLocal;
   int     dadosLocalLen;
   char    enderecoLocal[MAXLINE + 1];
   
   /* Para uso com o gethostbyname */
   struct  hostent *hptr;
   char    enderecoIPServidor[INET_ADDRSTRLEN];

	if (argc != 3) {
      fprintf(stderr,"Uso: %s <Endereco IP|Nome> <Porta>\n",argv[0]);
		exit(1);
	}

   /* Resolvendo o nome passado na linha de comando.
    * Sobre o funcionamento: o gethostbyname faz uma busca pelo nome
    * acessando os registros locais do SO (/etc/hosts por exemplo) e
    * depois o servidor DNS. No caso do DNS ele faz uma busca por
    * registros do tipo "A". Ao encontrar ele retorna as informações
    * em uma struct hostent que tem este formato:
    *
    * struct hostent {
    *     char  *h_name;        -- official (canonical) name of host 
    *     char **h_aliases;     -- pointer to array of pointers to alias names
    *     int    h_addrtype;    -- host address type: AF_INET 
    *     int    h_length;      -- length of address: 4 
    *     char **h_addr_list;   -- ptr to array of ptrs with IPv4 addrs 
    * };
    *
    * Em caso de erro (retorno NULL da função), o inteiro h_errno é modificado com os valores:
    *
    * HOST_NOT_FOUND
    * TRY_AGAIN
    * NO_RECOVERY
    * NO_DATA
    * 
    * A função hstrerror pode ser usada para interpretar o conteúdo do
    * h_errno.
    */
   if ( (hptr = gethostbyname(argv[1])) == NULL) {
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
   if ( (inet_ntop(AF_INET, hptr->h_addr_list[0], enderecoIPServidor, sizeof(enderecoIPServidor))) == NULL) {
      fprintf(stderr,"inet_ntop :(\n");
      exit (1);
   }

   /* A partir deste ponto o endereço IP do servidor estará na string
    * enderecoIPServidor */
   printf("[Conectando no servidor no IP %s]\n",enderecoIPServidor);
   printf("[o comando 'exit' encerra a conexão]\n");

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket error");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
   dadosLocalLen=sizeof(dadosLocal);
   bzero(&dadosLocal, dadosLocalLen);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(atoi(argv[2]));

   /* O endereço IP passado para a função é o que foi retornado na gethostbyname */
	if (inet_pton(AF_INET, enderecoIPServidor, &servaddr.sin_addr) <= 0) {
		perror("inet_pton error");
		exit(1);
	}

	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		perror("connect error");
		exit(1);
	}
   
   /* Imprimindo os dados do socket local */
   if (getsockname(sockfd, (struct sockaddr *) &dadosLocal, (socklen_t *) &dadosLocalLen)) {
			perror("getsockname error");
			exit(1);
   }
   printf("Dados do socket local: (IP: %s, PORTA: %d)\n",inet_ntop(AF_INET, &(dadosLocal.sin_addr).s_addr,enderecoLocal,sizeof(enderecoLocal)), ntohs(dadosLocal.sin_port));
   /***************************************/
   
   n=1;
	while (n != 0)  {
      if ((fgets(recvline,MAXLINE,stdin)) != NULL) {
         if (!strcmp(recvline,"exit\n"))
            n=0;
         else {
            /* Escreve a linha lida no socket */
            write(sockfd, recvline, strlen(recvline));
            /* Lê a linha reenviada pelo servidor e escreve na saída padrão */
            n=read(sockfd,recvline,MAXLINE);
            recvline[n]=0;
            if ((fputs(recvline,stdout)) == EOF) {
               perror("fputs error");
               exit (1);
            }
         }
      }
      else
         n=0;
	}

	if (n < 0) {
		perror("read error");
		exit(1);
	}
   
	exit(0);
}

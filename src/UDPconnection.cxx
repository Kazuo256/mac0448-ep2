
#include "UDPconnection.h"

#include <cstdio>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "command.h"

#define MAXLINE 4096

namespace ep2 {

using std::string;

UDPConnection::UDPConnection () :
  Connection(socket(AF_INET, SOCK_DGRAM, 0)) {
  bzero(&local_info_, sizeof(local_info_));
  bzero(&remote_info_, sizeof(remote_info_)); 
}

UDPConnection::~UDPConnection () {
  if (remote_addr_.size()) {
    printf(
       "Dados do socket remoto: (IP: %s, PORTA: %d desconectou)\n",
       remote_addr_.c_str(),
       ntohs(remote_info_.sin_port)
     );
  }
}

void UDPConnection::host (unsigned short port) {
	local_info_.sin_family      = AF_INET;
	local_info_.sin_addr.s_addr = htonl(INADDR_ANY);
	local_info_.sin_port        = htons(port);
	if (bind(sockfd(),
           (struct sockaddr*)&local_info_, sizeof(local_info_)) == -1) {
		perror("bind");
		exit(1);
	}
}

Connection* UDPConnection::accept () {
  struct sockaddr_in  remote_info;
	int                 remote_info_size = sizeof(remote_info);
  int                 n;
	char                enderecoRemoto[INET_ADDRSTRLEN];
  char                recvline[MAXLINE+1];

  puts("[Aceitando conexão UDP]");
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
  //accepted->local_info_  = local_info_;
  accepted->remote_info_ = remote_info;
  accepted->remote_addr_ = enderecoRemoto;
  if (::connect(accepted->sockfd(), (struct sockaddr*)&accepted->remote_info_,
                sizeof(accepted->remote_info_)) < 0) {
		perror("connect error");
    delete accepted;
		exit(1);
  }
  socklen_t dadosLocalLen = sizeof(accepted->local_info_);
  if (getsockname(accepted->sockfd(), (struct sockaddr*)&accepted->local_info_,
                  (socklen_t*)&dadosLocalLen)) {
  	perror("getsockname error");
  	exit(1);
  }
  printf(
    "[Dados do socket local: (IP: %s, PORTA: %d)]\n",
    accepted->local_address().c_str(),
    accepted->local_port()
  );
  if (::sendto(sockfd(), (const void*)&accepted->local_info_.sin_port,
               sizeof(accepted->local_info_.sin_port), 0,
               (struct sockaddr*)&remote_info, remote_info_size) < 0) {
    perror("connection confirmation error");
    delete accepted;
    exit(1);
  }
  puts("[UDP conectou]");
  return accepted;
}

bool UDPConnection::connect (const string& hostname, unsigned short port) {
  /* Para uso com o gethostbyname */
  struct    hostent *hptr;
  socklen_t dadosLocalLen;
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

  dadosLocalLen=sizeof(local_info_);
	remote_info_.sin_family = AF_INET;
	remote_info_.sin_port   = htons(port);

  // O endereço IP passado para a função é o que foi retornado na gethostbyname
	if (inet_pton(AF_INET, enderecoIPServidor, &remote_info_.sin_addr) <= 0) {
		perror("inet_pton error");
		exit(1);
	}

	if (::connect(sockfd(), (struct sockaddr*)&remote_info_,
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
    "[Dados do socket local: (IP: %s, PORTA: %d)]\n",
    local_address().c_str(),
    local_port()
  );

  if (::send(sockfd(), "connect_me", sizeof "connect_me", 0) < 0) {
    perror("connect request error");
    exit(1);
  }
  printf("[Enviou pedido de conexão]\n");

  char      recvline[MAXLINE+1];
  ssize_t   n;
  n = recv(sockfd(), recvline, MAXLINE, 0);
  recvline[n] = '\0';

  printf(
    "[UDP connection response: %hu]\n",
    ntohs(*(unsigned short*)recvline)
  );

  remote_info_.sin_port = *(unsigned short*)recvline;

	if (::connect(sockfd(), (struct sockaddr*)&remote_info_,
              sizeof(remote_info_)) < 0) {
		perror("connect 2 error");
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
  return Command::from_packet(cmdline, n);
}

void UDPConnection::send (const Command& cmd) {
  string packet = cmd.make_packet();
  size_t size = packet.size();
  if (::send(sockfd(), packet.c_str(), size, 0) < 0) {
    perror("write error");
    exit(1);
  }
}

unsigned short UDPConnection::local_port () const {
  return ntohs(local_info_.sin_port);
}

string UDPConnection::local_address () const {
  char addr[INET_ADDRSTRLEN];
  return
    inet_ntop(AF_INET, &local_info_.sin_addr.s_addr, addr, INET_ADDRSTRLEN);
}

unsigned short UDPConnection::remote_port () const {
  return ntohs(remote_info_.sin_port);
}

string UDPConnection::remote_address () const {
  char addr[INET_ADDRSTRLEN];
  return
    inet_ntop(AF_INET, &remote_info_.sin_addr.s_addr, addr, INET_ADDRSTRLEN);
}

} // namespace ep2



#include "UDPconnection.h"

#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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
	int                 remote_info_size;
  int                 n;
	char                enderecoRemoto[MAXDATASIZE + 1];
  char                recvline[MAXLINE+1];

  n = recvfrom(sockfd(), recvline, MAXLINE, 0, (struct sockaddr*)&remote_info,
               (socklen_t*)&remote_info_size);
  recvline[n] = '\0';
  printf("[UDP connection request: %s]\n", recvline);
  /* Imprimindo os dados do socket remoto */
  printf("[Dados do socket remoto: (IP: %s, PORTA: %d conectou)]\n",
         inet_ntop(
           AF_INET,
           &(remote_info.sin_addr).s_addr,
           enderecoRemoto,
           sizeof(enderecoRemoto)
          ),
         ntohs(remote_info.sin_port));
  UDPConnection *accepted = new UDPConnection();
  accepted->local_info_  = local_info_;
  accepted->remote_info_ = remote_info;
  accepted->remote_addr_ = enderecoRemoto;
  if (::connect(accepted->sockfd(), (struct sockaddr*)&accepted->local_info_,
                sizeof(accepted->local_info_)) < 0) {
		perror("connect error");
    delete accepted;
		exit(1);
  }
  if (::send(accepted->sockfd(), "accepted", sizeof "accepted", 0) < 0) {
    perror("connection confirmation error");
    delete accepted;
    exit(1);
  }
  return accepted;
}

bool UDPConnection::connect (const string& hostname, unsigned short port) {
  return false;
}

} // namespace ep2


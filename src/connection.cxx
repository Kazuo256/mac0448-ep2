
#include "connection.h"

#include <cstdlib>
#include <cstdio>
#include <strings.h>
#include <arpa/inet.h>

#include <iostream>

namespace ep2 {

using std::string;
using std::cout;
using std::cerr;

Connection::Connection (int sockfd) : sockfd_(sockfd) {
  if (sockfd_ < 0) {
    cerr << "[Connection::Connection - Socket inválido]\n";
    exit(1);
  }
  // acho que o C++ já faz isso, mas é bom garantir
  bzero(&local_info_, sizeof(local_info_));
  bzero(&remote_info_, sizeof(remote_info_));
}

Connection::~Connection () {
#ifdef EP2_DEBUG
  cout << "[Fechando socket " << sockfd_ << "]\n";
  if (remote_info_.sin_addr.s_addr != htonl(INADDR_ANY))
    cout  << "[Desconectou de " << remote_address() << ":" << remote_port()
          << "]\n";
#endif
  close(sockfd_);
}

// Getters

unsigned short Connection::local_port () const {
  return ntohs(local_info_.sin_port);
}

string Connection::local_address () const {
  char addr[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &local_info_.sin_addr.s_addr,
                addr, INET_ADDRSTRLEN) == NULL) {
    cerr << "Connection::local_address - inet_ntop error\n";
    exit (1);
  }
  return addr;
}

unsigned short Connection::remote_port () const {
  return ntohs(remote_info_.sin_port);
}

string Connection::remote_address () const {
  char addr[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &remote_info_.sin_addr.s_addr,
                addr, INET_ADDRSTRLEN) == NULL) {
    cerr << "Connection::remote_address - inet_ntop error\n";
    exit (1);
  }
  return addr;
}

// Setters

void Connection::set_local_info (short family, unsigned long address,
                                 unsigned short port) {
	local_info_.sin_family      = family;
	local_info_.sin_addr.s_addr = htonl(address);
	local_info_.sin_port        = htons(port);
}

void Connection::set_local_info () {
  int info_len = sizeof(local_info_);
  if (getsockname(sockfd_, (struct sockaddr*)&local_info_,
                  (socklen_t*)&info_len)) {
  	perror("Connection::set_local_info() - getsockname error");
  	exit(1);
  }
}

void Connection::set_remote_info (short family, const string& address,
                                  unsigned short port) {
	remote_info_.sin_family      = family;
	remote_info_.sin_port        = htons(port);
  // O endereço IP passado para a função tem que ser válido (fornecido por
  // gethostbyname(), por exemplo)
	if (inet_pton(AF_INET, address.c_str(), &remote_info_.sin_addr) <= 0) {
		perror("Connection::set_remote_info(family,addr,port) - inet_pton error");
		exit(1);
	}
}

// Outros

void Connection::bind () {
	if (::bind(sockfd(),
             (struct sockaddr*)&local_info_, sizeof(local_info_)) == -1) {
		perror("Connection::bind - bind failed");
		exit(1);
	}
}

} // namespace ep2


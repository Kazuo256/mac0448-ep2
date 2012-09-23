
#include "connection.h"

#include <cstdlib>
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

unsigned short Connection::local_port () const {
  return ntohs(local_info_.sin_port);
}

string Connection::local_address () const {
  char addr[INET_ADDRSTRLEN];
  return
    inet_ntop(AF_INET, &local_info_.sin_addr.s_addr, addr, INET_ADDRSTRLEN);
}

unsigned short Connection::remote_port () const {
  return ntohs(remote_info_.sin_port);
}

string Connection::remote_address () const {
  char addr[INET_ADDRSTRLEN];
  return
    inet_ntop(AF_INET, &remote_info_.sin_addr.s_addr, addr, INET_ADDRSTRLEN);
}

} // namespace ep2


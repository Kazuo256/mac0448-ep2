
#include "UDPconnection.h"

#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>

#include "command.h"

// Usado por UDPConnection::receive()
#define MAXLINE 4096

namespace ep2 {

using std::string;
using std::cout;

// Esse construtor cria um novo socket UDP.
UDPConnection::UDPConnection () :
  Connection(socket(AF_INET, SOCK_DGRAM, 0)) {}

void UDPConnection::host (unsigned short port) {
  // Inicializa informações locais da conexão e faz bind().
  set_local_info(AF_INET, INADDR_ANY, port);
  bind();
}

Connection* UDPConnection::accept () {
  // Recebe um requisito de conexão e guarda as informações do remetente.
  struct sockaddr_in  remote_info;
	int                 remote_info_size = sizeof(remote_info);
  int                 n;
  char                recvline[MAXLINE+1];
  n = recvfrom(sockfd(), recvline, MAXLINE, 0, (struct sockaddr*)&remote_info,
               (socklen_t*)&remote_info_size);
  recvline[n] = '\0';
  // Cria a nova conexão para aceitar a requisição e configura ela.
  UDPConnection *accepted = new UDPConnection();
  accepted->set_remote_info(remote_info);
  // Conecta com o remetente da requisição.
  if (::connect(accepted->sockfd(), accepted->remote_info(), info_size()) < 0) {
		perror("UDP::accept - connect error");
    delete accepted;
		exit(1);
  }
  // Atualiza as informações locais da nova conexão.
  // (a chamada a ::connect garante endereço e porta locais para o socket)
  accepted->set_local_info();
  // Envia para o remetente da requisição a porta na qual ele deve conectar.
  unsigned short port = htons(accepted->local_port());
  if (::sendto(sockfd(), (const void*)&port, sizeof(port), 0,
               accepted->remote_info(), info_size()) < 0) {
    perror("UDP::accept - connection confirmation error");
    delete accepted;
    exit(1);
  }
#ifdef EP2_DEBUG
  // Imprime as informações remotas da nova conexão.
  cout  << "[Nova conexão UDP com "
        << accepted->remote_address() << ":"
        << accepted->remote_port() << "]\n";
#endif
  // Devolve a nova conexão, pronta para ser usada. 
  return accepted;
}

bool UDPConnection::connect (const string& hostname, unsigned short port) {
  // Configuramos as informações remotas da conexão usando get_host() e a porta
  // fornecida.
  set_remote_info(AF_INET, get_host(hostname), port);
#ifdef EP2_DEBUG
  cout  << "[Conectando com "
        << remote_address() << ":" << remote_port() << "]\n";
#endif
  // Tenta conectar com o host.
	if (::connect(sockfd(), remote_info(), info_size()) < 0) {
		perror("UDP::connect() - connect error");
		exit(1);
	}
  // Atualiza as informações locais da conexão
  // (a chamada a ::connect garante endereço e porta locais para o socket)
  set_local_info();
  // Como é UDP, só conectar com o host não basta.
  // O host deve fornecer uma outra porta para essa conexão.
  // Para isso, requisito essa porta fazendo uma espécie de handshake.
  if (::send(sockfd(), "connect_me", sizeof "connect_me", 0) < 0) {
    perror("UDP::connect() - connection request error");
    exit(1);
  }
  // Obtém a porta enviada pelo host.
  char      recvline[MAXLINE+1];
  ssize_t   n;
  n = recv(sockfd(), recvline, MAXLINE, 0);
  recvline[n] = '\0';
  unsigned short new_port = ntohs(*(unsigned short*)recvline);
  // Atualizo as informações remotas para usar a porta correta.
  set_remote_info(AF_INET, remote_address(), new_port);
  // Conecto para poder mandar e receber pacotes mais facilmente.
	if (::connect(sockfd(), remote_info(), info_size()) < 0) {
		perror("UDP::connect() - connect 2 error");
		exit(1);
	}
#ifdef EP2_DEBUG
  // Imprime resultados
  cout  << "[Conexão estabelicida de "
        << local_address() << ":" << local_port() << " para "
        << remote_address() << ":" << remote_port() << "]\n";
#endif
  return true;
}

Command UDPConnection::receive() {
  // Usa recv() para ler pacotes vindos da rede através da conexão.
  char cmdline[MAXLINE+1];
  int n=recv(sockfd(), cmdline, MAXLINE, 0);
	if (n < 0) {
		perror("UDP::receive() - read error");
		exit(1);
	}
  cmdline[n]=0;
  // Transforma em Command e devolve.
  return Command::from_packet(string(cmdline, n));
}

void UDPConnection::send (const Command& cmd) {
  // Contrói pacote a partir do objeto Command.
  string packet = cmd.make_packet();
  // Usa send() para enviar o pacote para a rede através da conexão.
  size_t size = packet.size();
  if (::send(sockfd(), packet.c_str(), size, 0) < 0) {
    perror("UDP::send - write error");
    exit(1);
  }
}

} // namespace ep2


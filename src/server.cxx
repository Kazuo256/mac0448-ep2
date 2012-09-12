
#include "connection.h"
#include "TCPconnection.h"

using ep2::Connection;
using ep2::TCPConnection;

int main (int argc, char **argv) {
	if (argc != 2) {
      fprintf(stderr,"Uso: %s <Porta>\n",argv[0]);
		exit(1);
	}
  Connection *server = new TCPConnection();
  server->host(atoi(argv[1]));
  server->accept();
  delete server;
	return 0;
}



#ifndef EP2_TCPCONNECTION_H_
#define EP2_TCPCONNECTION_H_

#include "connection.h"

namespace ep2 {

/// Implementação da classe abstrata Connection usando protocolo TCP.
class TCPConnection : public Connection {

  public:

    // Construtor.
    TCPConnection ();
    
    // Métodos virtuais sobreescritos da classe Connection:

    void host (unsigned short port);
    Connection* accept ();

    bool connect (const std::string& hostname, unsigned short port);

    Command receive ();
    void send (const Command& cmd);

  private:

    TCPConnection (int sockfd);

};

} // namespace ep2

#endif // EP2_TCPCONNECTION_H_


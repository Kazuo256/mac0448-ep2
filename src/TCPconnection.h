
#ifndef EP2_TCPCONNECTION_H_
#define EP2_TCPCONNECTION_H_

#include "connection.h"

namespace ep2 {

/// Implementação da interface Connection usando protocolo TCP.
class TCPConnection : public Connection {

  public:

    TCPConnection ();
    
    // Overwritten methods
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


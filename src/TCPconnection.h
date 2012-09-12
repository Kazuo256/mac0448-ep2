
#ifndef EP2_TCPCONNECTION_H_
#define EP2_TCPCONNECTION_H_

#include "connection.h"

namespace ep2 {

class TCPConnection : public Connection {

  public:

    TCPConnection ();
    
    void host (unsigned short port);
    void accept ();

    bool connect (const std::string& hostname, unsigned short port) {
      return false;
    }
    void send (const std::string& data) {}

};

} // namespace ep2

#endif // EP2_TCPCONNECTION_H_


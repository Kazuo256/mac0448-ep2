
#ifndef EP2_UDPCONNECTION_H_
#define EP2_UDPCONNECTION_H_

#include "connection.h"

namespace ep2 {

class UDPConnection : public Connection {

  public:

    UDPConnection ();
    
    // Overwritten methods
    void host (unsigned short port);
    Connection* accept ();

    bool connect (const std::string& hostname, unsigned short port);

    Command receive ();
    void send (const Command& cmd);

};

} // namespace ep2

#endif // EP2_UDPCONNECTION_H_


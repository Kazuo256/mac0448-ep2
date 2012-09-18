
#ifndef EP2_UDPCONNECTION_H_
#define EP2_UDPCONNECTION_H_

#include "connection.h"

#include <netinet/in.h>

namespace ep2 {

class UDPConnection : public Connection {

  public:

    UDPConnection ();
    ~UDPConnection ();
    
    // Overwritten methods
    void host (unsigned short port);
    Connection* accept ();

    bool connect (const std::string& hostname, unsigned short port);

    Command receive ();
    void send (const Command& cmd);

    unsigned short local_port () const;
    std::string local_address () const;
    unsigned short remote_port () const;
    std::string remote_address () const;

  private:

    struct sockaddr_in  local_info_,
                        remote_info_;
    std::string         remote_addr_;

};

} // namespace ep2

#endif // EP2_UDPCONNECTION_H_


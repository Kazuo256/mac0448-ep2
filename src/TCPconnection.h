
#ifndef EP2_TCPCONNECTION_H_
#define EP2_TCPCONNECTION_H_

#include "connection.h"

#include <netinet/in.h>

namespace ep2 {

class TCPConnection : public Connection {

  public:

    TCPConnection ();
    TCPConnection (int sockfd);
    ~TCPConnection ();
    
    // Overwritten methods
    void host (unsigned short port);
    Connection* accept ();

    bool connect (const std::string& hostname, unsigned short port);

    Command receive ();
    void send (const Command& cmd);

    // TCP-only methods
    unsigned short local_port () const { return local_info_.sin_port; }
    std::string local_address () const;

  private:

    struct sockaddr_in  local_info_,
                        remote_info_;
    std::string         remote_addr_;

};

} // namespace ep2

#endif // EP2_TCPCONNECTION_H_



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
    
    void host (unsigned short port);
    Connection* accept ();

    bool connect (const std::string& hostname, unsigned short port);

    std::string receive ();
    void send (const std::string& data);

  private:

    struct sockaddr_in  local_info_,
                        remote_info_;
    char                zero[32];
    std::string         remote_addr_;

};

} // namespace ep2

#endif // EP2_TCPCONNECTION_H_


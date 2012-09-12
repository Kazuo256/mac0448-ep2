
#ifndef EP2_CONNECTION_H_
#define EP2_CONNECTION_H_

#include <cstdlib>
#include <cstdio>
#include <string>

namespace ep2 {

class Connection {

  public:

    virtual ~Connection () { close(sockfd_); }

    // Server side methods
    virtual void host (unsigned short port) = 0; 
    virtual void accept () = 0;

    // Client side methods
    virtual bool connect (const std::string& hostname, unsigned short port) = 0;
    virtual void send (const std::string& data) = 0;

  protected:

    Connection (int sockfd) : sockfd_(sockfd) {
      if (sockfd_ < 0) {
        puts("SOCKET FAIL");
        exit(1);
      }
    }

    int sockfd () const { return sockfd_; }

  private:

    int sockfd_;

};

} // namespace ep2

#endif // EP2_CONNECTION_H_


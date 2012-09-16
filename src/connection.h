
#ifndef EP2_CONNECTION_H_
#define EP2_CONNECTION_H_

#include <cstdlib>
#include <cstdio>
#include <string>

namespace ep2 {

class Command;

class Connection {

  public:

    virtual ~Connection () {
      printf("closing socket %d\n", sockfd_);
      close(sockfd_);
    }

    int sockfd () const { return sockfd_; }

    // Server side methods
    virtual void host (unsigned short port) = 0; 
    virtual Connection* accept () = 0;

    // Client side methods
    virtual bool connect (const std::string& hostname, unsigned short port) = 0;

    // Either side methods
    virtual Command receive () = 0;
    virtual void send (const Command& cmd) = 0;
    virtual unsigned short local_port () const = 0;
    virtual std::string local_address () const = 0;
    virtual unsigned short remote_port () const = 0;
    virtual std::string remote_address () const = 0;

  protected:

    Connection (int sockfd) : sockfd_(sockfd) {
      if (sockfd_ < 0) {
        puts("SOCKET FAIL");
        exit(1);
      }
    }

  private:

    int sockfd_;

};

} // namespace ep2

#endif // EP2_CONNECTION_H_


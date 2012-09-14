
#ifndef EP2_COMMANDHANDLER_H_
#define EP2_COMMANDHANDLER_H_

namespace ep2 {

class Command;
class Connection;

class CommandHandler {

  public:

    virtual void handle (Connection *connection, const Command& cmd) = 0;

  protected:

    CommandHandler () {}

};

} // namespace ep2

#endif


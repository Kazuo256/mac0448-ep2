
#ifndef EP2_SERVERHANDLER_H_
#define EP2_SERVERHANDLER_H_

#include "commandhandler.h"

namespace ep2 {

class ServerHandler : public CommandHandler {

  public:

    ServerHandler ();

    void handle (const Command& cmd);

};

} // namespace ep2

#endif


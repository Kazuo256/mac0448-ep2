
#ifndef EP2_SERVERHANDLER_H_
#define EP2_SERVERHANDLER_H_

#include "commandhandler.h"

namespace ep2 {

class ServerData;

class ServerHandler : public CommandHandler {

  public:

    ServerHandler (ServerData* serverdata);

    void handle (Connection *client, const Command& cmd);
  private:
    ServerData*  serverdata_;

};

} // namespace ep2

#endif


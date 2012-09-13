
#include "serverhandler.h"

#include <iostream>
#include <string>

#include "command.h"

namespace ep2 {

using std::cout;
using std::string;

ServerHandler::ServerHandler () :
  CommandHandler() {}

void ServerHandler::handle (const Command& cmd) {
  cout << static_cast<string>(cmd) << "\n";
}

} // namespace ep2


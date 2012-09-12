
#include "command.h"

namespace ep2 {

using std::string;
using std::make_pair;

Command Command::nick (const string& name) {
  return Command(NICK, arg_list(1, make_pair(name.size(), name)));
}

} // namespace ep2


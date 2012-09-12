
#ifndef EP2_COMMAND_H_
#define EP2_COMMAND_H_

#include <string>

namespace ep2 {

class Command {

  public:

    static Command disconnect ();
    static Command nick (const std::string& data); 

  private:

    typedef unsigned char byte;

    Command (byte opcode, byte len, const std::string& data);

};

} // namespace ep2

#endif


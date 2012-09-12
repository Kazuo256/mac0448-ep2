
#ifndef EP2_COMMAND_H_
#define EP2_COMMAND_H_

#include <string>
#include <vector>
#include <utility>

namespace ep2 {

class Command {

  public:

    // Client commands
    static Command nick (const std::string& name); 
    static Command msg (const std::string& name, const std::string& msg);
    static Command disconnect ();
    static Command send (const std::string& name, const std::string& path);
    static Command list ();
    static Command accept (const std::string& name, const std::string& file);
    static Command refuse (const std::string& name, const std::string& file);

  private:

    typedef unsigned char                 byte;
    typedef std::pair<byte, std::string>  arg_t;
    typedef std::vector<arg_t>            arg_list;

    byte      opcode_;
    arg_list  data_;

    const static byte NICK = 0x1,
                      DISCONNECT = 0x2,
                      MSG = 0x3;

    Command (byte opcode, const arg_list& data) :
      opcode_(opcode), data_(data) {}

};

} // namespace ep2

#endif


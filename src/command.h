
#ifndef EP2_COMMAND_H_
#define EP2_COMMAND_H_

#include <string>
#include <vector>
#include <utility>

namespace ep2 {

class Command {

  public:

    std::string make_packet () const;

    operator std::string () const;

    static Command from_packet (const std::string& packet);

    // Client commands
    static Command nick (const std::string& name); 
    static Command msg (const std::string& name, const std::string& msg);
    static Command disconnect ();
    static Command send (const std::string& name, const std::string& path);
    static Command list ();
    static Command accept (const std::string& name, const std::string& file);
    static Command refuse (const std::string& name, const std::string& file);

  private:

    typedef unsigned char             byte;
    typedef std::vector<std::string>  arg_list;

    byte      opcode_;
    arg_list  data_;

    const static byte NICK = 0x1,
                      DISCONNECT = 0x2,
                      MSG = 0x3;

    Command (byte opcode, const arg_list& data = arg_list()) :
      opcode_(opcode), data_(data) {}

};

} // namespace ep2

#endif


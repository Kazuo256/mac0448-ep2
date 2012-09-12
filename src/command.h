
#ifndef EP2_COMMAND_H_
#define EP2_COMMAND_H_

#include <string>
#include <vector>
#include <pair>

namespace ep2 {

class Command {

  public:

    // Client commands
    static Command disconnect ();
    static Command nick (const std::string& name); 
    static Command msg (const std::string& name, const std::string& msg);
    static Command send (const std::string& name, const std::string& path);
    static Command list ();
    static Command accept (const std::string& name, const std::string& file);
    static Command refuse (const std::string& name, const std::string& file);

  private:

    typedef unsigned char                 byte;
    typedef std::pair<byte, std::string>  arg_t;
    typedef std::vector<arg_t>            arg_list;

    Command (byte opcode, byte len, const arg_list& data);

};

} // namespace ep2

#endif


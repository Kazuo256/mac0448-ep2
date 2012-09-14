
#ifndef EP2_COMMAND_H_
#define EP2_COMMAND_H_

#include <string>
#include <vector>
#include <utility>

namespace ep2 {

class Command {

  public:

    typedef unsigned char             byte;

    const static byte REQUEST_ID = 0x1,
                      NICK = 0x2,
                      DISCONNECT = 0x3,
                      MSG = 0x4,
                      GIVE_ID = 0x11,
                      MAX_COMMAND = 0x12;

    byte opcode () const { return opcode_; }
    std::string arg (size_t idx) const { return data_[idx]; }

    std::string make_packet () const;

    operator std::string () const;

    static Command from_packet (const std::string& packet);

    // Client commands
    static Command request_id ();
    static Command nick (const std::string& name); 
    static Command msg (const std::string& name, const std::string& msg);
    static Command disconnect ();
    static Command send (const std::string& name, const std::string& path);
    static Command list ();
    static Command accept (const std::string& name, const std::string& file);
    static Command refuse (const std::string& name, const std::string& file);

    // Server Commands
    static Command give_id (const std::string& id);

  private:

    typedef std::vector<std::string>  arg_list;

    byte      opcode_;
    arg_list  data_;

    Command (byte opcode, const arg_list& data = arg_list()) :
      opcode_(opcode), data_(data) {}

};

} // namespace ep2

#endif


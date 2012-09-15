
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
                      REFUSE_NICK = 0x12,
                      ACCEPT_NICK = 0x13,
                      MAX_COMMAND = 0x14;

    byte opcode () const { return opcode_; }
    std::string arg (size_t idx) const { return data_[idx]; }

    std::string make_packet () const;

    operator std::string () const;

    static Command from_packet (const std::string& packet);

    static Command null_cmd () { return Command(0xff); }

    // Client commands
    static Command request_id ();
    static Command nick (const std::string& name, const std::string& id); 
    static Command msg (const std::string& name, const std::string& msg);
    static Command disconnect ();
    static Command send (const std::string& name, const std::string& path);
    static Command list ();
    static Command accept (const std::string& name, const std::string& file);
    static Command refuse (const std::string& name, const std::string& file);

    // Server Commands
    static Command give_id (const std::string& id);
    static Command refuse_nick ();
    static Command accpet_nick ();

  private:

    typedef std::vector<std::string>  arg_list;

    byte      opcode_;
    arg_list  data_;

    Command (byte opcode, const arg_list& data = arg_list()) :
      opcode_(opcode), data_(data) {}

    template <byte CODE>
    static Command generic_cmd (const std::string& arg1);

    template <byte CODE>
    static Command generic_cmd (const std::string& arg1,
                                const std::string& arg2);

};

template <Command::byte CODE>
inline Command Command::generic_cmd (const std::string& arg1) {
  return Command(CODE, arg_list(1, arg1));
}

template <Command::byte CODE>
inline Command Command::generic_cmd (const std::string& arg1,
                                     const std::string& arg2) {
  arg_list args;
  args.push_back(arg1);
  args.push_back(arg2);
  return Command(CODE, args);
}

} // namespace ep2

#endif


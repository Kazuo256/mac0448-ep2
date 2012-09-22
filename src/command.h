
#ifndef EP2_COMMAND_H_
#define EP2_COMMAND_H_

#include <string>
#include <vector>
#include <utility>

namespace ep2 {

/// Comandos usados pelo protocolo da camada de aplicação.
/** É possível construir objetos de comando usando os métodos estáticos (como
  * nick() ou msg()) ou a partir de pacotes (strings) recebidos pela rede.
  * Também é possível converter um comando em um pacote para mandar através de
  * uma conexão.
  */
class Command {

  public:

    // Tipo usado para identificar comandos e para compor os pacotes derivados
    // deles.
    typedef unsigned char             byte;

    // Tipo usado para guardar os argumentos de um comando.
    typedef std::vector<std::string>  ArgList;

    // Tipos de comandos.
    const static byte REQUEST_ID    = 0x01, // usados pelo cliente
                      NICK          = 0x02,
                      DISCONNECT    = 0x03,
                      MSG           = 0x04,
                      LIST_REQUEST  = 0x05,
                      SEND          = 0x06,
                      CHUNK         = 0x07,
                      ACCEPT        = 0x08,
                      REFUSE        = 0x09,
                      CONTINUE      = 0x0A,
                      GIVE_ID       = 0x11, // usados pelo servidor
                      REFUSE_NICK   = 0x12,
                      ACCEPT_NICK   = 0x13,
                      LIST_RESPONSE = 0x14,
                      MSG_FAIL      = 0x15,
                      MSG_OK        = 0x16,
                      SEND_FAIL     = 0x17,
                      SEND_OK       = 0x18,
                      MAX_COMMAND   = 0x20;

    byte opcode () const { return opcode_; }
    std::string arg (size_t idx) const { return data_[idx]; }
    size_t num_args () const { return data_.size(); }

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
    static Command accept (const std::string& sender);
    static Command refuse (const std::string& sender);
    static Command list_request ();
    static Command chunk (const std::string& data);
    static Command cont ();

    // Server Commands
    static Command give_id (const std::string& id);
    static Command refuse_nick ();
    static Command accept_nick ();
    static Command list_response (const ArgList& arg_list);
    static Command msg_ok ();
    static Command msg_fail ();
    static Command send_fail (const std::string& info);
    static Command send_ok (const std::string& addr, const std::string& port);

  private:

    byte      opcode_;
    ArgList  data_;

    Command (byte opcode, const ArgList& data = ArgList()) :
      opcode_(opcode), data_(data) {}

    template <byte CODE>
    static Command generic_cmd (const std::string& arg1);

    template <byte CODE>
    static Command generic_cmd (const std::string& arg1,
                                const std::string& arg2);

};

template <Command::byte CODE>
inline Command Command::generic_cmd (const std::string& arg1) {
  return Command(CODE, ArgList(1, arg1));
}

template <Command::byte CODE>
inline Command Command::generic_cmd (const std::string& arg1,
                                     const std::string& arg2) {
  ArgList args;
  args.push_back(arg1);
  args.push_back(arg2);
  return Command(CODE, args);
}

} // namespace ep2

#endif


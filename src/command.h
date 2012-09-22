
#ifndef EP2_COMMAND_H_
#define EP2_COMMAND_H_

#include <string>
#include <vector>
#include <utility>

namespace ep2 {

/// Representa os comandos usados pelo protocolo da camada de aplicação.
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
                      LIST_REQUEST  = 0x04,
                      CHUNK         = 0x05,
                      ACCEPT        = 0x06,
                      REFUSE        = 0x07,
                      CONTINUE      = 0x08,

                      GIVE_ID       = 0x09, // usados pelo servidor
                      REFUSE_NICK   = 0x0A,
                      ACCEPT_NICK   = 0x0B,
                      LIST_RESPONSE = 0x0C,
                      MSG_FAIL      = 0x0D,
                      MSG_OK        = 0x0E,
                      SEND_FAIL     = 0x0F,
                      SEND_OK       = 0x10,
                      
                      MSG           = 0x11, // usados por ambos
                      SEND          = 0x12,

                      MAX_COMMAND   = 0x13; // apenas para controle de erros

    // Devolve o código de operação do comando.
    byte opcode () const { return opcode_; }

    // Devolve o idx-ésimo argumento do comando. Não verifica a validade do
    // índice passado, o chamante deve usar num_arg() para verificar isso.
    std::string arg (size_t idx) const { return data_[idx]; }

    // Devolve o número de argumentos do comando.
    size_t num_args () const { return data_.size(); }

    // Contrói a partir do comando um pacote para ser enviado pela rede.
    std::string make_packet () const;

    // Constrói um comando a partir de um pacote recebido pela rede. Assume-se
    // que o pacote é bem formado.
    static Command from_packet (const std::string& packet);

#ifdef EP2_DEBUG
    // Converte o comando para uma string. Usado para depuração.
    operator std::string () const;
#endif

    /*************************************************************************/
    /*** Daqui em diante são os métodos estáticos que constroem os comandos ***/
    /*** do protocolo.                                                      ***/
    /*************************************************************************/

    // Comando nulo, usado para inicializar variaveis de objetos de comando.
    static Command null_cmd () { return Command(0xff); }

    // Comando usados pelo cliente.
    static Command request_id ();
    static Command nick (const std::string& name, const std::string& id);
    static Command disconnect ();
    static Command accept (const std::string& sender);
    static Command refuse (const std::string& sender);
    static Command list_request ();
    static Command chunk (const std::string& data);
    static Command cont ();

    // Comandos usados pelo servidor.
    static Command give_id (const std::string& id);
    static Command refuse_nick ();
    static Command accept_nick ();
    static Command list_response (const ArgList& users);
    static Command msg_ok ();
    static Command msg_fail ();
    static Command send_fail (const std::string& info);
    static Command send_ok (const std::string& addr, const std::string& port);

    // Comandos usados por ambos
    static Command msg (const std::string& name, const std::string& msg);
    static Command send (const std::string& name, const std::string& path);

  private:

    // Código de operação do comando.
    byte    opcode_;
    // Argumentos do comando.
    ArgList data_;

    // Construtor privado para que não sejam contruídos comandos inesperados.
    Command (byte opcode, const ArgList& data = ArgList()) :
      opcode_(opcode), data_(data) {}

    // Usada internamente para facilitar a construção de comandos com apenas um
    // argumento.
    template <byte CODE>
    static Command generic_cmd (const std::string& arg1);

    // Usada internamente para facilitar a construção de comandos com dois
    // argumentos.
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


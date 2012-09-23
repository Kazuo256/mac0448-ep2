
#ifndef EP2_COMMANDHANDLER_H_
#define EP2_COMMANDHANDLER_H_

namespace ep2 {

class Command;
class Connection;

/// Interface para gerenciar trocas de comandos pela rede.
/** A ideia era que tanto o servidor quanto o cliente implementassem essa
  * interface, mas por falta de tempo apenas o servidor o fez.
  */
class CommandHandler {

  public:

    // Recebe um comando e a conexão da qual ele veio, e deve tratá-lo
    // adequadamente.
    virtual void handle (Connection *connection, const Command& cmd) = 0;

  protected:

    // Construtor protegido para forçar herança.
    CommandHandler () {}

};

} // namespace ep2

#endif


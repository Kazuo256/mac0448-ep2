
#ifndef EP2_SERVERHANDLER_H_
#define EP2_SERVERHANDLER_H_

#include "commandhandler.h"

namespace ep2 {

class ServerData;

/// Implementa a interface CommandHandler para tratar comandos enviados ao
/// servidor.
class ServerHandler : public CommandHandler {

  public:

    // Construtor receve ponteiro para os dados do servidor.
    ServerHandler (ServerData* serverdata);

    // Método sobreescrito da classe CommandHandler.
    void handle (Connection *client, const Command& cmd);

  private:

    // Dados do servidor.
    ServerData*  serverdata_;

};

} // namespace ep2

#endif



#ifndef EP2_CONNECTION_H_
#define EP2_CONNECTION_H_

#include <netinet/in.h>

#include <string>

namespace ep2 {

class Command;

/// Classe abstrata para conexões de rede.
/** Há duas possíveis maneiras de se obter objetos que implementem essa
  * interface. Uma é através dos construtores das classes filhas,
  * TCPConnection e UDPConnection. A outra é através do método accept(),
  * que aceita um nova requisição de conexão vinda da rede e a devolve como
  * um novo objeto.
  */
class Connection {

  public:

    // O destrututor fecha o socket
    virtual ~Connection ();

    // Devolve o descritor do socket.
    int sockfd () const { return sockfd_; }

    // Indica que essa conexão serve apenas para receber novas conexões.
    // através da rede usando accept()
    virtual void host (unsigned short port) = 0; 

    // Aceita novas conexões vindas da rede. Só deve ser usada após a devida
    // chamada a host(port). A conexão devolvida segue o mesmo protocolo de rede
    // que a do objeto atual, e está pronta para chamadas de send(cmd) e
    // receive().
    virtual Connection* accept () = 0;

    // Tenta conectar com o endereço passado na porta indicada. Uma vez
    // conectado, é possível executar os métodos send(cmd) e receive().
    virtual bool connect (const std::string& hostname, unsigned short port) = 0;

    // Recebe um comando via rede.
    virtual Command receive () = 0;

    // Envia um comando via rede.
    virtual void send (const Command& cmd) = 0;

    // Devolve a porta local da conexão.
    unsigned short local_port () const;

    // Devolve o endereço local da conexão.
    std::string local_address () const;

    // Devolve a porta remota da conexão.
    unsigned short remote_port () const;

    // Devolve o endereço remoto da conexão.
    std::string remote_address () const;

  protected:

    // Construtor protegido para impedir intâncias diretas da classe Connection,
    // que é abstrata. Recebe o socket usado na conexão, que será fechado quando
    // o objeto for destruído.
    Connection (int sockfd);

    // Devolvem ponteiros opacos para as informações locais e remotas.
    const sockaddr* local_info () const {
      return (const struct sockaddr*)(&local_info_);
    }
    const sockaddr* remote_info () const {
      return (const struct sockaddr*)(&remote_info_);
    }

    // Configura as informações locais da conexão...
    // ... passando os dados explícitos;
    void set_local_info (short family, unsigned long address,
                         unsigned short port);
    // ... copiando de uma estrutura sockaddr_in fornecida;
    void set_local_info (const struct sockaddr_in& info) {
      local_info_ = info;
    }
    // ... copiando as informações locais de outra conexão;
    void set_local_info (const Connection* another) {
      local_info_ = another->local_info_;
    }
    // .. ou tentando obter as informações sozinho (usando getsockname).
    void set_local_info ();

    // Configura as informações remotas da conexão...
    // ... copiando de uma estrutura sockaddr_in fornacida;
    void set_remote_info (const struct sockaddr_in& info) {
      remote_info_ = info;
    }
    // ... ou passando os dados explicitamente (address tem que ser válido).
    void set_remote_info (short family, const std::string& address,
                          unsigned short port);

    // Associa o socket da conexão ao endereço local.
    void bind ();

    // Devolve o tamanho dos atributos local_into_ e remote_info_
    static int info_size () { return sizeof(struct sockaddr_in); }

  private:

    // Descritor do socket.
    int                 sockfd_;
    // Informação dos endereços local e remoto da conexão.
    struct sockaddr_in  local_info_,
                        remote_info_;

};

} // namespace ep2

#endif // EP2_CONNECTION_H_


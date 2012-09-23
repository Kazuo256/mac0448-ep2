
#ifndef EP2_CONNECTION_H_
#define EP2_CONNECTION_H_

#include <cstdlib>
#include <cstdio>
#include <netinet/in.h>

#include <string>
#include <iostream>

namespace ep2 {

class Command;

/// Interface para conexões de rede.
/** Há duas possíveis maneiras de se obter objetos que implementem essa
  * interface. Uma é através dos construtores das classes filhas,
  * TCPConnection e UDPConnection. A outra é através do método accept(),
  * que aceita um nova requisição de conexão vinda da rede e a devolve como
  * um novo objeto.
  */
class Connection {

  public:

    // O destrututor fecha o socket
    virtual ~Connection () {
#ifdef EP2_DEBUG
      std::cout << "[Closing socket " << sockfd_ << "]\n";
#endif
      close(sockfd_);
    }

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
    virtual unsigned short local_port () const = 0;

    // Devolve o endereço local da conexão.
    virtual std::string local_address () const = 0;

    // Devolve a porta remota da conexão.
    virtual unsigned short remote_port () const = 0;

    // Devolve o endereço remoto da conexão.
    virtual std::string remote_address () const = 0;

  protected:

    // Construtor protegido para impedir intâncias diretas da classe Connection,
    // que é abstrata. Recebe o socket usado na conexão, que será fechado quando
    // o objeto for destruído.
    Connection (int sockfd) : sockfd_(sockfd) {
      if (sockfd_ < 0) {
        std::cerr << "[Connection::Connection - Socket inválido]\n";
        exit(1);
      }
    }

  private:

    // Descritor do socket.
    int                 sockfd_;
    // Informação do endereço local e remoto da conexão.
    //struct sockaddr_in  local_info_,
    //                    remote_info_;

};

} // namespace ep2

#endif // EP2_CONNECTION_H_


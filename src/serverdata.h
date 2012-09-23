
#ifndef EP2_SERVERDATA_H_
#define EP2_SERVERDATA_H_

#include <vector>
#include <string>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <tr1/functional>

#include "connection.h"
#include "eventmanager.h"

namespace ep2 {
 
/// Representa os dados do servidor (tabelas de conexões e nicks dos clientes).
/** Existem três tabelas nessa classe:
  *   (+) a tabela de conexões gerais, que guarda todas as conexões que o
  *       servidor aceitou e ainda estão ativas;
  *   (+) a tabela de nicks, que associa nicks às conexões secundárias dos
  *       clientes (aquela usada para eles receberem comandos do servidor); e
  *   (+) a tabela de ligações entre conexões primárias e nicks, para saber o
  *       nick de clientes que mandam comandos para o servidor.
  */
class ServerData {

  public:

    // Construtor.
    ServerData () {}

    // Destrutor. Deleta todas as conexões registradas.
    ~ServerData () {
      for (ConnectionTable::iterator it = connections_.begin(); it != connections_.end(); ++it)
        delete *it;
      connections_.clear();
    }

    // Métodos para manipular a tabela de conexões gerais dos clientes.
    void add_connection (Connection* connection);
    void erase_connection (Connection* connection);

    // Métodos para manipular as ligações entre nicks e clientes.
    void link_connections (int key, const std::string& user);
    std::string get_link (int key) const;
    void remove_link (int key);

    // UserTable
    void set_user (const std::string& user, Connection* connection);
    Connection* get_connection (const std::string& user);
    bool used (const std::string& user);
    void erase_connection (const std::string& key);
    std::string get_user (const Connection* connection);
    void erase_user (const Connection* connection);
    void erase_user (const std::string& key);
    void get_list (std::vector<std::string>& list);
    
  private:
    typedef std::tr1::unordered_set<Connection*>              ConnectionTable;
    typedef std::tr1::unordered_map<int, std::string>         LinkTable;
    typedef std::tr1::unordered_map<std::string, Connection*> UserTable;

    UserTable         user_;
    ConnectionTable   connections_;
    LinkTable         links_;
};
  
} // namespace ep2

#endif

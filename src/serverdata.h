
#ifndef EP2_SERVERDATA_H_
#define EP2_SERVERDATA_H_

#include <vector>
#include <string>
#include <tr1/unordered_map>
#include <tr1/functional>

#include "connection.h"
#include "eventmanager.h"

namespace ep2 {
 
class ServerData {
  public:
    ServerData (EventManager& manager) :
      manager_(manager) {}
    ~ServerData () {
      for (ConnectionTable::iterator it = connections_.begin(); it != connections_.end(); ++it)
        delete it->second;
      connections_.clear();
    }
    // ConnectionTable
    void set_connection (Connection* connection);
    Connection* get_connection (int key);
    bool used (int key);
    void erase_connection (int key);
    // LinkTable
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
    // Other coisas
    void add_event (int fd, const EventManager::Callback& callback);
    
  private:
    typedef std::tr1::unordered_map<int, Connection*>         ConnectionTable;
    typedef std::tr1::unordered_map<int, std::string>         LinkTable;
    typedef std::tr1::unordered_map<std::string, Connection*> UserTable;

    UserTable         user_;
    ConnectionTable   connections_;
    LinkTable         links_;
    EventManager&     manager_;
};
  
} // namespace ep2

#endif

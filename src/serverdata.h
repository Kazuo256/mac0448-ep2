
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
      for (ConnectionTable::iterator it = table_.begin(); it != table_.end(); ++it)
        delete it->second;
      table_.clear();
    }
    // ConnectionTable
    void set_connection (Connection* connection);
    Connection* get_connection (int key);
    void erase_connection (int key);
    // UserTable
    void set_user (const std::string& user, Connection* connection);
    Connection* get_connection (const std::string& user);
    void erase_connection (const std::string& key);
    // Other coisas
    void add_event (int fd, const EventManager::Callback& callback);
    
  private:
    typedef std::tr1::unordered_map<int, Connection*>     ConnectionTable;
    typedef std::tr1::unordered_map<std::string, Connection*>  UserTable;

    UserTable         user_;
    ConnectionTable   table_;
    EventManager&     manager_;
};
  
} // namespace ep2

#endif
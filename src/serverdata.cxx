
#include "serverdata.h"

namespace ep2 {

using std::string;
using std::vector;

// Métodos para manipular a tabela de conexões gerais dos clientes.

void ServerData::add_connection (Connection* connection) {
  connections_.insert(connection);
}

void ServerData::erase_connection (Connection* connection) {
	connections_.erase(connection);
}

// Métodos para manipular os nicks registrados.

void ServerData::set_user (const string& user, Connection* connection) {
  user_[user] = connection;
}

Connection* ServerData::get_user (const string& user) {
  UserTable::const_iterator it = user_.find(user);
  if (it == user_.end()) return NULL;
  return it->second;
}

bool ServerData::used (const string& user) {
  if (get_user(user)) return true;
  return false; 
}

void ServerData::erase_user (const string& key) {
  user_.erase(key);
}

void ServerData::get_list (vector<string>& list) {
  for (UserTable::iterator it = user_.begin(); it != user_.end(); ++it) {
    list.push_back(it->first);
  }
}

// Métodos para manipular as ligações entre nicks e clientes.

void ServerData::link_connections (int key, const std::string& user) {
  links_[key] = user;
}

string ServerData::get_link (int key) const {
  LinkTable::const_iterator it = links_.find(key);
  return (it != links_.end()) ? it->second : "";
}

void ServerData::remove_link (int key) {
  links_.erase(key);
}

} // namespace ep2


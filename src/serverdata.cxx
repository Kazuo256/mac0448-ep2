
#include "serverdata.h"

using std::string;

namespace ep2 {
// ConnectionTable
void ServerData::set_connection (Connection* connection) {
	table_[connection->sockfd()] = connection;
}

Connection* ServerData::get_connection (int key) {
  ConnectionTable::const_iterator it = table_.find(key);
  if (it == table_.end()) return NULL;
  return it->second;
}


bool ServerData::used (int key) {
  if (get_connection(key)) return true;
  return false; 
}

void ServerData::erase_connection (int key) {
	table_.erase(key);
}
// UserTable
void ServerData::set_user (const string& user, Connection* connection) {
  user_[user] = connection;
}

Connection* ServerData::get_connection (const string& user) {
  UserTable::const_iterator it = user_.find(user);
  if (it == user_.end()) return NULL;
  return it->second;
}

bool ServerData::used (const string& user) {
  if (get_connection(user)) return true;
  return false; 
}

void ServerData::erase_connection (const string& key) {
  user_.erase(key);
}

void get_list (std::vector<string> list&) {
  for (UserTable::iterator it = table_.begin(); it != table_.end(); ++it) {
    list.push_back(it->first);
  }
}

void ServerData::add_event (int fd, const EventManager::Callback& callback) {
  manager_.add_event(fd, callback);
}

} // namespace ep2
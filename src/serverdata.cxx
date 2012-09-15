
#include "serverdata.h"

namespace ep2 {

using std::string;
using std::vector;

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

string ServerData::get_user (const Connection* connection) {
  UserTable::iterator user = user_.begin();
  for (UserTable::iterator it = user_.begin(); it != user_.end(); ++it) {
    if (it->second == connection) user = it;
  }
  if (!(user == user_.begin())) {
    return user->first;
  } 
  return "";
}

void ServerData::erase_user (const Connection* connection) {
  if (get_user(connection) != "") {
    table_.erase(toBeRemoved);
  }
}

void ServerData::get_list (vector<string>& list) {
  for (UserTable::iterator it = user_.begin(); it != user_.end(); ++it) {
    list.push_back(it->first);
  }
}

void ServerData::add_event (int fd, const EventManager::Callback& callback) {
  manager_.add_event(fd, callback);
}

} // namespace ep2

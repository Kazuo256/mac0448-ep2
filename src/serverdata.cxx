
#include "serverdata.h"

namespace ep2 {

using std::string;
using std::vector;

// ConnectionTable
void ServerData::add_connection (Connection* connection) {
  connections_.insert(connection);
}

void ServerData::erase_connection (Connection* connection) {
	connections_.erase(connection);
}

// LinkTable

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
  UserTable::iterator user = user_.end();
  for (UserTable::iterator it = user_.begin(); it != user_.end(); ++it) {
    if (it->second == connection) user = it;
  }
  if (user != user_.end()) {
    return user->first;
  } 
  return "";
}

void ServerData::erase_user (const Connection* connection) {
  string to_be_removed = get_user(connection);
  if (to_be_removed.size())
    user_.erase(to_be_removed);
}

void ServerData::erase_user (const string& key) {
  user_.erase(key);
}

void ServerData::get_list (vector<string>& list) {
  for (UserTable::iterator it = user_.begin(); it != user_.end(); ++it) {
    list.push_back(it->first);
  }
}

} // namespace ep2

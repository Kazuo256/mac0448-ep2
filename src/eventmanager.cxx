
#include "eventmanager.h"

#include <cstdlib>
#include <cstdio>
#include <poll.h>

#define BASE_SIZE 16

namespace ep2 {

using std::vector;

void EventManager::add_event (int fd, const Callback& callback) {
  fds_[fd] = callback;
}

void EventManager::loop () {
  while (true) {
    vector<int> fds;
    // poll for new events
    poll(fds);
    // loop through events
    for (vector<int>::iterator it = fds.begin(); it != fds.end(); ++it) {
      switch (call_event(*it)) {
        case CONTINUE: break; // does nothing
        case STOP:
          fds_.erase(*it);
          break;
        case EXIT:
          return;
        case NOTFOUND:
        default:
          puts("EVENT NOT FOUND");
          break;
      }
    }
  }
}

EventManager::Status EventManager::call_event (int fd) {
  EventTable::const_iterator event = fds_.find(fd);
  if (event == fds_.end())
    return NOTFOUND;
  return event->second();
}

void EventManager::poll (vector<int>& ready) {
  size_t pos = 0, size = fds_.size();
  struct pollfd *fds = new struct pollfd[size];
  for (EventTable::iterator it = fds_.begin(); it != fds_.end(); ++it, ++pos) {
    fds[pos].fd = it->first;
    fds[pos].events = POLLIN;
    fds[pos].revents = 0;
  }
  ::poll(fds, size, -1);
  for (pos = 0; pos < size; ++pos)
    if (fds[pos].revents & POLLIN)
      ready.push_back(fds[pos].fd);
  delete[] fds;
}

} // namespace ep2


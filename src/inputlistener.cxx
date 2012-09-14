
#include "inputlistener.h"

#include <cstdlib>
#include <poll.h>

#define BASE_SIZE 16

namespace ep2 {

using std::vector;
using std::list;

void InputListener::add_input (int fd) {
  fds_.push_back(fd);
}

void InputListener::remove_input (int fd) {
  fds_.remove(fd);
}

void InputListener::poll (vector<int>& ready) {
  size_t pos = 0, size = fds_.size();
  struct pollfd *fds = new struct pollfd[size];
  for (list<int>::iterator it = fds_.begin(); it != fds_.end(); ++it, ++pos) {
    fds[pos].fd = *it;
    fds[pos].events = POLLIN;
    fds[pos].revents = 0;
  }
  ::poll(fds, fds_.size(), -1);
  for (pos = 0; pos < size; ++pos)
    if (fds[pos].revents & POLLIN)
      ready.push_back(fds[pos].fd);
  delete[] fds;
}

} // namespace ep2


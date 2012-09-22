
#include "eventmanager.h"

#include <iostream>
#include <poll.h>

namespace ep2 {

using std::vector;
using std::cout;

void EventManager::add_event (int fd, const Callback& callback) {
  fds_[fd] = callback;
}

void EventManager::loop () {
  while (true) {
    vector<int> fds;
    // espera até haver eventos a serem tratados
    poll(fds);
    // trata cada evento
    for (vector<int>::iterator it = fds.begin(); it != fds.end(); ++it) {
      switch (call_event(*it)) {
        case CONTINUE:
          break;              // não faz nada
        case STOP:
          fds_.erase(*it);    // remove o evento
          break;
        case EXIT:
          return;             // pára de tratar eventos
        case NOTFOUND:
        default:
#ifdef EP2_DEBUG
          // Avisa que um evento inesperado aconteceu, mas não faz nada.
          cout << "EVENT NOT FOUND";
#endif
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
  // Usa a função poll do C para detectar os descritores com input.
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


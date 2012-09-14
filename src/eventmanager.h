
#ifndef EP2_INPUTLISTENER_H_
#define EP2_INPUTLISTENER_H_

#include <vector>
#include <tr1/functional>
#include <tr1/unordered_map>

namespace ep2 {

class EventManager {

  public:

    enum Status {
      CONTINUE,
      STOP,
      NOTFOUND,
      EXIT
    };

    typedef std::tr1::function<Status (void)> Callback;

    EventManager () {}
    ~EventManager () { fds_.clear(); }

    void add_event (int fd, const Callback& callback);
    void loop ();

  private:

    typedef std::tr1::unordered_map<int, Callback> EventTable;

    EventTable fds_;

    Status call_event (int fd);
    void poll (std::vector<int>& ready);

};

} // namespace ep2

#endif // EP2_INPUTLISTENER_H_


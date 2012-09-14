
#ifndef EP2_INPUTLISTENER_H_
#define EP2_INPUTLISTENER_H_

#include <vector>
#include <tr1/functional>
#include <tr1/unordered_map>

namespace ep2 {

class EventListener {

  public:

    enum Status {
      CONTINUE,
      STOP,
      NOTFOUND,
      EXIT
    };

    typedef std::tr1::function<Status (void)> Callback;

    EventListener () {}
    ~EventListener () { fds_.clear(); }

    void add_input (int fd, const Callback& callback);
    void remove_input (int fd);
    void listen ();
    void poll (std::vector<int>& ready);

  private:

    typedef std::tr1::unordered_map<int, Callback> EventTable;

    EventTable fds_;

    Status call_event (int fd);

};

} // namespace ep2

#endif // EP2_INPUTLISTENER_H_



#ifndef EP2_INPUTLISTENER_H_
#define EP2_INPUTLISTENER_H_

#include <vector>
#include <list>

struct pollfd;

namespace ep2 {

class InputListener {

  public:

    InputListener () {}
    ~InputListener () {}

    void add_input (int fd);
    void remove_input (int fd);

    void poll (std::vector<int>& ready);

  private:

    std::list<int>  fds_;

};

} // namespace ep2

#endif // EP2_INPUTLISTENER_H_


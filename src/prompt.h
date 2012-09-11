
#ifndef EP2_PROMPT_H_
#define EP2_PROMPT_H_

#include <string>
#include <map>

namespace ep2 {

class Prompt {
  public:
    Prompt (int sockfd) :
      sockfd_(sockfd) {}
    void init ();
    void run ();
  private:
    typedef void (*cmd_handler) (const std::string& arg, const std::string& data);
    int sockfd_;
    std::map<std::string, cmd_handler> cmd_map;
};

} // namespace ep2

#endif


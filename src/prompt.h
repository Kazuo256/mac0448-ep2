
#ifndef EP2_PROMPT_H_
#define EP2_PROMPT_H_

#include <string>
#include <map>

namespace ep2 {

class Prompt {
  public:
    Prompt (int sockfd) :
      sockfd_(sockfd) {}
    ~Prompt () { cmd_map_.clear(); }
    void init ();
    void run ();
  private:
    typedef std::string (*cmd_handler) (const std::string& arg,
                                        const std::string& data);
    typedef std::map<std::string, cmd_handler> CommandMap;
    int         sockfd_;
    CommandMap  cmd_map_;
    std::string check_cmd (const std::string& cmd, const std::string& arg,
                           const std::string& data);
};

} // namespace ep2

#endif


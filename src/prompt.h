
#ifndef EP2_PROMPT_H_
#define EP2_PROMPT_H_

#include <string>
#include <map>

namespace ep2 {

class Connection;
class Command;

class Prompt {
  public:
    Prompt () {}
    ~Prompt () { cmd_map_.clear(); }
    void init ();
    bool send_command (Connection* server);
  private:
    typedef Command (*cmd_handler) (const std::string& arg,
                                    const std::string& data);
    typedef std::map<std::string, cmd_handler> CommandMap;
    CommandMap  cmd_map_;
    Command check_cmd (const std::string& cmd, const std::string& arg,
                       const std::string& data);
};

} // namespace ep2

#endif


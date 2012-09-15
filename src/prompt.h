
#ifndef EP2_PROMPT_H_
#define EP2_PROMPT_H_

#include <string>
#include <tr1/unordered_map>
#include <tr1/functional>

namespace ep2 {

class Connection;
class Command;

class Prompt {
  public:
    typedef std::tr1::function<void (const std::string&,
                                     const std::string&)> CommandHandler;
    Prompt () {}
    ~Prompt () { cmd_map_.clear(); }
    void init ();
    void add_command (const std::string& cmd, const CommandHandler& handler);
    bool check_input ();
  private:
    typedef std::tr1::unordered_map<std::string, CommandHandler> CommandMap;
    CommandMap  cmd_map_;
    void run_cmd (const std::string& cmd, const std::string& arg,
                  const std::string& data);
};

} // namespace ep2

#endif


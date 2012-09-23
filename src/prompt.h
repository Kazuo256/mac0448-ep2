
#ifndef EP2_PROMPT_H_
#define EP2_PROMPT_H_

#include <string>
#include <tr1/unordered_map>
#include <tr1/functional>

namespace ep2 {

class Connection;

/// Representa um prompt de comandos.
/** Para usar um prompt, basta instancias um objeto, chamar init() e registrar
  * os comandos que se deseja oferecer ao usário através da função
  * add_command(). Depois disso, usa-se o método check_input() para obter os
  * comandos do usuário. Essa classe é compatível com a classe EventManager:
  * basta chamar check_input() quando o decritor da entrada padrão tiver inputs
  * disponíveis.
  *
  * No caso desse prompt, ele trata comandos de terminal da seguinte forma:
  *   > {nome-do-comando} {argumento} {texto}
  * onde {nome-do-comando} é a string com a qual o comando foi registrado usando
  * add_command(), {argumento} é a primeira palavra após o comando e {texto} é
  * tudo que vem depois do argumento. Assim, nos clientes temos, por exemplo, o
  * comando:
  *   > /msg user mensagem para o usuário
  * que chama o comando "msg" com argumento "user" e texto "mensagem para o
  * usuário".
  */
class Prompt {

  public:

    // Comandos de prompt são tratados usando funções compatíveis com esse tipo.
    typedef std::tr1::function<void (const std::string&,
                                     const std::string&)> CommandHandler;

    // Construtor. Começa sem nenhum comando registrado.
    Prompt () {}

    // Destrutor. Limpa a lista de comandos.
    ~Prompt () { cmd_map_.clear(); }

    // Inicializa o prompt.
    void init ();

    // Registra um novo comando "cmd" no prompt que usa handler para ser tratdo.
    void add_command (const std::string& cmd, const CommandHandler& handler);

    // Verifica o próximo input do usuário. Devolve false se o usuário encerrou
    // a entrada com EOF (CTRL+D), e true caso contrário.
    bool check_input ();

  private:

    // Representa uma tabela de comandos.
    typedef std::tr1::unordered_map<std::string, CommandHandler> CommandMap;

    // Tabela de comandos do prompt.
    CommandMap  cmd_map_;

    // Executa um comando usando o argumento e o texto passados.
    void run_cmd (const std::string& cmd, const std::string& arg,
                  const std::string& data);

};

} // namespace ep2

#endif


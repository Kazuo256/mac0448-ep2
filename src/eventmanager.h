
#ifndef EP2_INPUTLISTENER_H_
#define EP2_INPUTLISTENER_H_

#include <vector>
#include <tr1/functional>
#include <tr1/unordered_map>

namespace ep2 {

/// Classe para gerenciar eventos que ocorrem em resposta a inputs de file
/// descriptors, como o terminal ou os sockets usados nas conexões.
/** Basta criar um objeto dessa classe e registrar eventos usando add_event()
  * para depois usar loop() para deixar o gerenciador cuidando deles. Eventos
  * são registrados usando funções de callback que devem devolver o estado do
  * evento para que o gerenciador possa saber o que fazer. Ver abaixo para mais
  * informações.
  */
class EventManager {

  public:

    // Todo evento deve devolve o estado em que se encontra, dentre as opções
    // no nesse enum, com a exceção de NOTFOUND, que é de uso interno ao
    // EventMAnager.
    enum Status {
      CONTINUE, // o evento deve continuar sendo tratado
      STOP,     // o evento não deve ser mais tratado, e pode ser removido
      NOTFOUND, // o evento não foi encontrado
      EXIT      // o gerenciador deve encerrar todas suas atividades
    };

    // As funções que tratam eventos devem ser compatíveis com esse tipo.
    typedef std::tr1::function<Status (void)> Callback;

    // Construtor. Começa sem nenhum evento registrado.
    EventManager () {}

    // Destrutor. Limpa a lista de eventos.
    ~EventManager () { fds_.clear(); }

    // Adiciona um evento associado ao descritor passado. A função de callback
    // será chamada sempre que o descritor tiver inputs.
    void add_event (int fd, const Callback& callback);

    // O gerenciador entra em loop, esperando por sinais de inputs no
    // descritores e tratando cada caso com o devido evento. Mesmo depois de
    // entrar nesse estado, é possível adicionar mais eventos. O gerenciador
    // só para quando um evento devolve o estado EXIT ou quando o programa é
    // encerrado (através de exit(), por exemplo).
    void loop ();

  private:

    // Representa uma tabela de eventos.
    typedef std::tr1::unordered_map<int, Callback> EventTable;

    // Tabela de eventos associados aos desrcritores.
    EventTable fds_;

    // Tenta chamar o evento associado ao descritor passado, devolvendo NOTFOUND
    // se não houver evento associado a ele.
    Status call_event (int fd);
    
    // Devolve no vetor ready os descritores que receberam inputs e portanto
    // devem ser tratados com os devidos eventos. Se não houver nenhum input em
    // nenhum dos descritores, a função espera até que algum apareça.
    void poll (std::vector<int>& ready);

};

} // namespace ep2

#endif // EP2_INPUTLISTENER_H_


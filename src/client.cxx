
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <tr1/unordered_map>

#include "prompt.h"
#include "connection.h"
#include "TCPconnection.h"
#include "UDPconnection.h"
#include "eventmanager.h"
#include "command.h"

// Essa porta é usada apenas para que os clientes possam abrir uma conexão TCP
// entre si para enviar arquivos.
#define EP2_TRANSFER_HOST_PORT 8080

using std::string;
using std::stringstream;
using std::ifstream;
using std::ofstream;
using std::ios_base;
using std::cout;
using std::cerr;
using std::tr1::unordered_map;

using ep2::Prompt;
using ep2::Connection;
using ep2::TCPConnection;
using ep2::UDPConnection;
using ep2::EventManager;
using ep2::Command;

// AVISO: esse arquivo acabou ficando muito grande pois não tivemos tempo de
// refatorá-lo mais. A ideia era ter implementado um ClientHandler a partir da
// interface CommandHandler, mas não deu.

// Informações sobre o host.
static string                       hostname;
static unsigned short               port;
// Gerenciador de eventos de input.
static EventManager                 manager;
// Prompt para o usuário.
static Prompt                       prompt;
// Conexões com o servidor.
//  server_input:   conexão primária, usada para enviar comandos para o servidor
//                  e obter as respostas quando houver.
//  server_output:  conexão secundŕia, usada para receber comandos do servidor.
static Connection                   *server_output, *server_input;
// ID desse cliente no servidor.
static int                          ID = -1;
// Lista de pedidos pendentes de transferências vindas de outros clientes
// conectados no servidor.
static unordered_map<string,string> senders;

// Cria as conexões usando TCP ou UDP de acordo com o parâmetro passado.
static void create_server_connections (bool udp) {
  if (udp) {
    server_input = new UDPConnection;
    server_output = new UDPConnection;
  } else {
    server_input = new TCPConnection;
    server_output = new TCPConnection;
  }
}

//// Eventos de input do cliente ////

// Quando o usuário digita algo no terminal.
static EventManager::Status prompt_event () {
  // Delega trabalho de tratar o input para o prompt.
  // Se devolver false é porque o usuário quer encerrar o programa.
  return prompt.check_input()
    ? EventManager::CONTINUE  // continua ouvindo o terminal
    : EventManager::EXIT;     // para de tratar todos os eventos
}

// Quando o servidor manda um comando pela conexão secundária.
static EventManager::Status server_event () {
  // Pega o comando do servidor.
  Command cmd = server_output->receive();
#ifdef EP2_DEBUG
  cout << "\n[Comando recebido: " << static_cast<string>(cmd) << "]";
#endif
  // Pode ser um comando de mensagem de outro cliente ou de transferência de
  // arquivo de outro cliente.
  switch (cmd.opcode()) {
    case Command::MSG:
    {
      // Verifica integridade do comando.
      if (cmd.num_args() < 2)
        cout << "\n[Mensagem do servidor está mal formada]\n";
      else {
        // Se estiver OK, exibe para o usuário.
        cout << "\n[Messagem de '" << cmd.arg(0) << "':] ";
        cout << cmd.arg(1) << "\n";
        prompt.init();
      }
    } break;
    case Command::SEND:
    {
      // Verifica integridade do comando
      if (cmd.num_args() < 2)
        cout << "\n[Comando 'send' enviado pelo servidor está mal formado]\n";
      else {
        // Se estiver OK, avisa o usuário e adiciona na lista de transferências
        // pendentes.
        senders[cmd.arg(0)] = cmd.arg(1);
        cout  << "\n['" << cmd.arg(0) << "' quer enviar-lhe o arquivo '"
              << cmd.arg(1) << "']";
        cout  << "\n[Para aceitá-lo use '/accept <user>']\n";
        prompt.init();
      }
    } break;
  }
  // Sempre continua ouvindo comandos do servidor.
  return EventManager::CONTINUE;
}

//// Eventos de prompt do usuário ////

// Flag para saber se a conexão secundário já foi estabelecida.
static bool     secondary_connected = false;
// Nick sendo atualmente utilizado. Vazio significa que ainda não tem um nick.
static string   current_nick = "";

// Quando o usuário pede para mudar o nick.
static void nick_event (const string& nick, const string& unused) {
  if (nick.empty())
    cout << "Nick vazio inválido.\n";
  else {
    if (!secondary_connected) {
      // Abre conexão secundária se ainda não havia.
      server_output->connect(hostname, port);
      secondary_connected = true;
    }
    // Pede ao servidor para associar um novo nick ao ID
    stringstream ID_string;
    ID_string << ID;
    server_output->send(Command::nick(nick, ID_string.str()));
    // Verifica resposta do servidor.
    Command response = server_output->receive();
    if (response.opcode() == Command::REFUSE_NICK)
      cout << "[Nick '" << nick << "' foi recusado]\n";
    else if (response.opcode() == Command::ACCEPT_NICK) {
      cout << "[Novo nick confirmado: '" << nick << "'].\n";
      // Em caso positivo, renova o evento de receber comandos e atualiza o
      // nick atual.
      manager.add_event(server_output->sockfd(), server_event);   
      current_nick = nick;
    }
    else
      cout << "[Resposta inesperada do servidor]\n";
  }
} 

// Quando o cliente pede para listar os nick registrados no servidor.
static void list_event (const string& unused1, const string& unused2) {
  // Envia requisição de lista de nicks.
  server_input->send(Command::list_request());
  // Pega a resposta, verifica ela, e exibe a lista se possível.
  Command response = server_input->receive();
  if (response.opcode() == Command::LIST_RESPONSE) {
    cout << "[Usuários on-line no momento:]\n";
    for (size_t i = 0; i < response.num_args(); ++i)
      cout << "  " << response.arg(i) << "\n";
  }
  else
    cout << "[Resposta inesperada do servidor]\n";
}

// Quando o usuário quer enviar uma mensagem para outro cliente conectado no
// servidor.
static void msg_event (const string& target, const string& msg) {
  if (current_nick.empty())
    cout << "[Você não pode enviar mensagens sem um nick!]\n";
  else if (target.empty() || msg.empty())
    cout << "[Faltou destinatário ou a mensagem etá vazia]\n";
  else {
    // Envia comando de mensagem e analiza a resposta.
    server_input->send(Command::msg(target, msg));
    Command response = server_input->receive();
    if (response.opcode() == Command::MSG_OK)
      cout << "[Mensagem enviada para '" << target << "']\n";
    else if (response.opcode() == Command::MSG_FAIL) {
      cout << "[Não foi possível enviar a mensagem para '" << target << "']\n";
      cout << "[Não há usuário com esse nick no servidor.]\n";
      cout << "[Use /list para saber quem está on-line]\n";
    }
    else
      cout << "[Resposta inesperada do servidor]\n";
  }
}

// Quando o usuário pede para enviar um arquivo para outro cliente conectado no
// servidor.
static void transfer_event (const string& target, const string& filepath) {
  if (current_nick.empty()) {
    cout << "[Você não pode enviar arquivos sem um nick!]\n";
    return;
  }
  if (target.empty() || filepath.empty()) {
    cout << "[Usuário alvo ou arquivo a ser enviado não definidos]\n";
    return;
  }
  // Abre o arquivo para enviá-lo se ele exitir.
  ifstream file;
  file.open(filepath.c_str(), ios_base::in | ios_base::binary);
  if (file.fail()) {
    cout << "[Não foi possível acessar o arquivo '" << filepath << "']\n";
    return;
  }
  // Avisa que quer realizar a transferência.
  server_input->send(Command::send(target, filepath));
  cout << "[Tentando enviar arquivo para o usuário '" << target << "']\n";
  // Aguarda resposta e trata ela de acordo.
  Command response = server_output->receive();
  switch (response.opcode()) {
    case Command::SEND_OK:
      if (response.num_args() < 2)
        cout << "[Resposta inesperada do servidor]\n";
      else {
        // A confirmação de transferência vem com as informações do outro
        // cliente. Assim é possível abrir uma conexão TCP direta e transferir
        // o arquivo parte por parte (Jack, o Estripador feelings).
        TCPConnection transfer;
        cout << "[Enviando arquivo...]\n";
        transfer.connect(response.arg(0), EP2_TRANSFER_HOST_PORT);
        char chunk[8*255+1];
        while (file.good()) {
          file.read(chunk, 8*255);
          size_t n = file.gcount();
          chunk[n] = '\0';
          transfer.send(Command::chunk(chunk));
          // Sempre confirma a transferência
          Command check = transfer.receive();
          if (check.opcode() != Command::CONTINUE) break;
        }
        cout << "[Transferência concluída]\n";
      }
      break;
    case Command::SEND_FAIL:
      // Ou o usuário destinatário não existe, ou ele recusou a transferência.
      // O comando enviado pelo servidor contém essas informações.
      cout << "[Não foi possível enviar o arquivo para '" << target << "']\n";
      if (response.num_args() > 0)
        cout << "[" << response.arg(0) << "]\n";
      break;
    default:
      cout << "[Resposta inesperada do servidor]\n";
      break;
  }
  file.close();
}

// Função auxiliar que mostra quem está atualmente tentando enviar arquivos
// para o usuário.
static void show_senders () {
  unordered_map<string,string>::iterator it;
  if (senders.empty())
    cout << "[No momento, ninguém está tentando te enviar nenhum arquivo]\n";
  else {
    cout << "[Usuários tentando te enviar arquivos no momento:]\n";
    for (it = senders.begin(); it != senders.end(); ++it)
      cout << "  " << it->first << ": "<< it->second << "\n";
  }
}

// Quando o usuário quer aceitar uma transferência enviada por outro usuário.
static void accept_event (const string& sender, const string& unused) {
  if (current_nick.empty()) {
    cout << "[Você não pode receber arquivos sem um nick!]\n";
    return;
  }
  if (sender.empty()) {
    cout << "[Especifique de quem quer aceitar o arquivo]\n";
    show_senders();
    return;
  }
  // Verifica se de fato há uma transferÊncia pendente desse usuário.
  unordered_map<string,string>::iterator it;
  if ((it = senders.find(sender)) == senders.end()) {
    cout << "[Nenhuma transferência pendente de '" << sender << "']\n";
    return;
  }
  // Envia confirmação de transferÊncia para o servidor.
  server_input->send(Command::accept(sender));
  // Agora abre uma conexão temporária para receber a conexão do outro cliente.
  TCPConnection temp;
  temp.host(EP2_TRANSFER_HOST_PORT);
  Connection *download = temp.accept();
  // Abre o arquivo para escrever os dados recebidos.
  ofstream file((string("downloads/")+it->second).c_str(),
                ios_base::out | ios_base::binary);
  cout << "[Recebendo arquivo...]\n";
  // Recebe os pedaços do arquivo.
  while (true) {
    Command bytes = download->receive();
    // Chega um DISCONNECT quando o outro cliente acabou de enviar todos os
    // pedaços do arquivo.
    if (bytes.opcode() == Command::DISCONNECT) break;
    if (bytes.opcode() != Command::CHUNK) {
      cout << "[Outro usuário enviou pacote desconhecido "<<bytes.opcode()<<"]\n";
      break;
    }
    if (bytes.num_args() < 1) {
      cout << "[Dados perdidos?]\n";
      break;
    }
    for (size_t i = 0; i < bytes.num_args(); ++i)
      file << bytes.arg(i);
    download->send(Command::cont());
  }
  cout << "[Arquivo recebido com sucesso]\n";
  // Fecha o arquivo e a conexão de transferência, aĺém de remover o outro
  // cliente da lista de transferências pendentes.
  file.close();
  delete download;
  senders.erase(sender);
}

// Quando o usuário recusa uma transferÊncia de arquivo.
static void refuse_event (const string& sender, const string& unused) {
  if (current_nick.empty()) {
    cout << "[Você não pode recusar arquivos sem um nick!]\n";
    return;
  }
  if (sender.empty()) {
    cout << "[Especifique de quem quer recusar o arquivo]\n";
    show_senders();
    return;
  }
  // Verifica se de fato há tal transferência pendente.
  unordered_map<string,string>::iterator it;
  if ((it = senders.find(sender)) == senders.end()) {
    cout << "[Nenhuma transferência pendente de '" << sender << "']\n";
    return;
  }
  // Avisa o servidor que a transferência foi recusada e remove o outro cliente
  // da lista de transferências pendentes.
  server_input->send(Command::refuse(sender));
  senders.erase(sender);
  cout << "[Transferência de '" << sender << "' recusada]\n";
}

//// MAIN ////

int main(int argc, char **argv) {
	if (argc < 3 || argc > 4) {
    cerr << "Uso: " << argv[0] << " <Endereco IP|Nome> <Porta> [tcp|udp]\n";
		exit(1);
	}
  string protocol = (argc == 4 ? argv[3] : "tcp");
  if (protocol != "tcp" && protocol != "udp") {
    cerr << "Protocolo desconhecido.";
    cerr << "Uso: " << argv[0] << " <Endereco IP|Nome> <Porta> [tcp|udp]\n";
		exit(1);
  }
  // Guarda informações passadas pela linha de comando.
  hostname = argv[1];
  port = atoi(argv[2]);
  bool udp = (protocol == "udp");
  // Cria conexões.
  create_server_connections(udp);
  // Conecta a conexão primária
  server_input->connect(hostname, port);
  // Pede ID para o servidor.
  server_input->send(Command::request_id());
  Command cmd = server_input->receive();
  if (cmd.opcode() == Command::GIVE_ID)
    ID = atoi(cmd.arg(0).c_str());
#ifdef EP2_DEBUG
  cout << "[ID recebido: " << ID << "]\n";
#endif
  // Prepara comandos de prompt e eventos.
  prompt.add_command("/nick", nick_event);
  prompt.add_command("/list", list_event);
  prompt.add_command("/msg", msg_event);
  prompt.add_command("/send", transfer_event);
  prompt.add_command("/accept", accept_event);
  prompt.add_command("/refuse", refuse_event);
  manager.add_event(STDIN_FILENO, EventManager::Callback(prompt_event));
  //  Mensagens informativas para o usuário.
  cout << "[Cliente conectado com sucesso]\n";
  cout << "[Use CTRL+D para encerrar o programa]\n";
  // Ativa o prompt e deixa o gerenciador de eventos cuidar do resto.
  prompt.init();
  manager.loop();
  // Avisa o servidor para desconectar do cliente.
  server_input->send(Command::disconnect());
  if (secondary_connected)
    server_output->send(Command::disconnect());
  // Limpa tudo e cai fora.
  delete server_input;
  delete server_output;
  return 0;
}


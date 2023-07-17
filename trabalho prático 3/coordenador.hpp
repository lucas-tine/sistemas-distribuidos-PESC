#ifndef COORDENADOR_HPP
#define COORDENADOR_HPP

#include "socket_udp.hpp"
#include "mensageiro_ex_mut.hpp"
#include "registro_mensagem_ex_mut.hpp"
#include <thread>
#include <iostream>
#include <deque>
#include <mutex>
#include <tuple>
#include <map>
#include <iomanip>
#include <fstream>

struct endereco_processo {
    unsigned id_processo;
    sockaddr_in endereco;
};

const unsigned long MAX_TIME_WAIT = 3000;

class Coordenador: public mensageiro_ex_mut
{
private:
    SocketUDP* socket_servidor;
    std::thread* servico_terminal;
    std::thread* servico_respostas_udp;
    int porta;
    bool ativo;
    
    std::deque<endereco_processo> fila_acessos;
    std::mutex lock_fila_acessos;

    std::deque<registro_mensagem_ex_mut> historico_mensagens;
    std::mutex lock_historico_mensagens;

    std::string nome_arquivo_log;
    std::mutex lock_log;

public:
    // Construtor
    Coordenador(int porta);
    
    // Aguarda a finalização dos serviços em execução
    void aguardar();

    // Atende as requisições do terminal de comando
    void atender_terminal();

    // Imprime a fila de pedidos atual
    void imprimir_fila_atual();

    // Atende as requisições recebidas via UDP
    void atender_requisicoes();

    // Trata uma requisição recebida via UDP
    void tratar_requisicao(mensagem_udp requisicao);

    // Atende a fila de acessos, fornecendo GRANTs aos processos
    void atender_fila();

    // Registra a recepção de uma mensagem
    void registrar_recepcao(mensagem_udp requisicao);

    // Registra o envio de uma mensagem
    void registrar_envio(std::string msg, unsigned id_processo);

    // Retorna o ID do coordenador (sempre 0)
    unsigned id();
};

#endif

#ifndef SOCKETUDP_HPP
#define SOCKETUDP_HPP

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <stdexcept>
#include <deque>

class SocketUDP {
private:
    bool ok = true;
    int descritor_socket = 0;
    std::thread servidor;
    const static char* ip_local;
    sockaddr_in endereco_socket;
    size_t tamanho_buffer_msg;

public:
    SocketUDP(const int porta, const char *ipv4 = nullptr, size_t tamanho_buffer_msg = 16);
    ~SocketUDP();

    bool aguardar_mensagem( 
        std::string (*processamento_resposta)(std::string) = [](std::string s){return s;},
        unsigned long milissegundos = 0
    );

    std::thread& aguardar_mensagens_multithreaded(
        std::string (*processamento_resposta)(std::string) = [](std::string s){return s;},
        bool manter_servico = true
    );

    bool enviar_mensagem (std::string mensagem);

    void enderecar_a_si (){
        this->endereco_socket.sin_addr.s_addr = inet_addr(this->ip_local);
    }

    bool iniciar_servidor();

    void servir_enquanto(bool *condicao_dinamica, unsigned long verificacao_milissegundos, std::string (*processamento_resposta)(std::string) );
};

#endif

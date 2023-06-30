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
    std::thread servidor;
    const static char* ip_local;

    struct timeval tv;
    fd_set readfds;
    bool timeout_configurado = false;
    
public:
    sockaddr_in endereco_socket;
    size_t tamanho_buffer_msg;
    int descritor_socket = 0;

    SocketUDP(const int porta, const char *ipv4 = nullptr, size_t tamanho_buffer_msg = 16);
    ~SocketUDP();

    bool aguardar_mensagem( 
        std::string (*processamento_resposta)(std::string) = [](std::string s){return s;},
        unsigned long milissegundos = 0
    );

    bool enviar_mensagem (std::string mensagem);

    void enderecar_a_si (){
        this->endereco_socket.sin_addr.s_addr = inet_addr(this->ip_local);
    }

    bool iniciar_servidor();

    void servir_enquanto(bool *condicao_dinamica, unsigned long verificacao_milissegundos, std::string (*processamento_resposta)(std::string) );

    void configurar_timeout(unsigned long milissegundos);
    int aguardar_mensagem_timeout()
};

#endif

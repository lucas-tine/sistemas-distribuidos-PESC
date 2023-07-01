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

struct mensagem_udp {
    std::string mensagem;
    sockaddr_in endereco;
    bool sucesso = 1;
};

class SocketUDP {
private:
    std::thread servidor;
    const static char* ip_local;

    struct timeval tv;
    fd_set readfds;
    bool timeout_configurado = false, comunicacao_anterior = false;
    
public:
    sockaddr_in endereco_socket;
    size_t tamanho_buffer_msg;
    int descritor_socket = 0;

    SocketUDP(const int porta, const char *ipv4 = nullptr, size_t tamanho_buffer_msg = 16);
    ~SocketUDP();

    bool enviar_mensagem (std::string mensagem);
    mensagem_udp receber_mensagem();

    void enderecar_a_si (){
        this->endereco_socket.sin_addr.s_addr = inet_addr(this->ip_local);
    }

    bool iniciar_servidor();
    void configurar_timeout(unsigned long milissegundos);
    int aguardar_mensagem_timeout();
};

#endif

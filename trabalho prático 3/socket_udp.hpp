#ifndef SOCKETUDP_HPP
#define SOCKETUDP_HPP

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>

class SocketUDP {
private:
    bool ok = true;
    int descritor_socket = 0;
    std::thread servidor;

public:
    SocketUDP(const int porta);
    ~SocketUDP();

    void aguardar_mensagens(
        size_t tamanho_buffer, 
        std::string (*processamento_resposta)(std::string) = [](std::string s){return s;},
        bool manter_servico = true
    );
    std::thread& aguardar_mensagens_multithreaded(
        size_t tamanho_buffer, 
        std::string (*processamento_resposta)(std::string) = [](std::string s){return s;},
        bool manter_servico = true
    );

};

#endif

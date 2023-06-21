#ifndef SOCKETUDP_HPP
#define SOCKETUDP_HPP

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class SocketUDP {
private:
    bool ok = true;
    int descritor_socket = 0;

public:
    SocketUDP(const int porta);
    void aguardar_mensagem(size_t tamanho_buffer);
    ~SocketUDP();
};

#endif

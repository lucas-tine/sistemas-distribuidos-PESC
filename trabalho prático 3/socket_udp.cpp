#include "socket_udp.hpp"

SocketUDP::SocketUDP(const int porta) {
    int _socket = socket(AF_INET, SOCK_DGRAM, 0);
    bool erro_socket = _socket == -1;

    if (erro_socket) {
        std::cerr << "Erro ao criar o socket" << std::endl;
        this->ok = false;
        return;
    }

    // Configura o endereço do socket
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(porta);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Associa o endereço ao socket
    erro_socket = bind(_socket, (sockaddr*)&addr, sizeof(addr)) == -1;

    if (erro_socket) {
        std::cerr << "Erro ao associar o endereço ao socket" << std::endl;
        close(_socket);
        this->ok = false;
        return;
    }

    this->descritor_socket = _socket;
}

void SocketUDP::aguardar_mensagem(size_t tamanho_buffer) {
    // Espera por uma mensagem
    char buffer[tamanho_buffer];
    sockaddr_in senderAddr;
    socklen_t senderAddrLen = sizeof(senderAddr);
    ssize_t receivedBytes = recvfrom(this->descritor_socket, buffer, sizeof(buffer), 0,
                                     (sockaddr*)&senderAddr, &senderAddrLen);

    if (receivedBytes == -1) {
        std::cerr << "Erro de conexao" << std::endl;
        close(this->descritor_socket);
        return;
    }

    // Responde a mensagem
    sendto(this->descritor_socket, buffer, receivedBytes, 0,
           (sockaddr*)&senderAddr, senderAddrLen);
}

SocketUDP::~SocketUDP() {
    close(this->descritor_socket);
}
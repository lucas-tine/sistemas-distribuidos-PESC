#include "socket_udp.hpp"

const char* SocketUDP::ip_local = "127.0.0.1"; 

SocketUDP::SocketUDP(const int porta, const char *ipv4, size_t tamanho_buffer_msg) {
    int _socket = socket(AF_INET, SOCK_DGRAM, 0);
    bool erro_socket = _socket == -1;
    this->tamanho_buffer_msg = tamanho_buffer_msg;

    if (erro_socket) {
        throw std::runtime_error("Erro ao criar o socket"); 
    }

    // Configura o endereço do socket
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(porta);
    addr.sin_addr.s_addr = (ipv4 == nullptr) ? htonl(INADDR_ANY): inet_addr(ipv4);
    this->endereco_socket = addr;
    this->descritor_socket = _socket;
}

bool SocketUDP::iniciar_servidor(){
    // Associa o endereço ao socket
    bool erro_socket = bind(this->descritor_socket, (sockaddr*)&(this->endereco_socket), sizeof(sockaddr_in)) == -1;
    if (erro_socket)
        throw std::runtime_error("Erro ao linkar socket"); 
    return !erro_socket;
}

void SocketUDP::configurar_timeout(unsigned long milissegundos)
{
    if (milissegundos == 0) return;
    
    this->tv.tv_sec = 0;
    this->tv.tv_usec = milissegundos * 1000;
    this->timeout_configurado = true;

    FD_ZERO(&this->readfds);
    FD_SET(this->descritor_socket, &this->readfds);
}

int SocketUDP::aguardar_mensagem_timeout()
{
    if (!this->timeout_configurado){
        throw std::logic_error("timeout precisa ser configurado");   
        return -1;
    }
    return select(this->descritor_socket + 1, &this->readfds, NULL, NULL, &this->tv);
}

SocketUDP::~SocketUDP() {
    close(this->descritor_socket);
}

bool SocketUDP::enviar_mensagem(std::string mensagem){
        ssize_t sentBytes = sendto(
            this->descritor_socket, 
            mensagem.c_str(), 
            mensagem.length(), 
            0,
            (sockaddr*) &(this->endereco_socket), 
            sizeof(this->endereco_socket)
        );

        this->comunicacao_anterior = true;
        return (sentBytes != -1);
}

mensagem_udp SocketUDP::receber_mensagem(){
        if (not this->comunicacao_anterior){
            this->iniciar_servidor();
            this->comunicacao_anterior = true;
        }

        sockaddr_in senderAddr;
        socklen_t senderAddrLen = sizeof(senderAddr);
        std::string mensagem;
        char buffer[this->tamanho_buffer_msg];

        ssize_t receivedBytes = recvfrom(this->descritor_socket, buffer, sizeof(buffer), 0,
                                            (sockaddr*)&senderAddr, &senderAddrLen);

        bool erro = (receivedBytes == -1);
        if (erro) return mensagem_udp {std::string(""), sockaddr_in(), false};
            
        return mensagem_udp {std::string(buffer, receivedBytes), senderAddr, true};
    }
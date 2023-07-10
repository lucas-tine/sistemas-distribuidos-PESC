#include "socket_udp.hpp"

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
    bool erro_socket;
    if (not this->comunicacao_anterior){
            erro_socket = bind(this->descritor_socket, (sockaddr*)&(this->endereco_socket), sizeof(sockaddr_in)) == -1;
            if (erro_socket)
                throw std::runtime_error("Erro ao linkar socket"); 
            this->comunicacao_anterior = true;
        }

    return !erro_socket;
}

void SocketUDP::configurar_timeout(unsigned long milissegundos)
{
    this->iniciar_servidor();
    if (milissegundos == 0) return;
    
    this->tv.tv_sec = 0;
    this->tv.tv_usec = milissegundos * 1000;
    FD_ZERO(&this->readfds);
    FD_SET(this->descritor_socket, &this->readfds);
    this->timeout_configurado = true;
}

int SocketUDP::aguardar_mensagem_timeout()
{
    if (!this->timeout_configurado){
        throw std::logic_error("timeout precisa ser configurado");   
        return -1;
    }

    fd_set readfds = this->readfds;
    timeval tv = this->tv;
    return select(this->descritor_socket + 1, &readfds, NULL, NULL, &tv);
}

SocketUDP::~SocketUDP() {
    close(this->descritor_socket);
}

bool SocketUDP::enviar_mensagem(std::string mensagem){
        std::cout << "enviei: " << mensagem << std::endl;
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
        this->iniciar_servidor();

        sockaddr_in senderAddr;
        socklen_t senderAddrLen = sizeof(senderAddr);
        std::string mensagem;
        char buffer[this->tamanho_buffer_msg];

        ssize_t receivedBytes = recvfrom(this->descritor_socket, buffer, sizeof(buffer), 0,
                                            (sockaddr*)&senderAddr, &senderAddrLen);

        bool erro = (receivedBytes == -1);
        if (erro) return mensagem_udp {std::string(""), sockaddr_in(), false};
            
        std::cout << "socket: mensagem recebida " << std::string(buffer, receivedBytes) << std::endl;
        return mensagem_udp {std::string(buffer, receivedBytes), senderAddr, true};
}

mensagem_udp SocketUDP::receber_mensagem_e_enderecar(){
    mensagem_udp mensagem = this->receber_mensagem();
    this->endereco_socket = mensagem.endereco;
    return mensagem;
}
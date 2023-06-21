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

void SocketUDP::aguardar_mensagens(
    size_t tamanho_buffer, 
    std::string (*processamento_resposta)(std::string),
    bool manter_servico
    ) 
    {
    // Espera por uma mensagem
    char buffer[tamanho_buffer];
    sockaddr_in senderAddr;
    socklen_t senderAddrLen = sizeof(senderAddr);
    std::string mensagem;

    std::thread processamento;
    do 
    {
        ssize_t receivedBytes = recvfrom(this->descritor_socket, buffer, sizeof(buffer), 0,
                                        (sockaddr*)&senderAddr, &senderAddrLen);

        bool erro = receivedBytes == -1;

        if (erro) {
            std::cerr << "Erro de conexao" << std::endl;
            close(this->descritor_socket);
            return;
        }
        mensagem = std::string(buffer, receivedBytes);

        // Responde a mensagem
        processamento = std::thread(
            []  ( 
                std::string (*processamento_resposta)(std::string),
                std::string mensagem,
                int descritor_socket, 
                sockaddr_in senderAddr, 
                socklen_t senderAddrLen
                )
            {
                std::cout << "response thread opening ..." << std::endl; 
                std::string resposta = processamento_resposta(mensagem);
                char buffer [resposta.length() + 1];
                strcpy(buffer, resposta.c_str()); 

                sendto(
                    descritor_socket,
                    buffer, 
                    resposta.length() + 1, 
                    0,
                    (sockaddr*)&senderAddr,
                    senderAddrLen
                );
                std::cout << "thread success closing ..." << std::endl; 
            },
            processamento_resposta,
            mensagem,
            this->descritor_socket, 
            senderAddr, 
            senderAddrLen
        );

        if (manter_servico)
            processamento.detach();
    } 
    while (manter_servico);
    processamento.join();
}

SocketUDP::~SocketUDP() {
    close(this->descritor_socket);
}

std::thread& SocketUDP::aguardar_mensagens_multithreaded(
        size_t tamanho_buffer, 
        std::string (*processamento_resposta)(std::string),
        bool manter_servico
    ) {

    this->servidor = std::thread(
        [](SocketUDP* self, size_t tamanho_buffer, std::string (*processamento_resposta)(std::string), bool manter_servico){
            self->aguardar_mensagens(tamanho_buffer, processamento_resposta, manter_servico);
        },
        this,
        tamanho_buffer,
        processamento_resposta,
        manter_servico
    );
    return this->servidor;
}
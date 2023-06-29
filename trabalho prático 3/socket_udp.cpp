#include "socket_udp.hpp"

const char* SocketUDP::ip_local = "127.0.0.1"; 

SocketUDP::SocketUDP(const int porta, const char *ipv4, size_t tamanho_buffer_msg) {
    int _socket = socket(AF_INET, SOCK_DGRAM, 0);
    bool erro_socket = _socket == -1;
    this->tamanho_buffer_msg = tamanho_buffer_msg;

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
    addr.sin_addr.s_addr = (ipv4 == nullptr) ? htonl(INADDR_ANY): inet_addr(ipv4);
    this->endereco_socket = addr;
    this->descritor_socket = _socket;
}

bool SocketUDP::iniciar_servidor(){
    // Associa o endereço ao socket
    bool erro_socket = bind(this->descritor_socket, (sockaddr*)&(this->endereco_socket), sizeof(sockaddr_in)) == -1;
    this->ok = !erro_socket;
    return !erro_socket;
}

bool SocketUDP::aguardar_mensagem(
    std::string (*processamento_resposta)(std::string),
    unsigned long milissegundos
    ) 
{
    // Espera por uma mensagem
    char buffer[this->tamanho_buffer_msg];
    sockaddr_in senderAddr;
    socklen_t senderAddrLen = sizeof(senderAddr);
    std::string mensagem;
    std::thread processamento;

    // Configura o tempo limite
    if (milissegundos > 0)
    {
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = milissegundos * 1000;

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(this->descritor_socket, &readfds);

        int rv = select(this->descritor_socket + 1, &readfds, NULL, NULL, &tv);

        if (rv == -1) {
            // erro na chamada select
            return false;
        } else if (rv == 0) {
            // tempo limite atingido sem receber dados
            return false;
        }
    }

    bool servidor_loop = (milissegundos == 0);
    do 
    {
        ssize_t receivedBytes = recvfrom(this->descritor_socket, buffer, sizeof(buffer), 0,
                                        (sockaddr*)&senderAddr, &senderAddrLen);

        bool erro = receivedBytes == -1;
        if (erro) return false;
        
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
    }
    while (servidor_loop);

    //processamento.join();
    return true;
}


SocketUDP::~SocketUDP() {
    close(this->descritor_socket);
}

std::thread& SocketUDP::aguardar_mensagens_multithreaded(
        std::string (*processamento_resposta)(std::string),
        bool manter_servico
    ) {

    this->servidor = std::thread(
        [](SocketUDP* self, std::string (*processamento_resposta)(std::string), bool manter_servico){
            self->aguardar_mensagem(processamento_resposta, 1000);
        },
        this,
        processamento_resposta,
        manter_servico
    );
    return this->servidor;
}

bool SocketUDP::enviar_mensagem(std::string mensagem){
        ssize_t sentBytes = sendto(
            this->descritor_socket, 
            mensagem.c_str(), 
            mensagem.length(), 
            0,
            (sockaddr*) &(this->endereco_socket), 
            sizeof(sockaddr)
        );
        return (sentBytes != -1);
}

void SocketUDP::servir_enquanto(
    bool *condicao_dinamica, 
    unsigned long verificacao_milissegundos, 
    std::string (*processamento_resposta)(std::string) 
)
{
    // Espera por uma mensagem
    char buffer[this->tamanho_buffer_msg];
    sockaddr_in senderAddr;
    socklen_t senderAddrLen = sizeof(senderAddr);
    std::string mensagem;

    if (verificacao_milissegundos == 0) 
        throw std::invalid_argument("sem tempo para verificar condicao dinamica de servico do coordenador");

    std::cout << "tempo: " << verificacao_milissegundos << std::endl;;

    

    std::deque<std::thread*> processamentos;
    do 
    {
        // Configura o tempo limite
        struct timeval tv;
        tv.tv_sec = verificacao_milissegundos / 1000;
        tv.tv_usec = (verificacao_milissegundos % 1000) * 1000;
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(this->descritor_socket, &readfds);
        int rv = select(this->descritor_socket + 1, &readfds, NULL, NULL, &tv);
        if (rv == -1) {
            // erro na chamada select
            throw std::runtime_error("erro na chamada select");
        } else if (rv == 0) {
            // tempo limite atingido sem receber dados
            if (*condicao_dinamica) continue;
            else break;
        }

        ssize_t receivedBytes = recvfrom(this->descritor_socket, buffer, sizeof(buffer), 0,
                                        (sockaddr*)&senderAddr, &senderAddrLen);

        bool erro = (receivedBytes == -1);
        if (erro) 
            throw std::runtime_error("erro na recepcao da mensagem udp");
            
        mensagem = std::string(buffer, receivedBytes);
        // Responde a mensagem
        processamentos.push_back(new std::thread(
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
            )
        );
    } 
    while (*condicao_dinamica);

    std::cout << "(encerrando respostas)" << std::endl;
    for (std::thread* processamento: processamentos)
        processamento->join();
}

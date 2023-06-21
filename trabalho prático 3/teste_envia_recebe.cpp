#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// ESTE TESTE FOI GERADO PELO CHAT GPT

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " MENSAGEM PORTA" << std::endl;
        return 1;
    }

    std::string mensagem = argv[1];
    int porta = std::stoi(argv[2]);

    // Criando o socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        std::cerr << "Erro ao criar o socket" << std::endl;
        return 1;
    }

    // Configurando o endereço do servidor
    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(porta);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Enviando a mensagem para o servidor
    ssize_t sentBytes = sendto(sockfd, mensagem.c_str(), mensagem.length(), 0,
                               (sockaddr*)&servaddr, sizeof(servaddr));
    if (sentBytes == -1) {
        std::cerr << "Erro ao enviar a mensagem" << std::endl;
        close(sockfd);
        return 1;
    }

    std::cout << "Mensagem enviada com sucesso!" << std::endl;

    // Recebendo a resposta do servidor
    char buffer[1024];
    socklen_t len = sizeof(servaddr);
    ssize_t receivedBytes = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                                     (sockaddr*)&servaddr, &len);
    if (receivedBytes == -1) {
        std::cerr << "Erro ao receber a resposta" << std::endl;
        close(sockfd);
        return 1;
    }

    std::cout << "Resposta recebida: " << std::string(buffer, receivedBytes) << std::endl;

    close(sockfd);
    return 0;
}

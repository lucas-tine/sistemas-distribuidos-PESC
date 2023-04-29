#include <iostream>
#include <unistd.h>
#include <random>
#include <ctime>
#include <sys/socket.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define PORT 4321
#define TAMANHO_MAX 20
#define NUMEROS_A_GERAR 100

using namespace std;
const char* UTILIZACAO = "uso: ./sockets.produtor <IP> <PORTA> <Número_de_iterações>";

int erro(const char* aviso = UTILIZACAO)
{
    cout << aviso << endl;
    return 1;
}

int
main(int argc, char *argv[])
{
    if (argc != 4)
        return erro();

    int sock;
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &address.sin_addr);
    
    char numero[TAMANHO_MAX];
    char resultado[TAMANHO_MAX];

    // Criação do socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Error creating socket" << endl;
        return EXIT_FAILURE;
    }

    // Início da conexão
    if (connect(sock, (struct sockaddr*)&address, sizeof(address)) < 0) {
        cout << "Error connecting" << endl;
        return EXIT_FAILURE;
    }

    std::mt19937 rng(std::time(nullptr)); // Objeto gerador de números aleatórios
    std::uniform_int_distribution<int> incremento_aleatorio(1, 100); // Distribuição aleatória
    unsigned int numero_produzido = 1;

    for (int i = atoi(argv[3]); i > 0; i--){
        snprintf(numero, TAMANHO_MAX, "%i", numero_produzido);
        send(sock, numero, TAMANHO_MAX, 0);
        cout << "número " << numero << " enviado" << endl;
        numero_produzido += incremento_aleatorio(rng); // Adiciona um número aleatório

        // Recebe o resultado do consumidor
        recv(sock, resultado, TAMANHO_MAX, 0);
        cout << "Resposta do consumidor:" << resultado << endl;
    }
    
    send(sock, "0", sizeof(unsigned), 0); // sinalizando termino dos envio
    cout << "Resposta do consumidor:" << resultado << endl;
    close(sock);

    return 0;
}
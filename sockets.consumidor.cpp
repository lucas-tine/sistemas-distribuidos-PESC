#include <iostream>
#include <unistd.h>
#include <random>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 4321
#define TAMANHO_MAX 20

using namespace std;
const char* UTILIZACAO = "uso: ./sockets.consumidor <PORTA>";

int erro(const char* aviso = UTILIZACAO)
{
    cout << aviso << endl;
    return 1;
}

int
main(int argc, char *argv[])
{
    if (argc != 2){
        return erro();
    }

    int sock, prod_sock;

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[1]));
    int address_len = sizeof(address);
    
    char numero[TAMANHO_MAX];
    char resultado[TAMANHO_MAX];
    unsigned contador_de_primos = 0;

    // Criação do socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Error creating socket" << endl;
        return EXIT_FAILURE;
    }

    if (bind(sock, (struct sockaddr*)&address, sizeof(address)) < 0) {
        cout << "Error binding" << endl;
        return EXIT_FAILURE;
    }

    // Começa a escutar por conexões de socket
    if (listen(sock, 0) < 0) {
        cout << "Error connecting" << endl;
        return EXIT_FAILURE;
    }

    cout << "Socket escutando na porta " << argv[1] << endl;

    // Aceita a conexão com o produtor
    if ((prod_sock = accept(sock, (struct sockaddr*)&address, (socklen_t*)&address_len)) < 0) {
        cout << "Error accepting connection" << endl;
        return EXIT_FAILURE;
    }

    unsigned numero_recebido = -1; // valor inicial a ser sobrescrito

    while(numero_recebido != 0){
        
        bool eh_primo = true; 
        recv(prod_sock, numero, TAMANHO_MAX, 0);

        numero_recebido = atoi(numero);
        for (unsigned possivel_divisor = 2; possivel_divisor < numero_recebido/2; possivel_divisor++)
            if (numero_recebido % possivel_divisor == 0)
            {
                eh_primo = false;
                break;
            }
        
        if (eh_primo) contador_de_primos++;
        if (numero_recebido == 0) 
        {   
            cout << "um total de " << contador_de_primos << " primos gerados!" << endl;
        }
        cout << numero_recebido << ((eh_primo) ? " eh PRIMO !\n" : " nao ... \n");
        snprintf(resultado, TAMANHO_MAX, ((eh_primo) ? "%i eh PRIMO !\n" : "%i nao ... \n"), numero_recebido);
        send(prod_sock, resultado, strlen(resultado), 0);
    }

    close(sock);
    return EXIT_SUCCESS;
}
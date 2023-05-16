#include <iostream>
#include <unistd.h>
#include <random>
#include <ctime>

using namespace std;
unsigned numeros_a_gerar;

static void consumidor(int* pipe)
{
    close(pipe[1]); // fechando a escrita
    unsigned contador_de_primos = -1;
    const int descritor_pipe = pipe[0];
    unsigned numero_recebido = -1; // valor inicial a ser sobrescrito

    while(numero_recebido != 0){
        
        bool eh_primo = true; 
        read(descritor_pipe, &numero_recebido, sizeof(unsigned));

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
            return;
        }
        cout << numero_recebido << ((eh_primo) ? " eh PRIMO !\n" : " nao ... \n");
    }   
}


static void produtor(int* pipe)
{
    close(pipe[0]); // fechando a leitura
    const int descritor_pipe = pipe[1];
    

    std::mt19937 rng(std::time(nullptr)); // Objeto gerador de números aleatórios
    std::uniform_int_distribution<int> incremento_aleatorio(1, 100); // Distribuição aleatória
    unsigned int numero_produzido = 1;

    for (int i = numeros_a_gerar; i > 0; i--){
        write(descritor_pipe, &numero_produzido, sizeof(unsigned)); 
        numero_produzido += incremento_aleatorio(rng); // Adiciona um número aleatório 
    }
    
    numero_produzido = 0; // sinal de termino
    write(descritor_pipe, &numero_produzido, sizeof(unsigned)); // sinalizando termino dos envio
}

int erro(const char* aviso = "uso: ./pipe.primos <numeros-a-gerar>")
{
    cout << aviso << endl;
    return 1;
}

int
main(int argc, char** argv)
{
    int descritores_pipe [2];

    if (argc != 2) return erro();
    numeros_a_gerar = atoi(argv[1]);
    
    if (pipe(descritores_pipe) == -1) { // criação do pipe
        cout << "Error creating pipe" << endl;
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    bool falha_fork = (pid == -1), processo_filho = (pid == 0);

    if (falha_fork) { // criação do processo filho 
        cout << "Erro no fork!" << endl;
        return EXIT_FAILURE;
    } 
     
    if (processo_filho) consumidor(descritores_pipe);
    else produtor(descritores_pipe);

    return EXIT_SUCCESS;
}
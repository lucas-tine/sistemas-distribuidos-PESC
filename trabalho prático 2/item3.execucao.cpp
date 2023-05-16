#include <iostream>
#include <thread>
#include <semaphore.h>
#include "produtor.hpp"
#include "consumidor.hpp"

using namespace std;

int 
uso(char** argv)
{
    cout << "uso: " << argv[0] << " <num-threads-produtores> <num-threads-consumidores> <numeros-processados> <tamanho-buffer>" << endl;
    return EXIT_FAILURE;
}

int
main(int argc, char** argv)
{
    if (argc != 5) return uso(argv);

    unsigned long 
        numero_produtores = strtoul(argv[1], nullptr, 10), 
        numero_consumidores = strtoul(argv[2], nullptr, 10),
        inteiros_a_processar = strtoul(argv[3], nullptr, 10),
        tamanho_buffer = strtoul(argv[4], nullptr, 10); 

    if (numero_produtores == 0 || numero_consumidores == 0){
        cout << "erro: não podem haver menos de 1 produtor e 1 consumidor" << endl;
        return EXIT_FAILURE;
    }

    // inicializar memória compartilhada
    unsigned int buffer_compartilhado [tamanho_buffer];
    long long ultima_posicao = -1;

    // inicializar semáforos
    sem_t semaforo_inteiros, 
            semaforo_lacunas, 
            semaforo_acesso_buffer, 
            semaforo_numeros_a_produzir,
            semaforo_cout;

    sem_init(&semaforo_inteiros, 0, 0);
    sem_init(&semaforo_lacunas, 0, tamanho_buffer);
    sem_init(&semaforo_acesso_buffer, 0, 1);
    sem_init(&semaforo_numeros_a_produzir, 0, 1);
    sem_init(&semaforo_cout, 0, 1);

    sem_t  *semaforos [5] = {
        &semaforo_inteiros, 
        &semaforo_lacunas, 
        &semaforo_acesso_buffer, 
        &semaforo_numeros_a_produzir,
        &semaforo_cout
    };

    // inicializar threads de forma alternada
    thread consumidores[numero_consumidores];
    thread produtores[numero_produtores];
    const auto NUMERO_PRODUTORES_ORIGINAL = numero_produtores,
        NUMERO_CONSUMIDORES_ORIGINAL = numero_consumidores;

    while (numero_produtores > 0 || numero_consumidores > 0){
        if (numero_produtores > 0){
            numero_produtores--;
            produtores[numero_produtores] = thread (
                produtor, 
                &inteiros_a_processar,
                (unsigned int*) buffer_compartilhado,
                (sem_t**) semaforos,
                &ultima_posicao
            );
        }
        if (numero_consumidores > 0){
            numero_consumidores--;
            consumidores[numero_consumidores] = thread (
                consumidor, 
                &inteiros_a_processar,
                (unsigned int*) buffer_compartilhado, 
                (sem_t**) semaforos,
                &ultima_posicao
            );
        }
    }

    for (unsigned long indice_produtor = 0; indice_produtor < NUMERO_PRODUTORES_ORIGINAL; indice_produtor++)
        produtores[indice_produtor].join();

    while (ultima_posicao >= 0);
    for (unsigned long i = 0; i < NUMERO_CONSUMIDORES_ORIGINAL; i++)
        sem_post(&semaforo_inteiros);

    for (unsigned long i = 0; i < NUMERO_CONSUMIDORES_ORIGINAL; i++)
        consumidores[i].join();
    
    exit(EXIT_SUCCESS);
}
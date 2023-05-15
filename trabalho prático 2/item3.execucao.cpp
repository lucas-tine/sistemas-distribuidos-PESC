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

    // inicializar semáforos
    sem_t semaforo_inteiros, semaforo_lacunas;

    // inicializar threads de forma alternada
    thread consumidores[numero_consumidores];
    const auto NUMERO_CONSUMIDORES_ORIGINAL = numero_consumidores;

    while (numero_produtores > 0 || numero_consumidores > 0)
        if (numero_produtores > 0){
            numero_produtores--;
            thread novo_produtor(produtor, (unsigned int*) buffer_compartilhado);
        }
        if (numero_consumidores > 0){
            numero_consumidores--;
            thread novo_consumidor(consumidor, (unsigned int*) buffer_compartilhado, &inteiros_a_processar);
            consumidores[numero_consumidores] = move(novo_consumidor);
        }

    for (int indice_consumidor = 0; indice_consumidor < NUMERO_CONSUMIDORES_ORIGINAL; indice_consumidor++)
        consumidores[indice_consumidor].join();
    
    return EXIT_SUCCESS;
}
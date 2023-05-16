#include <random>
#include <ctime>
#include <semaphore.h>
#include <iostream> // remover

void 
produtor (unsigned long* numeros_a_produzir, unsigned int* buffer_compartilhado, sem_t  **semaforos)
{
    sem_t   &semaforo_inteiros      = *(semaforos[0]), 
            &semaforo_lacunas       = *(semaforos[1]), 
            &semaforo_acesso_buffer = *(semaforos[2]),
            &semaforo_modificacao_inteiros = *(semaforos[3]),
            &semaforo_numeros_a_produzir = *(semaforos[4]),
            &semaforo_cout = *(semaforos[5]);

    std::mt19937 rng(std::time(nullptr)); // gerador de números aleatórios
    std::uniform_int_distribution<unsigned> numero_aleatorio(1, 10000000); // Distribuição aleatória
    bool producao_em_andamento = true;

    while(producao_em_andamento)
    {
        sem_wait(&semaforo_numeros_a_produzir);

            if ( (*numeros_a_produzir) == 0 )
                    producao_em_andamento = false;
            else
                --(*numeros_a_produzir);

            sem_wait(&semaforo_cout);
            std::cout << "a produzir ainda " << *numeros_a_produzir << std::endl;
            sem_post(&semaforo_cout);
            
        sem_post(&semaforo_numeros_a_produzir);

        if (!producao_em_andamento) return;

        sem_wait(&semaforo_lacunas);

        sem_wait(&semaforo_modificacao_inteiros);
            sem_wait(&semaforo_acesso_buffer);
            
                int indice_buffer_atual;
                sem_getvalue(&semaforo_inteiros, &indice_buffer_atual);
                buffer_compartilhado[indice_buffer_atual] = numero_aleatorio(rng); // adiciona um número aleatório
                    sem_wait(&semaforo_cout);
                    std::cout << "produzi " << buffer_compartilhado[indice_buffer_atual] << " na posicao " << indice_buffer_atual << " do buffer" << std::endl;
                    sem_post(&semaforo_cout);
                sem_post(&semaforo_inteiros); 
            sem_post(&semaforo_acesso_buffer);
        sem_post(&semaforo_modificacao_inteiros);
        
    }
}
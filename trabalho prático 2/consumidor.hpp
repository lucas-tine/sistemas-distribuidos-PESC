#include<iostream>

void
consumidor (unsigned long* numeros_a_produzir, unsigned int* buffer_compartilhado, sem_t  **semaforos, long long* ultima_posicao)
{
    sem_t   &semaforo_inteiros      = *(semaforos[0]), 
            &semaforo_lacunas       = *(semaforos[1]), 
            &semaforo_acesso_buffer = *(semaforos[2]),
            &semaforo_numeros_a_produzir = *(semaforos[3]),
            &semaforo_cout = *(semaforos[4]);

    while (true)
    {
        // verificar final da producao
        sem_wait(&semaforo_numeros_a_produzir);
            bool end = (*numeros_a_produzir == 0 && (*ultima_posicao) < 0);
        sem_post(&semaforo_numeros_a_produzir);

        if (end) {
            sem_wait(&semaforo_cout);
            std::cout << "encerrando consumidor .." << std::endl;
            sem_post(&semaforo_cout);
            return;
        }

        sem_wait(&semaforo_cout);
            std::cout << "seguindo com consumidor ... " << std::endl
            << " NaP: " << *numeros_a_produzir << std::endl 
            << " UP: " << *ultima_posicao << std::endl;
        sem_post(&semaforo_cout);

        sem_wait(&semaforo_inteiros);
        // obter indice para o buffer
        if (*ultima_posicao < 0) return;
            
        sem_wait(&semaforo_acesso_buffer); 
            int indice_buffer_atual = (*ultima_posicao)--; // obter inteiro
            unsigned numero_recebido = buffer_compartilhado[indice_buffer_atual];
            buffer_compartilhado[indice_buffer_atual] = 0; // eliminar
            sem_wait(&semaforo_cout);
                std::cout << "indice de leitura recebido: " << indice_buffer_atual << " (" << numero_recebido << ")" << "\n";
                std::cout << "buffer: " ; for (long long i = 0; i <= *ultima_posicao; i++) std::cout << buffer_compartilhado[i] <<  " "; std::cout << std::endl;
            sem_post(&semaforo_cout);
            sem_post(&semaforo_lacunas);
        sem_post(&semaforo_acesso_buffer);

        bool eh_primo = true; 
        for (unsigned possivel_divisor = 2; possivel_divisor < numero_recebido/2; possivel_divisor++)
            if (numero_recebido % possivel_divisor == 0)
            {
                eh_primo = false;
                break;
            }
        sem_wait(&semaforo_cout);
        std::cout << numero_recebido << ((eh_primo) ? " eh PRIMO!" : " nao ...") << ")\n";
        sem_post(&semaforo_cout);
    }
}
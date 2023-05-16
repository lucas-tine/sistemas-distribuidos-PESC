#include<iostream>

void
consumidor (unsigned long* numeros_a_produzir, unsigned int* buffer_compartilhado, sem_t  **semaforos )
{
    sem_t   &semaforo_inteiros      = *(semaforos[0]), 
            &semaforo_lacunas       = *(semaforos[1]), 
            &semaforo_acesso_buffer = *(semaforos[2]),
            &semaforo_modificacao_inteiros = *(semaforos[3]),
            &semaforo_numeros_a_produzir = *(semaforos[4]),
            &semaforo_cout = *(semaforos[5]);

    sem_wait(&semaforo_cout);
    std::cout << "iniciando consumidor " << std::this_thread::get_id() << std::endl;
    sem_post(&semaforo_cout);

    while (true)
    {
        // verificar final da producao
        bool end = false;
        sem_wait(&semaforo_numeros_a_produzir);
            if (*numeros_a_produzir == 0){ 
                sem_wait(&semaforo_acesso_buffer); 
                    if (buffer_compartilhado[0] == 0) 
                        end = true;
                sem_post(&semaforo_acesso_buffer); 
            }
        sem_post(&semaforo_numeros_a_produzir);
        if (end) return;

        // obter indice para o buffer
        sem_wait(&semaforo_modificacao_inteiros);
            sem_wait(&semaforo_inteiros);
            int indice_buffer_atual;
            sem_getvalue(&semaforo_inteiros, &indice_buffer_atual);

            // obter inteiro
            sem_wait(&semaforo_acesso_buffer); 
                unsigned numero_recebido = buffer_compartilhado[indice_buffer_atual];
                buffer_compartilhado[indice_buffer_atual] = 0; // eliminar
                sem_post(&semaforo_lacunas);
            sem_post(&semaforo_acesso_buffer);

            sem_wait(&semaforo_cout);
                std::cout << "indice de leitura recebido: " << indice_buffer_atual << " (" << numero_recebido << ")" << "\n";
            sem_post(&semaforo_cout);
        sem_post(&semaforo_modificacao_inteiros);

        

        bool eh_primo = true; 

        for (unsigned possivel_divisor = 2; possivel_divisor < numero_recebido/2; possivel_divisor++)
            if (numero_recebido % possivel_divisor == 0)
            {
                eh_primo = false;
                break;
            }
        sem_wait(&semaforo_cout);
        std::cout << numero_recebido << ((eh_primo) ? " eh PRIMO!" : " nao ...") << " (pos " << indice_buffer_atual << ")\n";
        sem_post(&semaforo_cout);
    }
}
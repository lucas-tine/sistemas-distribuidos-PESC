#include <iostream>
#include <string>
#include <thread>
#include <cstdlib>
#include <deque>
#include <chrono>
#include "socket_udp.hpp"
#include "coordenador.hpp"

using namespace std;
int main (int argc, char **argv) {
    // Marca o tempo de início
    auto inicio = std::chrono::steady_clock::now();

    if (argc != 4){
        cout << "forneca n (numero de processo), r (numero de escritas) e k (tempo de acesso) na linha de comando" << endl;
        return EXIT_FAILURE;
    }

    unsigned n = std::stoi(argv[1]), r = std::stoi(argv[2]), k = std::stoi(argv[3]);
    deque<thread*> processos;

    for (;n > 0; n--){ // iniciando processos
        std::ostringstream comando;
        comando << "./processo " << n << " " << r << " " << k << " "; 
        std::string comando_str = comando.str();
        processos.push_back(
            new thread([comando_str](){return system(comando_str.c_str());})
        );
    }

    cout << "processos inicializados, certifique-se de que o coordenador já esta operante" << endl;

    for (thread *processo: processos)
        processo->join();

    auto fim = std::chrono::steady_clock::now();
    auto duracao = std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio).count();
    std::cout << "Tempo de execução: " << duracao << " milissegundos" << std::endl;

    return 0;
}
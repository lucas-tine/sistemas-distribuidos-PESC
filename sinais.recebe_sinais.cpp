#include <iostream>
#include <signal.h>
#include <unistd.h>

using namespace std;

const char* UTILIZACAO = "uso: ./recebe_sinais";

int erro(const char* aviso = UTILIZACAO)
{
    cout << aviso << endl;
    return 1;
}

static void sinal_encerramento(int signum)
{
    cout << "sinal para encerrar recebido (" << signum << ")" << endl;
    exit(EXIT_SUCCESS);
}

static void sinal_customizado(int signum)
{
    cout << "Sinal customizado numero " << signum - 34 <<" ! (sinal " << signum << ")" << endl;
}

int
main(int argc, char** argv)
{
    if (argc != 1) return erro();

    signal(34, sinal_customizado);
    signal(35, sinal_customizado);
    signal(36, sinal_customizado);
    signal(37, sinal_encerramento);
    cout << "processo rodando como " << getpid() << endl;
    
    while(true)
        pause();
}
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

static void perda_de_controle(int signum)
{
    cout << "[SIGHUP: perda de controle do processo] recebido" << endl; 
}

static void sinal_customizado(int signum)
{
    cout << "Sinal customizado numero " << signum - 34 <<" ! (sinal " << signum << ")" << endl;
}

int
main(int argc, char** argv)
{
    if (argc != 1) return erro();

    cout << "processo rodando como " << getpid() << endl;
    signal(1, perda_de_controle);

    signal(34, sinal_customizado);
    signal(35, sinal_customizado);
    signal(36, sinal_customizado);
    while(true)
        pause();
}
#include <iostream>
#include <signal.h>
#include <unistd.h>

using namespace std;

const char* UTILIZACAO = "uso: ./recebe_sinais <use-busy-wait-0/1>";
volatile sig_atomic_t sinal_recebido[4] = {0,0,0,0};

bool busy_wait = false;

int erro(const char* aviso = UTILIZACAO)
{
    cout << aviso << endl;
    return 1;
}

static void sinal_encerramento(int signum)
{
    if (busy_wait){
        sinal_recebido[3] = 1;
        return;
    }
    cout << "sinal para encerrar recebido (" << signum << ")" << endl;
    exit(EXIT_SUCCESS);
}

static void sinal_customizado(int signum)
{
    const unsigned sinal_interno = signum - 34;
    if (busy_wait){
        sinal_recebido[sinal_interno] = 1;
        return;
    }
    
    cout << "Sinal customizado numero " << sinal_interno <<" ! (sinal " << signum << ")" << endl;
}

int
main(int argc, char** argv)
{
    if (argc != 2) return erro();
    busy_wait = (bool) atoi(argv[1]);

    signal(34, sinal_customizado);
    signal(35, sinal_customizado);
    signal(36, sinal_customizado);
    signal(37, sinal_encerramento);

    cout << "processo rodando como " << getpid() << (busy_wait ? " (busy wait) " : " (blocking wait) ") << endl;
    
    while (busy_wait) // nao muda, loop infinito ate o sinal de encerrar ser recebido
    {
        for (int i = 0; i < 4; i++)
            if (i == 3 && sinal_recebido[3]){
                    cout << "sinal para encerrar recebido (" << 37 << ")" << endl;
                    exit(EXIT_SUCCESS);
                }
            else if (sinal_recebido[i]){
                cout << "Sinal customizado numero " << i <<
                    " ! (sinal " << i+34 << ")" << endl;
                sinal_recebido[i] = 0;
                
            }
    }
    
    while(true)
        pause();
}
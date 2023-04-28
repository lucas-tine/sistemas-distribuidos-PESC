#include <iostream>
#include <signal.h>

using namespace std;

const char* UTILIZACAO = "uso: ./sinalizador <pid> <sinal>";

bool processo_existe(pid_t pid) {
    return kill(pid, 0) == 0;
}

int erro(const char* aviso = UTILIZACAO)
{
    cout << aviso << endl;
    return 1;
}

int
main(int argc, char** argv)
{
    if (argc != 3) return erro();

    pid_t processo = atoi(argv[1]);
    int sinal = atoi(argv[2]);

    if (!processo_existe(processo)) return erro("O processo nao existe!");

    int retorno = kill(processo, sinal);
    if (retorno == -1) return erro("falha: sinal nao enviado, erro de sistema");

    cout << "sinal enviado com sucesso" << endl;
    return 0;
}
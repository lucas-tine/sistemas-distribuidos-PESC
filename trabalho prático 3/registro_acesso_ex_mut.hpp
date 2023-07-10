#include <ctime>
#include <netinet/in.h>

typedef struct 
{
    time_t tempo_requisicao = 0, tempo_acesso = 0, tempo_liberacao = 0;
    unsigned id_processo;
    sockaddr_in endereco_processo;
} 
registro_acesso_ex_mut;
#include <ctime>
#include <netinet/in.h>

typedef struct 
{
    time_t momento_mensagem;
    unsigned id_processo_origem, id_processo_destino;
    char tipo_mensagem;
} 
registro_mensagem_ex_mut;
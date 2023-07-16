#include <string>
#include <cstdarg>
#include <stdexcept>
#include <tuple>

// classe abstrata de um objeto que se comunica no sistema de exclusão mutua
class mensageiro_ex_mut
{
    private:
        // contagem de mensagens enviadas
        unsigned long contador_mensagens = 0;

    public:
        static const int TAMANHO_MENSAGEM;
        static const char request, wait, grant, release, ok;

        virtual unsigned id() = 0;

        // constroi uma mensagem, assinada com id e contagem, do tipo do char passado
        std::string construir_mensagem(const char);

        // divide uma dada mensagem em seus campos: tipo, id e contagem do remetente, retornando em tupla
        std::tuple<char, unsigned, unsigned> dividir_mensagem(std::string);
};

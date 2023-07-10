#include <string>
#include <cstdarg>
#include <stdexcept>

class mensageiro_ex_mut
{
    private:
        unsigned long contador_mensagens = 0;

    public:
        static const int TAMANHO_MENSAGEM;
        static const char request, wait, grant, release, ok;

        virtual unsigned id() = 0;
        std::string construir_mensagem(const char);
};

#include <iostream>
#include "socket_udp.hpp"
#include "mensageiro_ex_mut.hpp"

using namespace std;

SocketUDP& socket_coordenador_main_thread()
{
    SocketUDP& socket_coordenador = *new SocketUDP (1024);
    mensagem_udp msg;
    socket_coordenador.enderecar_a_si();
    return socket_coordenador;
}

class processo: public mensageiro_ex_mut
{
    unsigned _id;
    public:
        processo(int id) {
            this->_id = id;
        };

        unsigned id() override {
            return _id;
        };
    
};

int
main (int argc, char** argv)
{
    if (argc != 2){
        cout << "forneca apenas ID de exclusão mútua do processo por linha de comando" << endl;
        return EXIT_FAILURE;
    }

    int id = stoi(argv[1]);
    int opcao = -1;
    bool funcionando = true;
    processo _processo(id); 
    SocketUDP socket;

    while (funcionando){
        mensagem_udp msg;
        cout << "Opcoes: " << endl <<
        "(1) enviar request" << endl <<
        "(2) enviar release" << endl << 
        "(3) encerrar execucao do processo" << endl << endl <<
        "> " ;
        cin >> opcao;
        cout << "selecionando " << opcao << endl;

        switch (opcao) 
        {
            case 1:
                socket = socket_coordenador_main_thread();
                socket.enviar_mensagem(_processo.construir_mensagem(_processo.request));
                do
                {
                    msg = socket.receber_mensagem_e_enderecar(); // aponta o socket para quem primeiro enviar (outra thread, através de outro socket ...)
                    cout << "mensagem recebida: " << msg.mensagem << endl;
                } 
                while (msg.mensagem[0] != processo::grant);
                break;
            case 2:
                socket.enviar_mensagem("r|1|0000000");
                msg = socket.receber_mensagem();
                cout << "mensagem recebida: " << msg.mensagem  << endl;
                socket = socket_coordenador_main_thread(); // thread de atendimento fechou, o socket deve tratar novamente com main thread do coordenador
                break;
            case 3: 
                funcionando = false;
                break;
            default: 
                std::cout << endl << "opcao invalida!" << endl << endl; 
        }
    }
}
#include <iostream>
#include "socket_udp.hpp"

using namespace std;
int
main ()
{
    SocketUDP socket_coordenador(1024);
    mensagem_udp msg;
    socket_coordenador.enderecar_a_si();
    int opcao = -1;
    bool funcionando = true;

    while (funcionando){
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
                socket_coordenador.enviar_mensagem("p|1|0000000");
                do
                {
                    msg = socket_coordenador.receber_mensagem_e_enderecar();
                    cout << "mensagem recebida: " << msg.mensagem << endl;
                } 
                while (msg.mensagem[0] != 'g');
                break;
            case 2:
                socket_coordenador.enviar_mensagem("r|1|0000000");
                msg = socket_coordenador.receber_mensagem();
                cout << "mensagem recebida: " << msg.mensagem  << endl;
                break;
            case 3: 
                funcionando = false;
                break;
            default: 
                std::cout << endl << "opcao invalida!" << endl << endl; 
        }
    }
}
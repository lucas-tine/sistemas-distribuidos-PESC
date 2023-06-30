#include "socket_udp.hpp"
#include <thread>
#include <iostream>

using namespace std;

const unsigned long MAX_TIME_WAIT = 3000;

class Coordenador{
    private:
        SocketUDP* socket_servidor;
        thread* servico_terminal;
        thread* servico_respostas_udp;
        bool funcionando;

    public:

    Coordenador(int porta) 
    {
        this->funcionando = true;
        this->socket_servidor = new SocketUDP(porta);
        this->socket_servidor->iniciar_servidor();

        this->servico_terminal = new thread(&Coordenador::atender_terminal, this);
        this->servico_respostas_udp = new thread
        (
            &SocketUDP::servir_enquanto, 
            this->socket_servidor, 
            &(this->funcionando), 
            MAX_TIME_WAIT,
            [](string requisicao){
                return string("jae");
            }     
        );
    }

    void aguardar()
    {
        this->servico_terminal->join();
    }

    void atender_terminal()
    {
        int opcao = -1;

        while (1){
            cout << "Opcoes: " << endl <<
            "(1) ver fila de pedidos atual" << endl <<
            "(2) ver atendimentos feitos a cada processo" << endl << 
            "(3) encerrar execucao do coordenador" << endl << endl <<

            "> " ;

            cin >> opcao;
            cout << "selecionando " << opcao << endl;
        }

        this->funcionando = false;
        this->servico_respostas_udp->join();
    }

    void atender_requisicoes()
    {
        
    }
};
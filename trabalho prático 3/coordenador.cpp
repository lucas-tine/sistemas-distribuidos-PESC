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
        this->servico_respostas_udp = new thread(&Coordenador::atender_requisicoes, this);
    }

    void aguardar()
    {
        this->servico_terminal->join();
    }

    void atender_terminal()
    {
        int opcao = -1;

        while (this->funcionando){
            cout << "Opcoes: " << endl <<
            "(1) ver fila de pedidos atual" << endl <<
            "(2) ver atendimentos feitos a cada processo" << endl << 
            "(3) encerrar execucao do coordenador" << endl << endl <<

            "> " ;

            cin >> opcao;
            cout << "selecionando " << opcao << endl;
            switch (opcao) 
            {
                case 3:
                    this->funcionando = false;
                    break;
                default: 
                    std::cout << endl << "opcao invalida!" << endl << endl; 
            }
        }

        this->servico_respostas_udp->join();
    }

    void atender_requisicoes()
    {
        this->socket_servidor->configurar_timeout(500);
        int status;
        bool requisicao;

        while (this->funcionando){
            status = this->socket_servidor->aguardar_mensagem_timeout();
            requisicao = (status > 0);
            if (!requisicao) 
                continue;


            mensagem_udp mensagem = this->socket_servidor->receber_mensagem();
            thread(&Coordenador::tratar_requisicao, this, mensagem).detach();
        }
    }

    void tratar_requisicao(mensagem_udp requisicao)
    {

    }
};
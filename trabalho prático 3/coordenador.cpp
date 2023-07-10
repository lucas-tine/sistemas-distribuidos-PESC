#include "socket_udp.hpp"
#include "mensageiro_ex_mut.hpp"
#include "registro_acesso_ex_mut.hpp"
#include <thread>
#include <iostream>
#include <deque>
#include <mutex>

using namespace std;

const unsigned long MAX_TIME_WAIT = 3000;

class Coordenador: public mensageiro_ex_mut
{
    private:
        SocketUDP* socket_servidor;
        thread* servico_terminal;
        thread* servico_respostas_udp;
        int porta;
        bool funcionando;
        unsigned long contador_mensagens = 0;
        
        deque<sockaddr_in> fila_acessos;
        deque<registro_acesso_ex_mut*> historico_acessos; // AINDA NAO UTILIZADO

        mutex lock_fila_acessos;

    public:

    Coordenador(int porta) 
    {
        this->funcionando = true;
        this->porta = porta;
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
                case 1:
                    this->imprimir_fila_atual();
                    break;
                case 3:
                    this->funcionando = false;
                    break;
                default: 
                    std::cout << endl << "opcao invalida!" << endl << endl; 
            }
        }

        this->servico_respostas_udp->join();
    }

    void imprimir_fila_atual()
    {
        this->lock_fila_acessos.lock();
        {
            int lugar_na_fila = 1;
            if (this->fila_acessos.empty())
                cout << " [fila vazia] " << endl << endl;
            else for (sockaddr_in addr: this->fila_acessos)
                cout << lugar_na_fila++ << ") " << addr.sin_port << ": " << addr.sin_addr.s_addr << endl;
            
        } 
        this->lock_fila_acessos.unlock();
    }

    void atender_requisicoes()
    {
        this->socket_servidor = new SocketUDP(this->porta);
        this->socket_servidor->configurar_timeout(500);
        int status;
        bool requisicao;

        while (this->funcionando){
            status = this->socket_servidor->aguardar_mensagem_timeout();
            requisicao = (status > 0);
            if (!requisicao) {
                continue;
            }

            mensagem_udp mensagem = this->socket_servidor->receber_mensagem();
            thread(&Coordenador::tratar_requisicao, this, mensagem).detach();
        }
    }

    void tratar_requisicao(mensagem_udp requisicao)
    {
        // enviar WAIT, entrar na fila de execuções, e ao chegar na cabeça, enviar o GRANT.
        if (requisicao.mensagem[0] == mensageiro_ex_mut::request) 
        {
            bool primeiro_da_fila = false;
            this->lock_fila_acessos.lock();
            {
                if (this->fila_acessos.empty()) 
                    primeiro_da_fila = true;
                else {
                    SocketUDP socket(this->porta + 1);
                    socket.endereco_socket = requisicao.endereco;
                    cout << "wait enviado " << socket.enviar_mensagem (this->construir_mensagem(mensageiro_ex_mut::wait)) << endl;
                }   

                this->fila_acessos.push_back(requisicao.endereco);
            }
            this->lock_fila_acessos.unlock();
            if (primeiro_da_fila) 
                this->atender_fila();
        }
    }

    void atender_fila()
    {
        bool atendendo = true;
        while (atendendo)
        {
            this->lock_fila_acessos.lock();
            if (this->fila_acessos.empty()) {
                atendendo = false;
                this->lock_fila_acessos.unlock();
            }
            else 
            {
                int mensagens_recebidas = 0;
                sockaddr_in proximo = this->fila_acessos.front();
                this->lock_fila_acessos.unlock();

                SocketUDP socket(this->porta + 1);
                socket.endereco_socket = proximo;
                socket.enviar_mensagem (this->construir_mensagem(mensageiro_ex_mut::grant));

                socket.configurar_timeout(2000);
                while (mensagens_recebidas == 0 and atendendo){
                    mensagens_recebidas = socket.aguardar_mensagem_timeout();
                    if (not this->funcionando) atendendo = false;
                    cout << "nada de release... (" << mensagens_recebidas << ")" << endl;
                }
                if (mensagens_recebidas)
                {
                    if (socket.receber_mensagem().mensagem[0] == this->release)
                    {
                        cout << socket.enviar_mensagem(this->construir_mensagem(mensageiro_ex_mut::ok)) << endl;
                        this->lock_fila_acessos.lock();
                            this->fila_acessos.pop_front();
                        this->lock_fila_acessos.unlock();
                    }
                }
            }
            
        }
    }

    unsigned id() {return 0;} 
};
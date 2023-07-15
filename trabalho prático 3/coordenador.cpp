#include "socket_udp.hpp"
#include "mensageiro_ex_mut.hpp"
#include "registro_mensagem_ex_mut.hpp"
#include <thread>
#include <iostream>
#include <deque>
#include <mutex>
#include <tuple>
#include <map>

using namespace std;

struct endereco_processo {
    unsigned id_processo;
    sockaddr_in endereco;
};

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
        
        deque<endereco_processo> fila_acessos;
        deque<registro_mensagem_ex_mut> historico_mensagens; // AINDA NAO UTILIZADO

        mutex lock_fila_acessos;
        mutex lock_historico_mensagens;

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
                case 2:
                    map<unsigned, unsigned> grants, requests;
                    {
                        lock_guard<mutex> (this->lock_historico_mensagens);
                        for (registro_mensagem_ex_mut reg_msg: this->historico_mensagens)  
                            if (reg_msg.tipo_mensagem == this->grant){
                                if (grants.count(reg_msg.id_processo_destino) <= 0)
                                    grants[reg_msg.id_processo_destino] = 1;
                                else
                                    grants[reg_msg.id_processo_destino]++;
                            }
                            else if (reg_msg.tipo_mensagem == this->request){
                                if (requests.count(reg_msg.id_processo_origem) <= 0)
                                    requests[reg_msg.id_processo_origem] = 1;
                                else
                                    requests[reg_msg.id_processo_origem]++;

                            }
                    }
                    // FALTA PRINTAR AQUI OS ATENDIMENTOS CONTADOS
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
        lock_guard<mutex> lock(this->lock_fila_acessos);
        if (this->fila_acessos.empty())
            cout << " [fila vazia] " << endl << endl;
        else 
        {
            int lugar_na_fila = 1;
            for (endereco_processo addr: this->fila_acessos)
                cout << lugar_na_fila++ << ") " << " processo " << addr.id_processo 
                << ", " << addr.endereco.sin_addr.s_addr << ":" << addr.endereco.sin_port << endl;
        }
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
        this->registrar_recepcao(requisicao);
        auto componentes_requisicao = this->dividir_mensagem(requisicao.mensagem);
        // enviar WAIT, entrar na fila de execuções, e ao chegar na cabeça, enviar o GRANT.
        if (requisicao.mensagem[0] == mensageiro_ex_mut::request) 
        {
            bool primeiro_da_fila = false;
            lock_guard<mutex> lock(this->lock_fila_acessos);
            if (this->fila_acessos.empty()) 
                primeiro_da_fila = true;
            else {
                SocketUDP socket(this->porta + 1);
                socket.endereco_socket = requisicao.endereco;
                string wait = this->construir_mensagem(mensageiro_ex_mut::wait);
                cout << "wait enviado " << socket.enviar_mensagem (wait) << endl;
                registrar_envio(wait, get<1>(componentes_requisicao) );
            }   

            this->fila_acessos.push_back(
                endereco_processo {
                    get<1>(componentes_requisicao),
                    requisicao.endereco
                }
            );
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
                endereco_processo proximo = this->fila_acessos.front();
                this->lock_fila_acessos.unlock();

                SocketUDP socket(this->porta + 1);
                socket.endereco_socket = proximo.endereco;
                string grant = this->construir_mensagem(mensageiro_ex_mut::grant);
                socket.enviar_mensagem (grant);
                this->registrar_envio(grant, proximo.id_processo);

                socket.configurar_timeout(2000);

                int mensagens_recebidas = 0;
                while (mensagens_recebidas == 0 and atendendo)
                {
                    mensagens_recebidas = socket.aguardar_mensagem_timeout();
                    if (not this->funcionando) atendendo = false;
                    cout << "nada de release... (" << mensagens_recebidas << ")" << endl; // DEBUG
                }
                if (mensagens_recebidas)
                {
                    mensagem_udp mensagem_retorno = socket.receber_mensagem();
                    registrar_recepcao(mensagem_retorno);
                    if (mensagem_retorno.mensagem[0] == this->release)
                    {
                        lock_guard<mutex> lock(this->lock_fila_acessos);
                        string ok = this->construir_mensagem(mensageiro_ex_mut::ok);
                        cout << socket.enviar_mensagem(ok) << endl;
                        registrar_envio(ok, proximo.id_processo);
                        this->fila_acessos.pop_front();
                    }
                }
            }
            
        }
    }

    void registrar_recepcao(mensagem_udp requisicao)
    {
        lock_guard<mutex> lock(this->lock_historico_mensagens);
        auto campos_msg = this->dividir_mensagem(requisicao.mensagem);
        this->historico_mensagens.push_back
        (
            registro_mensagem_ex_mut
            {
                time(NULL),
                get<1>(campos_msg),
                0,
                get<0>(campos_msg)[0]
            }
        );
    }

    void registrar_envio(string msg, unsigned id_processo)
    {
        lock_guard<mutex> lock(this->lock_historico_mensagens);
        this->historico_mensagens.push_back(
            registro_mensagem_ex_mut {
                time(nullptr),
                0,
                id_processo, 
                msg[0]
            }
        ); 
    }

    unsigned id() {return 0;} 
};
#include "coordenador.hpp"

Coordenador::Coordenador(int porta) {
    this->funcionando = true;
    this->porta = porta;
    this->servico_terminal = new std::thread(&Coordenador::atender_terminal, this);
    this->servico_respostas_udp = new std::thread(&Coordenador::atender_requisicoes, this);
}

void Coordenador::aguardar() {
    this->servico_terminal->join();
}

void Coordenador::atender_terminal() {
    int opcao = -1;
    while (this->funcionando){
        std::cout << "Opcoes: " << std::endl <<
            "(1) ver fila de pedidos atual" << std::endl <<
            "(2) ver atendimentos feitos a cada processo" << std::endl << 
            "(3) encerrar execucao do coordenador" << std::endl << std::endl <<
            "> " ;
        std::cin >> opcao;
        std::cout << "selecionando " << opcao << std::endl;
        switch (opcao) 
        {
            case 1:
                this->imprimir_fila_atual();
                break;
            case 2:
                {
                    std::map<unsigned, unsigned> grants, requests;
                    {
                        std::lock_guard<std::mutex> lock(this->lock_historico_mensagens);
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
                    std::cout << "pedidos de acessos recebidos (REQUESTs):\n" 
                        << std::left << std::setw(8) << "id" 
                        << std::left << std::setw(14) << "contagem" << std::endl;
                    for (auto& pair: requests)
                        std::cout << std::left << std::setw(8) << pair.first 
                            << std::left << std::setw(14) << pair.second << '\n';

                    std::cout << "autorizacoes de acessos fornecidas (GRANTs):\n" 
                        << std::left << std::setw(8) << "id" 
                        << std::left << std::setw(14) << "contagem" << std::endl;
                    for (auto& pair: grants)
                        std::cout << std::left << std::setw(8) << pair.first 
                            << std::left << std::setw(14) << pair.second << '\n';
                    std::cout << std::endl;
                }
                break;
            case 3:
                this->funcionando = false;
                break;
            default: 
                std::cout << std::endl << "opcao invalida!" << std::endl << std::endl; 
        }
    }
    this->servico_respostas_udp->join();
}

void Coordenador::imprimir_fila_atual() {
    std::lock_guard<std::mutex> lock(this->lock_fila_acessos);
    if (this->fila_acessos.empty())
        std::cout << " [fila vazia] " << std::endl << std::endl;
    else 
    {
        int lugar_na_fila = 1;
        for (endereco_processo addr: this->fila_acessos)
            std::cout << lugar_na_fila++ << ") " << " processo " << addr.id_processo 
            << ", " << addr.endereco.sin_addr.s_addr << ":" << addr.endereco.sin_port << std::endl;
    }
}

void Coordenador::atender_requisicoes() {
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
        std::thread(&Coordenador::tratar_requisicao, this, mensagem).detach();
    }
}

void Coordenador::tratar_requisicao(mensagem_udp requisicao) {
    this->registrar_recepcao(requisicao);
    auto componentes_requisicao = this->dividir_mensagem(requisicao.mensagem);
    if (requisicao.mensagem[0] == mensageiro_ex_mut::request) 
    {
        bool primeiro_da_fila = false;
        std::lock_guard<std::mutex> lock(this->lock_fila_acessos);
        if (this->fila_acessos.empty()) 
            primeiro_da_fila = true;
        else {
            SocketUDP socket(this->porta + 1);
            socket.endereco_socket = requisicao.endereco;
            std::string wait = this->construir_mensagem(mensageiro_ex_mut::wait);
            socket.enviar_mensagem (wait);
            registrar_envio(wait, std::get<1>(componentes_requisicao));
        }   

        this->fila_acessos.push_back(
            endereco_processo {
                std::get<1>(componentes_requisicao),
                requisicao.endereco
            }
        );
        this->lock_fila_acessos.unlock();
        if (primeiro_da_fila) 
            this->atender_fila();
    }
}

void Coordenador::atender_fila() {
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
            std::string grant = this->construir_mensagem(mensageiro_ex_mut::grant);
            socket.enviar_mensagem (grant);
            this->registrar_envio(grant, proximo.id_processo);

            socket.configurar_timeout(2000);

            int mensagens_recebidas = 0;
            while (mensagens_recebidas == 0 and atendendo)
            {
                mensagens_recebidas = socket.aguardar_mensagem_timeout();
                if (!this->funcionando) atendendo = false;
            }
            if (mensagens_recebidas)
            {
                mensagem_udp mensagem_retorno = socket.receber_mensagem();
                registrar_recepcao(mensagem_retorno);
                if (mensagem_retorno.mensagem[0] == this->release)
                {
                    std::lock_guard<std::mutex> lock(this->lock_fila_acessos);
                    std::string ok = this->construir_mensagem(mensageiro_ex_mut::ok);
                    socket.enviar_mensagem(ok);
                    this->registrar_envio(ok, proximo.id_processo);
                    this->fila_acessos.pop_front();
                }
            }
        }
        
    }
}

void Coordenador::registrar_recepcao(mensagem_udp requisicao) {
    std::lock_guard<std::mutex> lock(this->lock_historico_mensagens);
    auto campos_msg = this->dividir_mensagem(requisicao.mensagem);
    this->historico_mensagens.push_back(
        registro_mensagem_ex_mut
        {
            time(nullptr),
            std::get<1>(campos_msg),
            0,
            std::get<0>(campos_msg)
        }
    );
}

void Coordenador::registrar_envio(std::string msg, unsigned id_processo) {
    std::lock_guard<std::mutex> lock(this->lock_historico_mensagens);
    this->historico_mensagens.push_back(
        registro_mensagem_ex_mut {
            time(nullptr),
            0,
            id_processo, 
            msg[0]
        }
    ); 
}

unsigned Coordenador::id() {
    return 0;
}
#ifndef SOCKETUDP_HPP
#define SOCKETUDP_HPP

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <stdexcept>
#include <deque>

/** Guarda uma mensagem transitada por um socket UDP, com
 *  seu conteúdo, o endereço do remetente e se não houve nenhum erro em sua recepção
 */
struct mensagem_udp {
    /** o conteudo da mensagem em si, encapsulado em std::string por conveniencia
     * (já que o padrao seria char[]) 
     */
    std::string mensagem;
    
    // o endereco do remetente da mensagem
    sockaddr_in endereco;

    // indica se nao houve nenhum erro conhecido na recepcao da mensagem
    bool sucesso = 1;
};

// encapsula as operações para abrir, utilizar e fechar um socket udp com mais facilidade
class SocketUDP {
private:
    struct timeval tv;
    fd_set readfds;
    size_t tamanho_buffer_msg;
    int descritor_socket = 0;
    bool timeout_configurado = false,
     comunicacao_anterior = false;
    
    bool iniciar_servidor();
    
public:
    sockaddr_in endereco_socket;
    

    SocketUDP(const int porta, const char *ipv4 = nullptr, size_t tamanho_buffer_msg = 16);
    ~SocketUDP();

    bool enviar_mensagem (std::string mensagem);

    // aponta o endereco do socket para a maquina local
    void enderecar_a_si (){
        this->endereco_socket.sin_addr.s_addr = inet_addr("127.0.0.1");
    }

    /**
     * espera por uma mensagem de forma bloqueante, ou recupera uma mensagem 
     * que ja foi recebida mas ainda não recuperada
     */
    mensagem_udp receber_mensagem();

    // chama "receber_mensagem" retornando se conteudo, e endereca o socket para o remetente 
    mensagem_udp receber_mensagem_e_enderecar();

    void configurar_timeout(unsigned long milissegundos);

    /**
     * aguarda uma mensagem ate receber, ou ate que venca o timeout configurado anteriormente
     * 
     * @return int - o numero de mensagens recebidos no periodo bloqueado, ou -1 em caso de erros
     */
    int aguardar_mensagem_timeout();
};

#endif

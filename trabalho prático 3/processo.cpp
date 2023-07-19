#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <string>
#include <tuple>
#include "socket_udp.hpp"
#include "mensageiro_ex_mut.hpp"

const unsigned 
TIMEOUT_COORDENADOR = 2000,
MAX_REPEAT_REQUEST = 3;

using namespace std;

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

string obter_hora_sistema();
SocketUDP& socket_coordenador_main_thread();
void esperar_mensagem_e_repetir (SocketUDP& socket, processo& _processo, char enviar, char esperar_receber); 

int
main (int argc, char** argv)
{
    if (argc != 4){
        cout << "forneca ID de exclusão mútua, r (numero de escritas) e k (tempo de acesso) na linha de comando" << endl;
        return EXIT_FAILURE;
    }

    unsigned id = stoi(argv[1]), r = stoi(argv[2]), k = stoi(argv[3]);
    processo _processo(id);  

    while (r-- > 0){
        SocketUDP socket = socket_coordenador_main_thread();
        socket.configurar_timeout(TIMEOUT_COORDENADOR);
        esperar_mensagem_e_repetir(socket, _processo, processo::request, processo::grant);
        // escrever em arquivo
        ofstream resultado("resultado.txt", ios::app);
        resultado << id << " - " << obter_hora_sistema() << endl;
        sleep(k);
        resultado.close();
        esperar_mensagem_e_repetir(socket, _processo, processo::release, processo::ok);
    }
}

string obter_hora_sistema() {
    auto tempo_atual = std::chrono::system_clock::now();
    std::time_t tempo = std::chrono::system_clock::to_time_t(tempo_atual);
    std::tm* tempo_local = std::localtime(&tempo);
    auto milissegundos = std::chrono::duration_cast<std::chrono::milliseconds>(
        tempo_atual.time_since_epoch()
    ).count() % 1000;

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tempo_local);
    std::string hora_completa = buffer + std::string(".") + std::to_string(milissegundos);

    return hora_completa;
}

SocketUDP& socket_coordenador_main_thread()
{
    SocketUDP& socket_coordenador = *new SocketUDP (1024);
    mensagem_udp msg;
    socket_coordenador.enderecar_a_si();
    return socket_coordenador;
}

void esperar_mensagem_e_repetir (SocketUDP& socket, processo& _processo, char enviar, char esperar_receber)
{
    mensagem_udp msg;
    int msg_recebida;
    tuple<char, unsigned, unsigned> campos_msg;
    unsigned repeticoes = 0;
    do
    {
        if (repeticoes++ >= MAX_REPEAT_REQUEST) {
            ofstream resultado("resultado.txt", ios::app);
            resultado << "ERRO DE COMUNICACAO NO PROCESSO " << _processo.id() << endl;
            resultado.close();
            exit(EXIT_FAILURE);
        }
        socket.enviar_mensagem(_processo.construir_mensagem(enviar));
        msg_recebida = socket.aguardar_mensagem_timeout();
        if (msg_recebida > 0){
            msg = socket.receber_mensagem(); // aponta o socket para quem primeiro enviar (outra thread, através de outro socket ...)
            campos_msg = _processo.dividir_mensagem(msg.mensagem);
        }
    } 
    while (msg_recebida < 1 || std::get<1>(campos_msg) != 0);

    while (std::get<0>(campos_msg) != esperar_receber){
        msg = socket.receber_mensagem();
        campos_msg = _processo.dividir_mensagem(msg.mensagem);
    }

    socket.endereco_socket = msg.endereco;
    return;
}
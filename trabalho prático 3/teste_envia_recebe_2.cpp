#include <iostream>
#include <string>
#include "socket_udp.hpp"

// ESTE TESTE FOI GERADO PELO CHAT GPT
using namespace std;
int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " MENSAGEM PORTA" << std::endl;
        return 1;
    }

    std::string mensagem = argv[1];
    int porta = std::stoi(argv[2]);
    SocketUDP socket(porta);
    socket.enderecar_a_si();
    socket.enviar_mensagem(mensagem);
    socket.aguardar_mensagem([](string resposta){
        cout << resposta << endl;
        return string("");
    },
    1000);
    
    return 0;
}

#include <iostream>
#include <string>
#include "socket_udp.hpp"

using namespace std;

void ha(SocketUDP& socket);

int main(int argc, char *argv[]) {
    SocketUDP socket(1024);
    socket.configurar_timeout(1500);
    do {
        cout << "nada..." << endl;
    }
    while (socket.aguardar_mensagem_timeout() <= 0);

    mensagem_udp msg = socket.receber_mensagem();
    cout << "recebido: " << msg.mensagem << endl;
    cout << "enviando resposta ..." ;

    for (char& ch : msg.mensagem) ch++;
    socket.endereco_socket = msg.endereco;
    cout << ' ' << socket.enviar_mensagem(msg.mensagem) << endl;

    return EXIT_SUCCESS;
}


#include <iostream>
#include <string>
#include "socket_udp.hpp"

using namespace std;

void ha(SocketUDP& socket);

int main(int argc, char *argv[]) {
    SocketUDP socket(1024);
    ha(socket);
    ha(socket);
    return EXIT_SUCCESS;
}

void ha(SocketUDP& socket)
{
    mensagem_udp msg = socket.receber_mensagem();
    cout << msg.mensagem << endl;

    string s = msg.mensagem;
    for (char& ch: s)
        ++ch;

    socket.endereco_socket = msg.endereco;
    socket.enviar_mensagem(s);

    sleep(2);
    for (char& ch: s)
        ++ch;
    socket.enviar_mensagem(s);
}

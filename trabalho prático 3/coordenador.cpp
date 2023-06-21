#include <iostream>
#include "socket_udp.hpp"

using namespace std;

int main () {
    SocketUDP socket(1024);
    socket.aguardar_mensagem(100);
    return 0;
}
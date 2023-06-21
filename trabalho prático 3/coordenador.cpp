#include <iostream>
#include "socket_udp.hpp"

using namespace std;

int main () {
    SocketUDP socket(1024);
    thread& servico = socket.aguardar_mensagens_multithreaded(
        100, 
        [](string s){return string("nada a ver");},
        false
    );
    servico.join();
    return 0;
}
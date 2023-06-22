#include <iostream>
#include "socket_udp.hpp"

using namespace std;

int main () {
    SocketUDP socket(1024);
    socket.iniciar_servidor();
    thread& servico = socket.aguardar_mensagens_multithreaded(
        100, 
        [](string request){return string("resposta");},
        false
    );
    servico.join();
    return 0;
}
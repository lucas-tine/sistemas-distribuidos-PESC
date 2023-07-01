#include <iostream>
#include <string>
#include "socket_udp.hpp"


using namespace std;
int main(int argc, char *argv[]) {
    SocketUDP socket(1024);
    socket.enderecar_a_si();
    cout << socket.enviar_mensagem("bora que bora!") << endl;
    cout << socket.receber_mensagem().mensagem << endl;
    cout << socket.receber_mensagem().mensagem << endl;
    return 0; 
}

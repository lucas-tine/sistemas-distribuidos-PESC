#include <iostream>
#include "socket_udp.hpp"
#include "coordenador.hpp"

using namespace std;

int main () {
    Coordenador coord(PORTA_PADRAO);
    coord.aguardar();
}
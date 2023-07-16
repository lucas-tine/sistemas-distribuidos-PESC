#include <iostream>
#include "socket_udp.hpp"
#include "coordenador.hpp"

using namespace std;

int main () {
    Coordenador coord(1024);
    coord.aguardar();
}
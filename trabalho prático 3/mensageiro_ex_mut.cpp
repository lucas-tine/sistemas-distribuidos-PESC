#include "mensageiro_ex_mut.hpp"
#include <iostream>
const int mensageiro_ex_mut::TAMANHO_MENSAGEM = 16;

const char mensageiro_ex_mut::request = 'p'; // "P"lease
const char mensageiro_ex_mut::grant   = 'g'; 
const char mensageiro_ex_mut::wait    = 'w';
const char mensageiro_ex_mut::ok      = 'o';
const char mensageiro_ex_mut::release = 'r'; 

std::string 
mensageiro_ex_mut::construir_mensagem(const char ch)
{
    std::string mensagem("");
    mensagem += ch;
    mensagem += "|" + std::to_string(this->id()) + "|" + std::to_string(this->contador_mensagens++);
    int tamanho_atual = mensagem.length();
    if (tamanho_atual > TAMANHO_MENSAGEM)
        throw std::overflow_error("Estouro de mensagem");
    else if (tamanho_atual == TAMANHO_MENSAGEM) 
        return mensagem;

    mensagem += "|";
    int tamanho_extra = TAMANHO_MENSAGEM - tamanho_atual - 1;
    std::string zeros(tamanho_extra, '0');
    mensagem += zeros;

    return mensagem;
};
using namespace std;
std::tuple<char, unsigned, unsigned>
mensageiro_ex_mut::dividir_mensagem(std::string mensagem)
{
    std::string pedacos [3] = {std::string(""), std::string(""), std::string("")};
    int indice = 0;
    for (char ch: mensagem) {
        if (indice >= 3) break;
        if (ch == '|') {
            indice++;
            continue;
        }
        else 
            pedacos[indice] += ch;
    }
    return std::tuple<char, unsigned, unsigned> (pedacos[0][0], stoi(pedacos[1]), stoi(pedacos[2]));
};
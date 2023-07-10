CXX = g++
CXXFLAGS = -Wall -g

TESTS = teste_enviar teste_receber teste_processo teste_envia_recebe_manual
EXECS = $(TESTS) main

AUX = socket_udp.o coordenador.o mensageiro_ex_mut.o

all: $(AUX) $(EXECS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $^ -c

$(EXECS): %: %.o $(AUX)
	$(CXX) $(CXXFLAGS) $^ -pthread -o $@

clean:
	rm -f $(EXECS) *.o

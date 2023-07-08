CXX = g++
CXXFLAGS = -Wall -g

TESTS = teste_enviar teste_receber teste_processo
EXECS = $(TESTS) main

AUX = socket_udp.o coordenador.o

all: $(AUX) $(EXECS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $^ -c

$(EXECS): %: %.o $(AUX)
	$(CXX) $(CXXFLAGS) $^ -pthread -o $@

clean:
	rm -f $(EXECS) *.o

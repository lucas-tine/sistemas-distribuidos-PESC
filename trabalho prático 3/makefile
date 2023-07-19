CXX = g++
CXXFLAGS = -Wall -g

TESTS = teste_enviar teste_receber teste_processo teste_envia_recebe_manual
EXECS = processo iniciar_coordenador iniciar_processos

AUX = socket_udp.o coordenador.o mensageiro_ex_mut.o

all: $(AUX) $(EXECS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $^ -c

$(EXECS): %: %.o $(AUX)
	$(CXX) $(CXXFLAGS) $^ -pthread -o $@

$(TESTS): %: %.o $(AUX)
	$(CXX) $(CXXFLAGS) $^ -pthread -o $@


clean:
	rm -f $(EXECS) $(TESTS) *.o log-*.txt resultado.txt

clean-txt:
	rm -f log-*.txt resultado.txt

clean-obj:
	rm -f *.o

tests: $(TESTS)
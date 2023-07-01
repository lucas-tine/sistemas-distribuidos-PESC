CXX = g++
CXXFLAGS = -Wall -g

TESTS = teste_enviar teste_receber
EXECS = main $(TESTS)

AUX = socket_udp.o coordenador.o

all: $(EXECS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $^ -c

$(EXECS): %: %.o $(AUX)
	$(CXX) $(CXXFLAGS) $^ -pthread -o $@

clean:
	rm -f $(EXECS) *.o

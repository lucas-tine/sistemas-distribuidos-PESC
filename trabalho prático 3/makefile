CXX = g++
CXXFLAGS = -Wall -g

EXECS = main teste_envia_recebe teste_envia_recebe_2

AUX = socket_udp.o coordenador.o

all: $(EXECS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $^ -c

$(EXECS): %: %.o $(AUX)
	$(CXX) $(CXXFLAGS) $^ -pthread -o $@

clean:
	rm -f $(EXECS) *.o

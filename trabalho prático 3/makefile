CXX = g++
CXXFLAGS = -Wall -g

EXECS = coordenador teste_envia_recebe

AUX = socket_udp.o

all: $(EXECS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $^ -pthread -c

$(EXECS): %: %.o $(AUX)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(EXECS) *.o

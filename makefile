# the compiler
CC = g++

# compiler flags:
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall

# the build target executable:
TARGET = sinais.sinalizador	\
	sinais.recebe_sinais	\
	pipe.primos 	\
	produtor_socket	\
	consumidor_socket

all: $(TARGET) 

%: %.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(TARGET)

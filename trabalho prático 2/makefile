# the compiler
CC = g++

# compiler flags:
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -std=c++2a -Wall

# the build target executable:
TARGET = item3.execucao

all: $(TARGET) 

%: %.cpp
	$(CC) $(CFLAGS) $< -pthread -o $@

clean:
	$(RM) $(TARGET)

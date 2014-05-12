CXX=clang
CXXFLAGS=-std=c++11 -Wall -Wextra -Werror -g -O2
LDLIBS=-lstdc++ -g

all: main

main: main.o components.o tools.o Desc.o

clean:
	rm -f main *.o

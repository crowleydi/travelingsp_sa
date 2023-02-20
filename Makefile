CXXFLAGS=-O3 -fopenmp
CC=g++
LDFLAGS=-fopenmp

tsp: tsp.o
clean:
	rm -f tsp tsp.o

CPLEXFLAGS=-O3 -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -I/opt/cplex/cplex/include -I/opt/cplex/concert/include  -L/opt/cplex/cplex/lib/x86-64_linux/static_pic -lilocplex -lcplex -L/opt/cplex/concert/lib/x86-64_linux/static_pic -lconcert -lm -pthread -std=c++0x -ldl

CFLAGS=-std=c++11 -static-libstdc++ -static-libgcc -Wall

all:clique.o
	g++  clique.o -o clique.run $(CPLEXFLAGS) $(CFLAGS)


%.o: %.cpp %.hpp
	g++ -c $< -o $@ $(CFLAGS)

clique.o: clique.cpp
	g++ -c -o clique.o clique.cpp $(CPLEXFLAGS) $(CFLAGS)

clean:
	rm -f *.o


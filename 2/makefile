CC=g++

CFLAGS=-c

INCDIR=-I/opt/intel/ipp/include

LIBDIR=-L/opt/intel/ipp/lib/intel64

LIBFILE=-lippcore

all: main

main: main.o 
	$(CC) $(LIBDIR) $(LIBFILE) main.o -o main

main.o: main.cpp
	$(CC) $(CFLAGS) -msse3 $(INCDIR) main.cpp

clean:
	rm -f *o main

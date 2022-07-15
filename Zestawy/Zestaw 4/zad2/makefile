cc = gcc -Wall

all: lib

lib: main.o main.c
	$(cc) -c main.c
	$(cc) main.o -o main

clean:
	rm -f *.o main
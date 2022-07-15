cc = gcc -Wall

all: lib

lib: main.o main.c
	$(cc) -DLIB=1 -c main.c
	$(cc) -DLIB=1 main.o -o main

clean:
	rm -f *.o main *.txt
cc = gcc -Wall

all: lib 

lib: main.o main.c
	$(cc) -c main.c
	$(cc) main.o -o main

test: main
	./main test.txt

clean:
	rm -f *.o main
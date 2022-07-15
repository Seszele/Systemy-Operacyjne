cc = gcc -Wall

all: lib EXEC

lib: main.o main.c
	$(cc) -c main.c
	$(cc) main.o -o main

EXEC: EXEC.o EXEC.c
	$(cc) -c EXEC.c
	$(cc) EXEC.o -o EXEC

test: lib EXEC
	./main 0
	./main 1
	./main 2
	./main 3

clean:
	rm -f *.o main EXEC
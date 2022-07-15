cc = gcc -pthread -Wall

all: main

main: main.o main.c 
	$(cc) -c main.c
	$(cc) main.o -o main

clean:
	rm -f *.o main
	killall -q main || true
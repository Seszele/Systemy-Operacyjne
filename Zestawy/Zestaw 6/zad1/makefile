cc = gcc -Wall

all: server client 

server: main.o main.c
	$(cc) -c main.c
	$(cc) main.o -o main

client: client.o client.c
	$(cc) -c client.c
	$(cc) client.o -o client

test: server client
	killall -q main || true
	./main &
	./client 

test2: server client
	killall -q main || true
	./main &
	./client 

clean:
	rm -f *.o main client
	killall -q main || true
	killall -q client || true
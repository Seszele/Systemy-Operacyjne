cc = gcc -Wall

all: server client

server: server.o server.c
		$(cc) -c server.c -lpthread
		$(cc) server.o -o server -lpthread

 client:  client.o  client.c
		$(cc) -c  client.c -lpthread
		$(cc)  client.o -o  client -lpthread

clean:
		rm -f *.o server client srv
		killall -q server || true
		killall -q client || true
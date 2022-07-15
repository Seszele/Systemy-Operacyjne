cc = gcc -Wall

all: lib

sys: main.o main.c
	$(cc) -DSYS=1 -c main.c
	$(cc) -DSYS=1 main.o -o main

lib: main.o main.c
	$(cc) -DLIB=1 -c main.c
	$(cc) -DLIB=1 main.o -o main


clean:
	rm -f *.o main
cc = gcc -Wall

all: nftw opendir
	$(info main  --> opendir(), readdir()....  )
	$(info main2 -->  nftw()  )

opendir: main.o main.c
	$(cc) -c main.c
	$(cc) main.o -o main

nftw: main2.o main2.c
	$(cc) -c main2.c
	$(cc) main2.o -o main2

test: main main2
	./main test
	./main2 test

clean:
	rm -f *.o main main2
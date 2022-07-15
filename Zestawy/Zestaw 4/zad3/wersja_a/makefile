cc = gcc -Wall

all: sender catcher

sender: sender.o sender.c
	$(cc) -c sender.c
	$(cc) sender.o -o sender

catcher: catcher.o catcher.c
	$(cc) -c catcher.c
	$(cc) catcher.o -o catcher

test: all
	./catcher
	./sender

clean:
	rm -f *.o catcher sender
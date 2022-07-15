cc = gcc -Wall

all: cook deliverer main

cook: cook.o cook.c constants.h
	$(cc) -c cook.c
	$(cc) cook.o -o cook

deliverer: deliverer.o deliverer.c constants.h
	$(cc) -c deliverer.c
	$(cc) deliverer.o -o deliverer

main: main.o main.c constants.h
	$(cc) -c main.c
	$(cc) main.o -o main

clean:
	rm -f *.o cook deliverer main
	killall -q cook || true
	killall -q deliverer || true
	killall -q main || true
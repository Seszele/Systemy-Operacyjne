cc = gcc -Wall

all: prod cons 

prod: producent.o producent.c
	$(cc) -c producent.c
	$(cc) producent.o -o producent

cons: consument.o consument.c
	$(cc) -c consument.c
	$(cc) consument.o -o consument

test: consument producent
	./producent fifo 0 prod0.txt 5 &
	./producent fifo 5 prod2.txt 5 &
	./producent fifo 1 prod1.txt 5 &
	./consument fifo dest.txt 5

clean:
	rm -f *.o consument producent
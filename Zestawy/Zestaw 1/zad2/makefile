all: main

mylib.o: mylib.c
	gcc -Wall -c mylib.c

libmylib.a: mylib.o
	ar rcs libmylib.a mylib.o

main.o: main.c
	gcc -Wall -c main.c

main: main.o libmylib.a
	gcc  -Wall main.o libmylib.a -o main

clean:
	rm *.a *.o main -f





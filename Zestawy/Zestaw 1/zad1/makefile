cc = gcc -Wall

all: shared cleanO static

static: libmylib.a
shared: libmylib.so

libmylib.a: mylib.o
	$(cc) -c mylib.c
	ar rcs libmylib.a mylib.o

libmylib.so: mylib.c
	$(cc) -fPIC -c mylib.c
	$(cc) -shared mylib.o -o libmylib.so

clean:
	rm *.a *.o *so -f 

cleanO:
	rm -f *.o





cc = gcc -Wall
dest = raport3a.txt
dest2 = raport3b.txt

all: static

#static lib
libmylib.a: mylib.c
	$(cc) -c mylib.c
	ar rcs libmylib.a mylib.o

#shared lib
libmylib.so: mylib.c
	$(cc) -fPIC -c mylib.c
	$(cc) -shared mylib.o -o libmylib.so

static: main.c libmylib.a
	$(cc) -DSTATIC=1 -c main.c
	$(cc) main.o libmylib.a -o main

shared: main.c libmylib.so
	$(cc) -DSHARED="1" -c main.c
	$(cc) main.o -L. -lmylib -Wl,-rpath=. -o main

dynamic: main.c libmylib.so
	$(cc) -DDYNAMIC=1 main.c -o main -ldl
#
static_O0: main.c
	$(cc) -O0 -c mylib.c
	ar rcs libmylib.a mylib.o
	$(cc) -O0 -DSTATIC=1 -c main.c
	$(cc) -O0 main.o libmylib.a -o main
static_Os: main.c
	$(cc) -Os -c mylib.c
	ar rcs libmylib.a mylib.o
	$(cc) -Os -DSTATIC=1 -c main.c
	$(cc) -Os main.o libmylib.a -o main
static_O3: main.c
	$(cc) -O3 -c mylib.c
	ar rcs libmylib.a mylib.o
	$(cc) -O3 -DSTATIC=1 -c main.c
	$(cc) -O3 main.o libmylib.a -o main

clean:
	rm *.a *.o *so main -f

test_O0:clean static_O0
	echo Optymalizacja -O0:  >> $(dest2)
	./main t 1000 s z_u_10_male z ex1.txt u 0 z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0 z ex1.txt u 0 z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0
	./main t 1000 s z_u_10_duze z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0 z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0
	./main t 1000 s z_6_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_7_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_9_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_u_5_duze z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0
	./main t 1000 s z_10_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt s u_10_duze u 0 u 1 u 2 u 3 u 4 u 5 u 6 u 7 u 8 u 9
	./main t 1000 s z_10_srednie z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt s u_10_srednie u 0 u 1 u 2 u 3 u 4 u 5 u 6 u 7 u 8 u 9
	./main t 1000 s z_5_srednie z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt
	./main t 1000 s z_8_male z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt
	./main s t_1000 t 1000 s z_3_duze z ex3.txt z ex4.txt z ex4.txt

test_Os:clean static_Os
	echo >>$(dest2)
	echo Optymalizacja -Os:  >> $(dest2)
	./main t 1000 s z_u_10_male z ex1.txt u 0 z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0 z ex1.txt u 0 z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0
	./main t 1000 s z_u_10_duze z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0 z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0
	./main t 1000 s z_6_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_7_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_9_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_u_5_duze z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0
	./main t 1000 s z_10_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt s u_10_duze u 0 u 1 u 2 u 3 u 4 u 5 u 6 u 7 u 8 u 9
	./main t 1000 s z_10_srednie z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt s u_10_srednie u 0 u 1 u 2 u 3 u 4 u 5 u 6 u 7 u 8 u 9
	./main t 1000 s z_5_srednie z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt
	./main t 1000 s z_8_male z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt
	./main s t_1000 t 1000 s z_3_duze z ex3.txt z ex4.txt z ex4.txt

test_O3:clean static_O3
	echo >>$(dest2)
	echo Optymalizacja -O3:  >> $(dest2)
	./main t 1000 s z_u_10_male z ex1.txt u 0 z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0 z ex1.txt u 0 z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0
	./main t 1000 s z_u_10_duze z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0 z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0
	./main t 1000 s z_6_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_7_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_9_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_u_5_duze z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0
	./main t 1000 s z_10_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt s u_10_duze u 0 u 1 u 2 u 3 u 4 u 5 u 6 u 7 u 8 u 9
	./main t 1000 s z_10_srednie z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt s u_10_srednie u 0 u 1 u 2 u 3 u 4 u 5 u 6 u 7 u 8 u 9
	./main t 1000 s z_5_srednie z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt
	./main t 1000 s z_8_male z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt
	./main s t_1000 t 1000 s z_3_duze z ex3.txt z ex4.txt z ex4.txt

test_shared: clean shared
	echo Biblioteka dzielona:  >> $(dest)
	./main t 1000 s z_u_10_male z ex1.txt u 0 z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0 z ex1.txt u 0 z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0
	./main t 1000 s z_u_10_duze z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0 z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0
	./main t 1000 s z_6_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_7_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_9_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_u_5_duze z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0
	./main t 1000 s z_10_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt s u_10_duze u 0 u 1 u 2 u 3 u 4 u 5 u 6 u 7 u 8 u 9
	./main t 1000 s z_10_srednie z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt s u_10_srednie u 0 u 1 u 2 u 3 u 4 u 5 u 6 u 7 u 8 u 9
	./main t 1000 s z_5_srednie z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt
	./main t 1000 s z_8_male z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt
	./main s t_1000 t 1000 s z_3_duze z ex3.txt z ex4.txt z ex4.txt

test_static: clean static
	echo Biblioteka statyczna:  >> $(dest)
	./main t 1000 s z_u_10_male z ex1.txt u 0 z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0 z ex1.txt u 0 z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0
	./main t 1000 s z_u_10_duze z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0 z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0
	./main t 1000 s z_6_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_7_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_9_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_u_5_duze z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0
	./main t 1000 s z_10_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt s u_10_duze u 0 u 1 u 2 u 3 u 4 u 5 u 6 u 7 u 8 u 9
	./main t 1000 s z_10_srednie z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt s u_10_srednie u 0 u 1 u 2 u 3 u 4 u 5 u 6 u 7 u 8 u 9
	./main t 1000 s z_5_srednie z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt
	./main t 1000 s z_8_male z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt
	./main s t_1000 t 1000 s z_3_duze z ex3.txt z ex4.txt z ex4.txt

test_dynamic: clean dynamic
	echo Biblioteka ładowana dynamicznie:  >> $(dest)
	./main t 1000 s z_u_10_male z ex1.txt u 0 z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0 z ex1.txt u 0 z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0  z ex1.txt u 0
	./main t 1000 s z_u_10_duze z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0 z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0
	./main t 1000 s z_6_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_7_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_9_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt
	./main t 1000 s z_u_5_duze z ex4.txt u 0 z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0  z ex4.txt u 0
	./main t 1000 s z_10_duze z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt z ex4.txt s u_10_duze u 0 u 1 u 2 u 3 u 4 u 5 u 6 u 7 u 8 u 9
	./main t 1000 s z_10_srednie z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt s u_10_srednie u 0 u 1 u 2 u 3 u 4 u 5 u 6 u 7 u 8 u 9
	./main t 1000 s z_5_srednie z ex3.txt z ex3.txt z ex3.txt z ex3.txt z ex3.txt
	./main t 1000 s z_8_male z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt z ex1.txt
	./main s t_1000 t 1000 s z_3_duze z ex3.txt z ex4.txt z ex4.txt

tests: test_static test_shared test_dynamic
	$(info Tests completed)

tests_O: test_O0 test_O3 test_Os
	$(info O Tests completed)
	
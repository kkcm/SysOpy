CC = gcc -Wall -std=c11 -ggdb


all: static shared clean

static:
	$(CC) -c library.c
	ar rcs library.a library.o

shared:
	$(CC) -c -fPIC library.c
	$(CC) -shared -fPIC -o library.so library.o

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so

ex3a_stat:
	make static
	$(CC) -o check main.c library.a -I .
	make checks

ex3a_shar:
	make shared
	$(CC) -o check main.c -L. library.so -I .
	make checks

ex3a_dyn:
	make shared
	$(CC) -o check main.c -I .
	make checks


checks: 
	./check 4000000 30 static
	./check 500000 40 static remove_many_blocks 10000
	./check 600000 50 static rotate_remove_many_blocks 20000 remove_many_blocks 10000
	./check 700000 60 static remove_many_blocks 30000 add 2000 search_element 15

	./check 400000 40 dynamic
	./check 500000 50 dynamic remove_many_blocks 10000
	./check 600000 60 dynamic rotate_remove_many_blocks 20000 remove_many_blocks 10000
	./check 700000 70 dynamic remove_many_blocks 30000 add 20 search_element 15
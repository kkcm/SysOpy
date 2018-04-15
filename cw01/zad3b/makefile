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

ex3b_stat_O0:
	make static
	$(CC) -o check main.c library.a -I . -O0
	make checks

ex3b_stat_O2:
	make static
	$(CC) -o check main.c library.a -I . -O2
	make checks

ex3b_stat_Os:
	make static
	$(CC) -o check main.c library.a -I . -Os
	make checks

ex3b_shar_O0:
	make shared
	$(CC) -o0 check main.c -L. library.so -I . -O0
	make checks

ex3b_shar_O2:
	make shared
	$(CC) -o2 check main.c -L. library.so -I . -O2
	make checks

ex3b_shar_Os:
	make shared
	$(CC) -os check main.c -L. library.so -I . -Os
	make checks

ex3b_dyn_O0:
	make shared
	$(CC) -o0 check main.c -I . -O0
	make checks

ex3b_dyn_O2:
	make shared
	$(CC) -o2 check main.c -I . -O2
	make checks

ex3b_dyn_Os:
	make shared
	$(CC) -os check main.c -I . -Os
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
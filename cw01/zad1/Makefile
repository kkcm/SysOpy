CC = gcc -Wall -std=c11 -ggdb


all: static dynamic clean

static:
	$(CC) -c library.c
	ar rcs liblibrary.a library.o

dynamic:
	$(CC) -c -fPIC library.c
	$(CC) -shared -fPIC -o liblibrary.so library.o

clean:
	rm -f *.o
	#rm -f *.a
	#rm -f *.so
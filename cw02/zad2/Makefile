CC = gcc -Wall -std=c11 -ggdb

all: compile clean

compile:
	$(CC) -o main main.c -O2

clean:
	rm -f *.o

test:
	make compile
	./main /Users/kubi/Desktop  ">" "2018-03-22" "23:23:23" nftw
	./main /Users/kubi/Desktop  "<" "2018-03-22" "23:23:23" stat
	./main /Users/kubi/Desktop  "=" "2018-03-23" "09:24:57" stat

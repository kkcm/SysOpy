
CC=gcc
CFLAGS= -g -O2 -std=c11 -ggdb -Wall -lpthread -lm
M_PATH = main

all: main

main: main.c
	$(CC) $(CFLAGS) $(M_PATH).c -o $(M_PATH)

test: main
	./main configuration

clean:
	rm -rf results main  *.o
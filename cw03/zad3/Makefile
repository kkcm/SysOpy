CC = gcc -Wall -std=c11 -ggdb
M_PATH = main


all: compile clean

test_all: test test_prog test_time test_memory

compile:
	$(CC) -o $(M_PATH) $(M_PATH).c -O2

clean:
	rm -f *.o

test:
	make compile
	./$(M_PATH) test.txt 2 2

test_prog:
	make compile
	$(CC) -o help_prog help_prog.c
	./$(M_PATH) test_help.txt 2 2

test_time:
	make compile
	$(CC) -o big_time big_time.c
	./$(M_PATH) test_time.txt 2 2

test_memory:
	make compile
	$(CC) -o big_memory big_memory.c
	./$(M_PATH) test_memory.txt 2 2

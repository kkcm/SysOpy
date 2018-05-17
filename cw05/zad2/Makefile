CC = gcc -Wall -std=c11 -ggdb
M_PATH_1 = master
M_PATH_2 = slave
FIFO = fifo

all: compile clean

compile:
	$(CC) -o $(M_PATH_1) $(M_PATH_1).c
	$(CC) -o $(M_PATH_2) $(M_PATH_2).c

clean:
	rm -f *.o
	rm FIFO

test_master:
	./$(M_PATH_1) FIFO

test_slave:
	./$(M_PATH_2) FIFO 7
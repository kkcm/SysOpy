CC = gcc -Wall -std=c11 -ggdb
M_PATH_1 = server
M_PATH_2 = client

all: compile clean

compile:
	$(CC) -o $(M_PATH_1) $(M_PATH_1).c -lrt
	$(CC) -o $(M_PATH_2) $(M_PATH_2).c -lrt

clean:
	rm -f *.o

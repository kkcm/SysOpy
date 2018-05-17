CC = gcc 
CFLAGS = -g -O2 -std=c11 -ggdb -Wall -D_POSIX_C_SOURCE=199309L -lrt -pthread
COMMON = barbershop
M_PATH_1 = barber
M_PATH_2 = client_producer


all: compile clean

compile:
	$(CC) $(CFLAGS) $(COMMON).c $(M_PATH_1).c -o $(M_PATH_1)
	$(CC) $(CFLAGS) $(COMMON).c $(M_PATH_2).c -o $(M_PATH_2)

clean:
	rm -f *.o

test:
	make compile


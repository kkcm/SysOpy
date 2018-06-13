CC=gcc
CFLAGS= -g -O2 -std=c11 -ggdb -Wall9 -lpthread -lm
M_PATH = main

all: main

main: main.c
	$(CC) $(CFLAGS) $(M_PATH).c -o $(M_PATH)

test: main
	mkdir -p results
	bash -c 'for FILTER in {03,10,14,50,65}; do       	  				\
				for THREAD in {1,2,4,8}; do           					\
					./main $$THREAD                     				\
					before.pgm                          				\
					filters/filter$$FILTER              				\
					results/after.thread$$THREAD.filter$$FILTER.pgm;	\
				done;                                   				\
			done'

clean:
	rm -rf results main  *.o

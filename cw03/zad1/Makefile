CC = gcc -Wall -std=c11 -ggdb
M_PATH = main


all: compile clean

compile:
	$(CC) -o $(M_PATH) $(M_PATH).c -O2

clean:
	rm -f *.o

test:
	make compile
	./$(M_PATH) /Users/kubi/Desktop  ">" "2018-03-25" "23:23:23" nftw
	./$(M_PATH) /Users/kubi/Desktop  "<" "2018-03-25" "23:23:23" stat
	./$(M_PATH) /Users/kubi/Desktop  "=" "2018-03-23" "17:55:44" stat

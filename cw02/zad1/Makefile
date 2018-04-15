CC = gcc -Wall -std=c11 -ggdb
M_PATH = main

all: compile clean

compile:
	$(CC) -o main main.c -O2

clean:
	rm -f *.o

test:
	make compile


	./$(M_PATH) generate s_data 500 4
	./$(M_PATH) copy s_data l_data 500 4 sys
	./$(M_PATH) sort s_data 500 4 sys
	./$(M_PATH) sort l_data 500 4 lib

	./$(M_PATH) generate s_data 1000 4
	./$(M_PATH) copy s_data l_data 1000 4 sys
	./$(M_PATH) sort s_data 1000 4 sys
	./$(M_PATH) sort l_data 1000 4 lib

	./$(M_PATH) generate s_data 200 512
	./$(M_PATH) copy s_data l_data 200 512 sys
	./$(M_PATH) sort s_data 200 512 sys
	./$(M_PATH) sort l_data 200 512 lib

	./$(M_PATH) generate s_data 400 512
	./$(M_PATH) copy s_data l_data 400 512 sys
	./$(M_PATH) sort s_data 400 512 sys
	./$(M_PATH) sort l_data 400 512 lib

	./$(M_PATH) generate s_data 200 4096
	./$(M_PATH) copy s_data l_data 200 4096 sys
	./$(M_PATH) sort s_data 200 4096 sys
	./$(M_PATH) sort l_data 200 4096 lib

	./$(M_PATH) generate s_data 400 4096
	./$(M_PATH) copy s_data l_data 400 4096 sys
	./$(M_PATH) sort s_data 400 4096 sys
	./$(M_PATH) sort l_data 400 4096 lib

	./$(M_PATH) generate s_data 200 8192
	./$(M_PATH) copy s_data l_data 200 8192 sys
	./$(M_PATH) sort s_data 200 8192 sys
	./$(M_PATH) sort l_data 200 8192 lib

	./$(M_PATH) generate s_data 400 8192
	./$(M_PATH) copy s_data l_data 400 8192 sys
	./$(M_PATH) sort s_data 400 8192 sys
	./$(M_PATH) sort l_data 400 8192 lib

	./$(M_PATH) generate data 200 4
	./$(M_PATH) copy data s_data 200 4 sys
	./$(M_PATH) copy data l_data 200 4 lib

	./$(M_PATH) generate data 400 4
	./$(M_PATH) copy data s_data 400 4 sys
	./$(M_PATH) copy data l_data 400 4 lib

	./$(M_PATH) generate data 200 512
	./$(M_PATH) copy data s_data 200 512 sys
	./$(M_PATH) copy data l_data 200 512 lib

	./$(M_PATH) generate data 400 512
	./$(M_PATH) copy data s_data 400 512 sys
	./$(M_PATH) copy data l_data 400 512 lib

	./$(M_PATH) generate data 200 4096
	./$(M_PATH) copy data s_data 200 4096 sys
	./$(M_PATH) copy data l_data 200 4096 lib

	./$(M_PATH) generate data 400 4096
	./$(M_PATH) copy data s_data 400 4096 sys
	./$(M_PATH) copy data l_data 400 4096 lib

	./$(M_PATH) generate data 200 8192
	./$(M_PATH) copy data s_data 200 8192 sys
	./$(M_PATH) copy data l_data 200 8192 lib

	./$(M_PATH) generate data 4000 8192
	./$(M_PATH) copy data s_data 4000 8192 sys
	./$(M_PATH) copy data l_data 4000 8192 lib

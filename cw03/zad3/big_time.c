#include <stdlib.h>
#include <stdio.h>

int fib(int n){
	if (n == 0){
		return 0;
	} else if (n == 1){
		return 1;
	} else {
		return fib(n-1) + fib(n-2);
	}
}

int main (void){
	printf("This is big_time_program.\n");

	printf("fib(50) = %d\n", fib(50));

	return 0;
}
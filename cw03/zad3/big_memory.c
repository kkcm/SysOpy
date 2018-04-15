#include <stdlib.h>
#include <stdio.h>

#define ARR_SIZE 1000000000

int main (void){
	printf("This is big_memory_program.\n");

	int i = 0;

	char* array = calloc(ARR_SIZE, sizeof(char));
	for (i = 0; i < ARR_SIZE; i++){
		array[i] = i % 64 + 32;
	}

	return 0;
}
#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_LENGTH 100000
#define MAX_WIDTH 100000

char global_array[MAX_LENGTH][MAX_WIDTH];

struct my_array* create_arr (int num_of_blocks, int is_static){
    if (num_of_blocks <= 0){
        return NULL;
    }

    struct my_array* arr = malloc(sizeof(struct my_array));
    arr -> is_static = is_static;
    arr -> num_of_blocks = num_of_blocks;

    if(is_static){
        arr -> array = (char** ) global_array;
    } else {
        arr -> array = calloc(num_of_blocks, sizeof(char*));
    }

    return arr;
}

void add_block (struct my_array* arr, char block[], int index){
    if (arr == NULL){
        printf("You didn't create array, so you can't add block.");
        return;
    }

    if (index < 0 || index >= arr -> num_of_blocks){
        printf("Your index %d isn't appropriate.", index);
        return;
    }

    if (arr -> is_static){
        arr -> array[index] = block;
    } else {
        if (arr -> array[index] != NULL){
        //    printf("This position isn't free, sorry.");
        //    return;
            strcpy(arr -> array[index], block);
        } else {
            arr -> array[index] = calloc(strlen(block), sizeof(char));
            strcpy(arr -> array[index], block);
        }
    }
}

void remove_block (struct my_array* arr, int index){
    if (index < 0 || index >= arr -> num_of_blocks){
        printf("Your index %d doesn't exist.", index);
        return;
    }

    if (arr -> is_static){
        arr -> array[index] = NULL;
    } else {
        free (arr -> array[index]);
        arr -> array[index] = NULL;
    }
}

void remove_arr (struct my_array* arr){
    for (int i = 0; i < arr -> num_of_blocks; i++){
        remove_block(arr, i);
    }
    free(arr);
}

int block_to_int (char* block){
    int result = 0;
    for (int i = 0; i < strlen(block); i++){
        result += (int) block[i];
    }
    return result;
}

char* find_nearest (struct my_array* arr, int index){
    char* nearest = NULL;
    int minimal_difference = INT_MAX;
    int element_value;

    if (arr -> array[index] != NULL){
        char* block = arr -> array[index];
        element_value = block_to_int(block);
    }

    for (int i =0; i < arr -> num_of_blocks; i++){
        char* block = arr -> array[i];
        if (block != NULL) {
            int difference = abs(block_to_int(block) - element_value);
            if (difference < minimal_difference){
                minimal_difference = difference;
                nearest = block;
            }
        }
    }
    return nearest;
}
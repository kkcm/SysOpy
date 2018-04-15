#ifndef _LIBRARY_H
#define _LIBRARY_H

struct my_array {
    char** array;
    int num_of_blocks;
    int is_static;
};

struct my_array* create_arr (int num_of_blocks, int is_static);

void remove_arr (struct my_array* arr);

void add_block (struct my_array* arr, char block[], int index);

void remove_block (struct my_array* arr, int index);

char* find_nearest (struct my_array* arr, int index);


#endif

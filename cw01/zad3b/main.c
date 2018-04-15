#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <limits.h>
#include <sys/times.h>

void * dl_handle;

char* generate_new_random_string(int length){
    if (length < 1){
        return NULL;
    }
    char* base_string = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int length_base_string = (int) strlen(base_string);
    char* new_random_string = calloc((size_t) length, sizeof(char));

    for(int i = 0; i < length; i++){
        new_random_string[i] = base_string[rand() % length_base_string];
    }
    return new_random_string;
}

void print_array (struct my_array* arr) {
    if (arr == NULL) {
        printf("Array doesn't exist. You gave me NULL");
        return;
    }

    for (int i = 0; i < arr->num_of_blocks; i++) {
        printf("%i ", i);
        printf("%s ", arr->array[i]);
        printf("\n");
    }
}

void fill_array (struct my_array* arr, int size_of_block){
    for (int i = 0; i < arr -> num_of_blocks; i++){
        char* new_random_string = generate_new_random_string(size_of_block);
        add_block(arr, new_random_string, i);
    }
}

void remove_many_blocks(struct my_array* arr, int number){
    for (int i = 0; i < number; i++){
        remove_block(arr, i);
    }
}

void add_many_block (struct my_array* arr, int number, int size_of_block){
    for (int i = 0; i < number; i++){
        char* new_random_string = generate_new_random_string(size_of_block);
        add_block(arr, new_random_string, i);
    }
}

void remove_and_add (struct my_array* arr, int number, int size_of_block){
    remove_many_blocks(arr, number);
    add_many_block(arr, number, size_of_block);
}

void rotate_remove_and_add (struct my_array* arr, int number, int size_of_block) {
    for (int i = 0; i < number; i++){
        remove_block(arr, arr -> num_of_blocks - i - 1);
        char* new_random_string = generate_new_random_string(size_of_block);
        add_block(arr, new_random_string, arr -> num_of_blocks - i - 1);
    }

}

void execute_job (char* job, int parameter, struct my_array* arr, int size_of_block){
    if(job == NULL){
        printf("You didn't give me more jobs or you wrote bad name of job.");
        return;
    }

    if (strcmp(job, "search_element") == 0) {
        find_nearest(arr, parameter);
    } else if (strcmp(job, "remove") == 0){
        remove_many_blocks(arr, parameter);
    } else if (strcmp(job, "add") == 0){
        add_many_block(arr, parameter, size_of_block);
    } else if (strcmp(job, "remove_and_add") == 0){
        remove_and_add (arr, parameter, size_of_block);
    } else if (strcmp(job, "rotate_remove_and_add") == 0) {
        rotate_remove_and_add(arr, parameter, size_of_block);
    }
}

void start_measurements(clock_t* start_time_processor, struct tms* start_time_kernel){
    *start_time_processor = clock();
    times(start_time_kernel);
}

void stop_measurements(clock_t* end_time_processor, struct tms* end_time_kernel){
    *end_time_processor = clock();
    times(end_time_kernel);
}

void print_and_save_in_raport_time(FILE* my_file, clock_t* start_time_processor, clock_t* end_time_processor,
                                   struct tms* start_time_kernel, struct tms* end_time_kernel){

//    *end_time_processor = clock();
//    times(end_time_kernel);


    double real_time = (double) (*end_time_processor - *start_time_processor) / CLOCKS_PER_SEC;
    double kernel_time = (double) (end_time_kernel->tms_stime - start_time_kernel->tms_stime) / CLOCKS_PER_SEC;
    double user_time = (double) (end_time_kernel->tms_utime - start_time_kernel->tms_utime) / CLOCKS_PER_SEC;


    printf("Real time: %.8lf \n"
           "Kernel time: %.8lf \n"
           "User time: %.8lf \n", real_time, kernel_time, user_time);

    fprintf(my_file, "Real time: %.8lf \n"
                     "Kernel time: %.8lf \n"
                     "User time: %.8lf \n", real_time, kernel_time, user_time);

}






int main(int argc, char **argv) {


 #ifdef DL_HANDLE

    dl_handle = dlopen("./liblibrary.so", RTLD_LAZY);
    typedef struct my_array arr;
    my_array* (create_arr) (int num_of_blocks, int is_static)=dlsym(dl_handle, "create_arr");
    my_array* (add_block) (my_array *arr, char block[], int index)=dlsym(dl_handle, "add_block");
    my_array* (remove_block) (my_array *arr, int index)=dlsym(dl_handle, "remove_block");
    my_array* (remove_arr) (my_array *arr)=dlsym(dl_handle, "remove_arr");
    char (*find_nearest) (my_array *arr, int index)=dlsym(dl_handle, "find_nearest");
#endif

    if (argc < 4){
        printf("You did't give me required arguments. I need number of blocks, size of block, mode of memory allocation and list of instruction to do (max 3).");
        return 1;
    }

    int number_of_blocks = (int) strtol(argv[1], NULL, 10);
    int size_of_block = (int) strtol(argv[2], NULL, 10);
    int is_static;

    FILE *my_file = fopen("raport3b.txt", "a");
    if (!my_file){
        printf ("There is some problem with writing to file!\n");
        return 1;
    }

    if (strcmp(argv[3], "static") == 0){
        is_static = 1;
        printf("This test is for static memory allocation. \n\n");
        fprintf(my_file, "This test is for static memory allocation. \n\n");
    } else if (strcmp(argv[3], "dynamic") == 0){
        is_static = 0;
        printf("This test is for dynamic memory allocation. \n\n");
        fprintf(my_file, "This test is for dynamic memory allocation. \n\n");
    } else {
        printf("You gave me wrong type of memory allocation. You can write only static or dynamic.");
        return 1;
    }

    char* job1 = NULL;
    char* job2 = NULL;
    char* job3 = NULL;
    int arg1 = -1;
    int arg2 = -1;
    int arg3 = -1;

    if (argc >= 6){
        job1 = argv[4];
        arg1 = (int) strtol(argv[5], NULL, 10);
    }

    if (argc >= 8){
        job2 = argv[6];
        arg2 = (int) strtol(argv[7], NULL, 10);
    }

    if (argc >= 10){
        job3 = argv[8];
        arg3 = (int) strtol(argv[9], NULL, 10);
    }

    clock_t* start_time_processor = malloc(sizeof(clock_t));
    clock_t* end_time_processor = malloc(sizeof(clock_t));

    struct tms* start_time_kernel = malloc(sizeof(struct tms));
    struct tms* end_time_kernel = malloc(sizeof(struct tms));

    srand((unsigned int) time(NULL));

    printf("Let's start measuring time: \n");
    fprintf(my_file, "Let's start measuring time: \n");

    start_measurements(start_time_processor, start_time_kernel);

    struct my_array* arr;
    arr = create_arr(number_of_blocks, is_static);
    fill_array(arr, size_of_block);

    stop_measurements(end_time_processor, end_time_kernel);
    print_and_save_in_raport_time( my_file, start_time_processor, end_time_processor, start_time_kernel, end_time_kernel);

    printf("This is time for creating and filling array with num_of_blocks - %d.\n", number_of_blocks);
    fprintf(my_file, "This is time for creating and filling array with num_of_blocks - %d.\n", number_of_blocks);


    start_measurements(start_time_processor, start_time_kernel);
    execute_job (job1, arg1, arr, size_of_block);

    stop_measurements(end_time_processor, end_time_kernel);
    print_and_save_in_raport_time( my_file, start_time_processor, end_time_processor, start_time_kernel, end_time_kernel);

    printf("This is time for first operation - %s on %d element.\n", job1, arg1);
    fprintf(my_file, "This is time for first operation - %s on %d element.\n", job1, arg1);

    start_measurements(start_time_processor, start_time_kernel);
    execute_job (job2, arg2, arr, size_of_block);

    stop_measurements(end_time_processor, end_time_kernel);
    print_and_save_in_raport_time( my_file, start_time_processor, end_time_processor, start_time_kernel, end_time_kernel);

    printf("This is time for second operation - %s on %d element.\n", job2, arg2);
    fprintf(my_file, "This is time for second operation - %s on %d element.\n", job2, arg2);

    start_measurements(start_time_processor, start_time_kernel);
    execute_job (job3, arg3, arr, size_of_block);

    stop_measurements(end_time_processor, end_time_kernel);
    print_and_save_in_raport_time( my_file, start_time_processor, end_time_processor, start_time_kernel, end_time_kernel);

    printf("This is time for third operation - %s on %d element.\n", job3, arg3);
    fprintf(my_file, "This is time for third operation - %s on %d element.\n", job3, arg3);

    start_measurements(start_time_processor, start_time_kernel);
    remove_arr(arr);

    stop_measurements(end_time_processor, end_time_kernel);
    print_and_save_in_raport_time( my_file, start_time_processor, end_time_processor, start_time_kernel, end_time_kernel);

    printf("This is time for removing array with num_of_blocks - %d.\n", number_of_blocks);
    fprintf(my_file, "This is time for removing array with num_of_blocks - %d.\n", number_of_blocks);

    printf("The end of the tests. \n\n\n");
    fprintf(my_file, "The end of the tests. \n\n\n");

    free(start_time_kernel);
    free(start_time_processor);
    free(end_time_kernel);
    free(end_time_processor);


    fclose(my_file);

    #ifdef DL_HANDLE
    dlclose(dl_handle);
    #endif

    return 0;
}

#ifndef BARBERSHOP_H
#define BARBERSHOP_H

#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>





// #define _POSIX_C_SOURCE 199309L
#define MAX_CHAIRS_NUMBER 21
#define BARBERSHOP_ID 3712438756
#define FAILURE_EXIT(code, message, ...) {printf(message, getpid(), ##__VA_ARGS__); exit(code);}

typedef struct barber_fifo {
    pid_t queue[MAX_CHAIRS_NUMBER];
    pid_t seat;
    int queue_head;
    int queue_tail;
    int max;
} barber_fifo;

const char* shared_memory_path;
const char* barber_path;
const char* fifo_path;
const char* clients_path;
const char* sitting_path;

void init_fifo(barber_fifo* fifo, int max);

int push_fifo(barber_fifo* fifo, pid_t a);

pid_t pop_fifo (barber_fifo* fifo);

long get_time();

#endif //BARBERSHOP_H

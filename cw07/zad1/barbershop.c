//
// Created by Kuba Kołoczek on 10.05.2018.
//

#include "barbershop.h"

#include <time.h>

void init_fifo(barber_fifo* fifo, int max){
    fifo -> max = max;
    fifo -> queue_head = -1;
    fifo -> queue_tail = 0;
    fifo -> seat = 0;
}

int is_full(barber_fifo* fifo){
    if(fifo -> queue_head ==  fifo-> queue_tail) return 1;
    else return 0;
}

int is_empty(barber_fifo* fifo){
    if(fifo -> queue_head ==  -1) return 1;
    else return 0;
}

int push_fifo(barber_fifo* fifo, pid_t a){
    if(is_full(fifo) == 1) return -1;
    if(is_empty(fifo) == 1)
        fifo -> queue_head = fifo -> queue_tail = 0;

    fifo -> queue[fifo -> queue_tail] = a;
    fifo -> queue_tail++;
    if(fifo-> queue_tail == fifo->max) fifo -> queue_tail = 0;
    return 0;
}

pid_t pop_fifo (barber_fifo* fifo){
    if(is_empty(fifo) == 1) return -1;

    fifo -> seat = fifo->queue[fifo -> queue_head];
    fifo-> queue_head++;

    if(fifo-> queue_head == fifo -> max) fifo -> queue_head = 0;
    if(fifo-> queue_head == fifo -> queue_tail) fifo -> queue_head = -1;

    return fifo->seat;
}

long get_time(){
    struct timespec timer;
    if (clock_gettime(CLOCK_MONOTONIC, &timer) == -1) FAILURE_EXIT(1, "PID - %d: Meh, something wrong with getting time by get_time function.\n");
    return timer.tv_nsec;
}



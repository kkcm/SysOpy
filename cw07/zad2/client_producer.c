

//
// Created by Kuba Kołoczek on 10.05.2018.
//

#define _XOPEN_SOURCE

#include <unistd.h>
#include <printf.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/errno.h>
#include <semaphore.h>
#include "barbershop.h"

sem_t *BARBER;
sem_t *FIFO;
sem_t *CLIENTS;
sem_t *SITTING;
volatile int number_of_done_cuts = 0;
sigset_t full_mask;
barber_fifo* barbershop_fifo;



void SIGINT_handler(int signo){
    printf("\nClient_producer - %d: Meh, CTRL + D was clicked. That's the end of our journey.\n", getpid());
    exit(7);
}

void SIGUSR1_handler(int signo){
    number_of_done_cuts++;
    printf("Client - %d: cut number %d done.\n", getpid(), number_of_done_cuts);
}


void clean(){
    if(munmap(barbershop_fifo, sizeof(barbershop_fifo)) == -1) printf("Barber - %d: Oh no, something went wrong with disconnecting from shared_memory, but who cares. Let's move on.\n", getpid());
    else printf("Barber - %d: Oh, the end is coming, I have just disconnected from shared_memory.\n", getpid());

    if(sem_close(BARBER) == -1) printf("Barber - %d: Oh no, something went wrong with closing BARBER semaphore, but who cares. Let's move on.\n", getpid());
    else printf("Barber - %d: Oh, the end is coming, I have just closed BARBER semaphore.\n", getpid());

    if(sem_close(FIFO) == -1) printf("Barber - %d: Oh no, something went wrong with closing FIFO semaphore, but who cares. Let's move on.\n", getpid());
    else printf("Barber - %d: Oh, the end is coming, I have just closed FIFO semaphore.\n", getpid());

    if(sem_close(CLIENTS) == -1) printf("Barber - %d: Oh no, something went wrong with closing CLIENTS semaphore, but who cares. Let's move on.\n", getpid());
    else printf("Barber - %d: Oh, the end is coming, I have just closed CLIENTS semaphore.\n", getpid());

    if(sem_close(SITTING) == -1) printf("Barber - %d: Oh no, something went wrong with closing SITTING semaphore, but who cares. Let's move on.\n", getpid());
    else printf("Barber - %d: Oh, the end is coming, I have just closed SITTING semaphore.\n", getpid());
}

void prepare_fifo(){

    int shared_memory_ID = shm_open(shared_memory_path, O_RDWR, 0666);
    if(shared_memory_ID == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with creating shared_memory.\n");

    void* shared_memory_address_indicator =  mmap(NULL, sizeof(barbershop_fifo), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_ID, 0);
    if(shared_memory_address_indicator == (void*) -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with connecting to shared_memory.\n");

    barbershop_fifo = (struct barber_fifo*) shared_memory_address_indicator;
}

void prepare_semaphores(){
    BARBER = sem_open(barber_path, O_RDWR);
    if(BARBER == SEM_FAILED) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with creating BARBER semaphore.\n" );

    FIFO = sem_open(fifo_path, O_RDWR);
    if(FIFO == SEM_FAILED) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with creating FIFO semaphore.\n" );

    CLIENTS = sem_open(clients_path, O_RDWR);
    if(BARBER == SEM_FAILED) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with creating CLIENTS semaphore.\n" );

    SITTING = sem_open(sitting_path, O_RDWR);
    if(SITTING == SEM_FAILED) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with creating SITTING semaphore.\n" );
}


int find_seat(){
    int barber_status;
    if (sem_getvalue(BARBER, &barber_status) == -1) FAILURE_EXIT(1, "Client - %d: Meh, something went wrong with getting BARBER semaphore value.\n");

    pid_t client_PID = getpid();

    if(barber_status){
        if (push_fifo(barbershop_fifo, client_PID) == -1){
            printf("Client - %d | Time = %ld:Oh no, I didn't find any free seat!\n", getpid(), get_time());
            fflush(stdout);
            return -1;
        } else {
            printf("Client - %d | Time = %ld:Oh yeah, I have just found free seat! Now I have to wait.\n", getpid(), get_time());
            fflush(stdout);
            return 0;
        }
    } else {
        if (sem_post(BARBER) == -1)  FAILURE_EXIT(1, "Client - %d: Meh, something went wrong with awakening BARBER semaphores.\n");
        printf("Client - %d | Time = %ld: I have just awake barber!\n", getpid(), get_time());
        fflush(stdout);

        barbershop_fifo -> seat = client_PID;

        return 1;
    }
}

void go_to_barber(int number_of_cut){
    while(number_of_done_cuts < number_of_cut){
        if (sem_wait(CLIENTS) == -1) FAILURE_EXIT(1, "Client - %d: Meh, something went wrong with taking CLIENTS semaphores.\n");
        if (sem_wait(FIFO) == -1) FAILURE_EXIT(1, "Client - %d: Meh, something went wrong with taking FIFO semaphores.\n");

        int is_cut = find_seat();

        if (sem_post(FIFO) == -1) FAILURE_EXIT(1, "Client - %d: Meh, something went wrong with taking FIFO semaphores.\n");
        if (sem_post(CLIENTS) == -1) FAILURE_EXIT(1, "Client - %d: Meh, something went wrong with releasing CLIENTS semaphores.\n");

        if(is_cut != -1){

//          semaphore_operation.sem_num = SITTING;
//          semaphore_operation.sem_op = 1;
//          if(semop(semaphores_ID, &semaphore_operation, 1) == -1) FAILURE_EXIT(1, "Client - %d: Meh, something went wrong with releasing SITTING semaphore.\n");

            printf("Client - %d | Time = %ld: I'm ready Sir. Let's cut my hair!\n", getpid(), get_time());
            sigsuspend(&full_mask);

//            semaphore_operation.sem_op = -1;
//            if(semop(semaphores_ID, &semaphore_operation, 1) == -1) FAILURE_EXIT(1, "Client - %d: Meh, something went wrong with taking SITTING semaphore.\n");

            printf("Client - %d | Time = %ld: Oh yeah, this is realy good and fast barber!\n", getpid(), get_time());
            fflush(stdout);
        }
    }
}

int main(int argc, char *argv[]){

    if (argc < 3) FAILURE_EXIT(2, "Client_producer - %d: You have to give me number of clients and number of cuts.\n");
    int number_of_client = (int) strtol(argv[1], NULL, 10);
    if (number_of_client < 1) FAILURE_EXIT(2, "Client_producer - %d: Your number of clients have to be bigger than 0, but you gave me %d.\n", number_of_client);
    int number_of_cut = (int) strtol(argv[2], NULL, 10);
    if (number_of_cut < 1) FAILURE_EXIT(2, "Client_producer - %d: Your number of cuts have to be bigger than 0, but you gave me %d.\n", number_of_client);

    if(atexit(clean) == -1) FAILURE_EXIT(2, "Client_producer - %d: Meh, something went wrong in atexit function.\n");
    if(signal(SIGINT, SIGINT_handler) == SIG_ERR) FAILURE_EXIT(2, "Client_producer - %d: Meh, something went wrong with setting handler for SIGINT.\n");
    if(signal(SIGUSR1, SIGUSR1_handler) == SIG_ERR) FAILURE_EXIT(2, "Client_producer - %d: Meh, something went wrong with setting handler for SIGUSR1.\n");


    prepare_fifo();
    prepare_semaphores();

    sigfillset(&full_mask);
    sigdelset(&full_mask, SIGINT);
    sigdelset(&full_mask, SIGUSR1);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    if(sigprocmask(SIG_BLOCK, &mask, NULL) == -1) FAILURE_EXIT(2, "Client_producer - %d: Meh, something went wrong with blocking masked signals.\n")

    for (int i = 0; i < number_of_client; i++){
        pid_t client_id = fork();
        if (client_id == -1) FAILURE_EXIT(2, "Client_producer - %d: Meh, something went wrong with forking client number %d.\n", i);
        if(client_id == 0){
            go_to_barber(number_of_cut);
            return 0;
        }
    }

    printf("Client_producer - %d: Wow, all clients were created, now I'm waiting for ending their processes.\n", getpid());
    while(1){
        wait(NULL);
        if(errno == ECHILD) break;
    }

    printf("Client_producer - %d: That's the end for today. It was good work.\n", getpid());
    return 0;
}


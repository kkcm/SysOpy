//
// Created by Kuba Kołoczek on 10.05.2018.
//

#define _GNU_SOURCE

#include <unistd.h>
#include <printf.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include <unistd.h>
#include <unistd.h>

#include "barbershop.h"

key_t fifo_key;
int semaphores_ID = -1;
int shared_memory_ID = -1;
barber_fifo* barbershop_fifo = NULL;
sigset_t full_mask;


void SIGINT_handler(int signo){
    printf("\nBarber - %d: Meh, CTRL + D was clicked. That's the end of our journey.\n", getpid());
    exit(7);
}

void clean(){
    if(shmdt(barbershop_fifo) == -1) printf("Barber - %d: Oh no, something went wrong with disconnecting from shared_memory, but who cares. Let's move on.\n", getpid());
    else printf("Barber - %d: Oh, the end is coming, I have just disconnected from shared_memory.\n", getpid());

    if(shmctl(shared_memory_ID, IPC_RMID, NULL) == -1) printf("Barber - %d: Oh no, something went wrong with deleting shared_memory, but who cares. Let's move on.\n", getpid());
    else printf("Barber - %d: Oh, the end is coming, I have just deleted shared_memory.\n", getpid());

    if(semctl(semaphores_ID, 2137, IPC_RMID) == -1) printf("Barber - %d: Oh no, something went wrong with deleting semaphores, but who cares. Let's move on.\n", getpid());
    else printf("Barber - %d: Oh, the end is coming, I have just deleted semaphores.\n", getpid());
}

key_t prepare_fifo_key(){
    char* path = getenv("HOME");
    if (path == NULL) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with getting enviromental variable HOME.\n");

    key_t fifo_key = ftok(path, BARBERSHOP_ID);
    if(fifo_key == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with generating fifo_key.\n");

    return fifo_key;
}

void prepare_fifo(key_t fifo_key, int number_of_chair){

    shared_memory_ID = shmget(fifo_key, sizeof(barber_fifo), 0666 | IPC_CREAT | IPC_EXCL);
    if(shared_memory_ID == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with creating shared_memory.\n");

    void* shared_memory_address_indicator =  shmat(shared_memory_ID, NULL, 0);
    if(shared_memory_address_indicator == (void*) -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with connecting to shared_memory.\n");

    barbershop_fifo = (struct barber_fifo*) shared_memory_address_indicator;
    init_fifo(barbershop_fifo, number_of_chair);
}

void prepare_semaphores(key_t fifo_key){
    semaphores_ID = semget(fifo_key, 4, 0666 | IPC_CREAT | IPC_EXCL);
    if(semaphores_ID == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with creating semaphores.\n");

    if(semctl(semaphores_ID, 0, SETVAL, 0) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with setting semaphore.\n" );
    for(int i = 1; i<4; i++){
        if(semctl(semaphores_ID, i, SETVAL, 1) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with setting semaphore.\n" );
    }
}


pid_t get_client_from_waiting_room(struct sembuf *semaphore_operation){
    semaphore_operation -> sem_num = FIFO;
    semaphore_operation -> sem_op = -1;
//    printf("Barber: test7\n");
    if(semop(semaphores_ID, semaphore_operation, 1) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with taking FIFO semaphore.\n");
//    printf("Barber: test7a\n");
    pid_t client_to_cut_pid = barbershop_fifo -> seat;

//    printf("Barber: test8\n");
    semaphore_operation -> sem_op = 1;
    if(semop(semaphores_ID, semaphore_operation, 1) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with releasing FIFO semaphore.\n");

    return client_to_cut_pid;
}

void cut(pid_t client_pid, struct sembuf *semaphore_operation){

    printf("Barber - %d | Time = %ld: Come on! Let's cut your hair Sir - %d.\n", getpid(), get_time(), client_pid);
 //   semaphore_operation -> sem_num = SITTING;
 //   semaphore_operation -> sem_op = -1;
 //   if(semop(semaphores_ID, semaphore_operation, 1) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with taking SITTING semaphore.\n");


    printf("Barber - %d | Time = %ld: Oh, I have the client: %d. I gonna prepare cutting. 3, 2, 1... START!\n", getpid(), get_time(), (int) client_pid);
    fflush(stdout);

    kill(client_pid, SIGUSR1);

    printf("Barber - %d | Time = %ld: Oh, I have done cutting the client: %d. I'm really fast.\n", getpid(), get_time(), (int) client_pid);
    fflush(stdout);

//    semaphore_operation -> sem_op = 1;
//    if(semop(semaphores_ID, semaphore_operation, 1) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with releasing SITTING semaphore.\n");

}


void barber_job(){
    struct sembuf semaphore_operation;
    semaphore_operation.sem_flg = 0;
//    printf("Barber: test1\n");

    // tutaj info o spaniu
    while(1){
        semaphore_operation.sem_num = BARBER;
        semaphore_operation.sem_op = -1;
//        printf("Barber: test2\n");
        if(semop(semaphores_ID, &semaphore_operation, 1) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with taking BARBER semaphore.\n");

        printf("Barber - %d | Time = %ld: What's going on?! Who the hell woke me up?! Oh sorry Sir, I have really hard night because of operating systems project.\n", getpid(), get_time());

        pid_t somebody_to_cut = get_client_from_waiting_room(&semaphore_operation);

        cut(somebody_to_cut, &semaphore_operation);
//        printf("Barber: test3\n");
        while(1){
            semaphore_operation.sem_num = FIFO;
            semaphore_operation.sem_op = -1;
            if(semop(semaphores_ID, &semaphore_operation, 1) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with taking FIFO semaphore.\n");
            somebody_to_cut = pop_fifo(barbershop_fifo);

//            printf("Barber: test4\n");
            if(somebody_to_cut == -1) {
                printf("Barber - %d | Time = %ld: Oh no, nobody want to be cut. I'm going to sleep...\n", getpid(), get_time());
                fflush(stdout);
                semaphore_operation.sem_num = BARBER;
                semaphore_operation.sem_op = -1;
                if(semop(semaphores_ID, &semaphore_operation, 1) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with taking BARBER semaphore.\n");
//                printf("Barber: test5\n");
                semaphore_operation.sem_num = FIFO;
                semaphore_operation.sem_op = 1;
                if(semop(semaphores_ID, &semaphore_operation, 1) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with releasing FIFO semaphore.\n");
                break;
            } else {
                semaphore_operation.sem_op = 1;
                if(semop(semaphores_ID, &semaphore_operation, 1) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with releasing FIFO semaphore.\n");

                cut(somebody_to_cut, &semaphore_operation);
//                printf("Barber: test6\n");
            }
        }

    }
}




int main(int argc, char *argv[]){

    if (argc < 2) FAILURE_EXIT(1, "Barber - %d: You have to give me number of chairs in waiting room.\n");
    int number_of_chair = (int) strtol(argv[1], NULL, 10);
    if (number_of_chair < 1 || number_of_chair > MAX_CHAIRS_NUMBER) FAILURE_EXIT(1, "\"Barber - %d: Your number of chair have to be bigger than 0 and smaller from %d, but you gave me %d.\n", MAX_CHAIRS_NUMBER+1, number_of_chair);

    if(atexit(clean) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong in atexit function.\n");
    if(signal(SIGINT, SIGINT_handler) == SIG_ERR) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with setting handler for SIGINT.\n");

    fifo_key = prepare_fifo_key();

    prepare_fifo(fifo_key,number_of_chair);
    prepare_semaphores(fifo_key);

    barber_job();

    printf("Barber - %d: That's the end for today. It was good work.\n", getpid());
    return 0;
}


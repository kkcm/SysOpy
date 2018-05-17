
// Created by Kuba Kołoczek on 10.05.2018.
//

#define _GNU_SOURCE

#include "barbershop.h"

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
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>


sem_t *BARBER;
sem_t *FIFO;
sem_t *CLIENTS;
sem_t *SITTING;
barber_fifo* barbershop_fifo = NULL;



void SIGINT_handler(int signo){
    printf("\nBarber - %d: Meh, CTRL + D was clicked. That's the end of our journey.\n", getpid());
    exit(7);
}

void clean(){
    if(munmap(barbershop_fifo, sizeof(barbershop_fifo)) == -1) printf("Barber - %d: Oh no, something went wrong with disconnecting from shared_memory, but who cares. Let's move on.\n", getpid());
    else printf("Barber - %d: Oh, the end is coming, I have just disconnected from shared_memory.\n", getpid());

    if(shm_unlink(shared_memory_path) == -1) printf("Barber - %d: Oh no, something went wrong with deleting shared_memory, but who cares. Let's move on.\n", getpid());
    else printf("Barber - %d: Oh, the end is coming, I have just deleted shared_memory.\n", getpid());

    if(sem_close(BARBER) == -1) printf("Barber - %d: Oh no, something went wrong with closing BARBER semaphore, but who cares. Let's move on.\n", getpid());
    else printf("Barber - %d: Oh, the end is coming, I have just closed BARBER semaphore.\n", getpid());

    if(sem_close(FIFO) == -1) printf("Barber - %d: Oh no, something went wrong with closing FIFO semaphore, but who cares. Let's move on.\n", getpid());
    else printf("Barber - %d: Oh, the end is coming, I have just closed FIFO semaphore.\n", getpid());

    if(sem_close(CLIENTS) == -1) printf("Barber - %d: Oh no, something went wrong with closing CLIENTS semaphore, but who cares. Let's move on.\n", getpid());
    else printf("Barber - %d: Oh, the end is coming, I have just closed CLIENTS semaphore.\n", getpid());

    if(sem_close(SITTING) == -1) printf("Barber - %d: Oh no, something went wrong with closing SITTING semaphore, but who cares. Let's move on.\n", getpid());
    else printf("Barber - %d: Oh, the end is coming, I have just closed SITTING semaphore.\n", getpid());
}

void prepare_fifo(int number_of_chair){

    int shared_memory_ID = shm_open(shared_memory_path, O_CREAT | O_EXCL | O_RDWR, 0666);
    if(shared_memory_ID ==  -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with creating shared_memory.\n");

    if(ftruncate(shared_memory_ID, sizeof(barbershop_fifo)) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with truncating shared_memory.\n");

    void* shared_memory_address_indicator =  mmap(NULL, sizeof(barbershop_fifo), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_ID,  0);
    if(shared_memory_address_indicator == (void*) -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with connecting to shared_memory.\n");

    barbershop_fifo = (struct barber_fifo*) shared_memory_address_indicator;
    init_fifo(barbershop_fifo, number_of_chair);
}

void prepare_semaphores(){
    
    FIFO = sem_open(fifo_path, O_CREAT | O_EXCL | O_RDWR, 0666, 1);
	printf("%s", strerror(errno));
    if(FIFO == SEM_FAILED) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with creating FIFO semaphore.\n" );

    CLIENTS = sem_open(clients_path, O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    if(CLIENTS == SEM_FAILED) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with creating CLIENTS semaphore.\n" );

    BARBER = sem_open(barber_path, O_CREAT | O_EXCL | O_RDWR, 0666, 0);
    if(BARBER == SEM_FAILED) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with creating BARBER semaphore.\n" );


    SITTING = sem_open(sitting_path, O_CREAT | O_EXCL | O_RDWR, 0666, 0);
    if(SITTING == SEM_FAILED) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with creating SITTING semaphore.\n" );
}


pid_t get_client_from_waiting_room(){
    if(sem_wait(FIFO) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with taking FIFO semaphore.\n");

    pid_t client_to_cut_pid = barbershop_fifo -> seat;

    if(sem_post(FIFO) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with releasing FIFO semaphore.\n");

    return client_to_cut_pid;
}

void cut(pid_t client_pid){

    printf("Barber - %d | Time = %ld: Come on! Let's cut your hair Sir - %d.\n", getpid(), get_time(), client_pid);
//    semaphore_operation -> sem_num = SITTING;
//    semaphore_operation -> sem_op = -1;
//    if(semop(semaphores_ID, semaphore_operation, 1) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with taking SITTING semaphore.\n");


    printf("Barber - %d | Time = %ld: Oh, I have the client: %d. I gonna prepare cutting. 3, 2, 1... START!\n", getpid(), get_time(), (int) client_pid);
    fflush(stdout);

    kill(client_pid, SIGUSR1);

    printf("Barber - %d | Time = %ld: Oh, I have done cutting the client: %d. I'm really fast.\n", getpid(), get_time(), (int) client_pid);
    fflush(stdout);

//    semaphore_operation -> sem_op = 1;
//    if(semop(semaphores_ID, semaphore_operation, 1) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with releasing SITTING semaphore.\n");

}


void barber_job(){
    while(1){
        if (sem_wait(BARBER) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with taking BARBER semaphore.\n");
        if (sem_post(BARBER) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with setting BARBER semaphore as awaken.\n");

        printf("Barber - %d | Time = %ld: What's going on?! Who the hell woke me up?! Oh sorry Sir, I have really hard night because of operating systems project.\n", getpid(), get_time());

        pid_t somebody_to_cut = get_client_from_waiting_room();

        cut(somebody_to_cut);

        while(1){
            if (sem_wait(FIFO) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with taking FIFO semaphore.\n");
            somebody_to_cut = pop_fifo(barbershop_fifo);

            if(somebody_to_cut == -1) {
                printf("Barber - %d | Time = %ld: Oh no, nobody want to be cut. I'm going to sleep...\n", getpid(), get_time());
                if (sem_wait(BARBER) == -1)  FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with taking BARBER semaphore.\n");

                if (sem_post(FIFO) == -1)  FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with releasing FIFO semaphore.\n");
                break;
            } else {
                if (sem_post(FIFO) == -1) FAILURE_EXIT(1, "Barber - %d: Meh, something went wrong with releasing FIFO semaphore.\n");

                cut(somebody_to_cut);

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


    prepare_fifo(number_of_chair);
    prepare_semaphores();

    barber_job();

    printf("Barber - %d: That's the end for today. It was good work.\n", getpid());
    return 0;
}


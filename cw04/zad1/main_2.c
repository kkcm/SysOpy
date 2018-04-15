//
// Created by Kuba Kołoczek on 13.04.2018.
//

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

int paused = 0;


void SIGTSTP_catch(int signal_number){
    paused = paused == 0 ? 1 : 0;
    if (paused) {
        printf("\nI recived SIGNAL %d. I'm waiting for CTRL+Z to continuation or CTRL+C to termination program.\n", signal_number);
    }
}

void SIGINT_catch (int signal_number){
    printf("\nI recived SIGNAL %d. So I gonna terminate this program.\n", signal_number);
    exit(EXIT_SUCCESS);
}

void print_actual_time(){
    time_t actual_time;
    char buffer[30];

    actual_time = time(NULL);
    strftime(buffer, sizeof(buffer), "%H:%M:%S", localtime(&actual_time));
    printf("\n%s\n", buffer);
}


int main(int argc, char** argv) {

    struct sigaction act;
    act.sa_handler = SIGTSTP_catch;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);

    sigaction(SIGTSTP, &act, NULL);
    signal(SIGINT, SIGINT_catch);

    while(1){
        int status;
        if (!paused) {
            pid_t pid;
            pid = fork();
            if (pid < 0){
                perror("\nSomething went wrong with fork().\n");
            } else if ( pid == 0 ){
                status = execvp(argv[1], argv);
                exit(EXIT_SUCCESS);
            }
        }
        wait(&status);
    }

    return 0;
}


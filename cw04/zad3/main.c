#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


volatile int type;
volatile int signals_sent_to_child;
volatile int signals_received_by_child;
volatile int signals_received_from_child;

volatile pid_t  child;


void print_statistics(){
    printf("Signals sent to child by mother: %d.\n", signals_sent_to_child);
    printf("Signals received by child from mother: %d.\n", signals_received_by_child);
    printf("Signals received by mother from child: %d.\n", signals_received_from_child);
}

void mother_catcher(int signum, siginfo_t* informations, void* context){
    if (signum == SIGINT){
        printf("Mother received SIGINT. So I gonna terminate child and end program.\n");
        kill(child, SIGUSR2);
        print_statistics();
        exit(EXIT_SUCCESS);
    }

    if (informations -> si_pid != child) {
        printf("Mother received signal not from child. I gonna continue.\n");
        return;
    }

    if (type == 3 && signum == SIGRTMIN){
        signals_received_from_child++;
        printf("Mother received %d. SIGRTMIN signal from child.\n", signals_received_from_child);
    } else if ((type == 3 || type == 2) && signum == SIGUSR1){
        signals_received_from_child++;
        printf("Mother received %d. SIGUSR1 signal from child.\n", signals_received_from_child);
    } else {
        printf("Weird, child send to mother other signal than SIGUSR1 or SIGRTMIN, but I gonna cotinue.\n");
    }
}

void mother_process(int l){
    struct sigaction action;
    action.sa_sigaction = mother_catcher;
    action.sa_flags = SA_SIGINFO;

    if (sigaction(SIGINT, &action, NULL) == -1){
        printf("Something went wrong with sigaction() and SIGINT.\n");
        exit(EXIT_FAILURE);
    }

    if (type == 3) {
        if (sigaction(SIGRTMIN, &action, NULL) == -1){
            printf("Something went wrong with sigaction() and SIGRTMIN.\n");
            exit(EXIT_FAILURE);
        }
    } else {
        if (sigaction(SIGUSR1, &action, NULL) == -1){
            printf("Something went wrong with sigaction() and SIGUSR1.\n");
            exit(EXIT_FAILURE);
        }
    }

    if (type == 3){
        for (; signals_sent_to_child < l; signals_sent_to_child++){
            printf("Mother sending %d. SIGRTMIN to child.\n", signals_sent_to_child);
            kill(child, SIGRTMIN);
        }

        signals_sent_to_child++;
        printf("Mother sending %d. SIGRTMAX to child.\n", signals_sent_to_child);
        kill(child, SIGRTMAX);
    } else {
        for(; signals_sent_to_child < l; signals_sent_to_child++){
            printf("Mother sending %d. SIGUSR1 to child.\n", signals_sent_to_child);
            kill(child, SIGUSR1);

            if (type == 2){
                printf("I'm waiting for my child's respods.\n");
                pause();
            }
        }

        signals_sent_to_child++;
        printf("Mother sending %d. SIGUSR2 to child.\n", signals_sent_to_child);
        kill(child, SIGUSR2);
    }

    int status = 0;
    waitpid(child, &status, 0);
    if (!WIFEXITED(status)){
        printf("Something went wrong with termination of child.\n");
        exit(EXIT_FAILURE);
    }

}

void child_catcher(int signum, siginfo_t* informations, void* context){
    if (informations -> si_pid != getpid()) {
        printf("Child received signal not from mother. I gonna continue.\n");
        return;
    }

    if (type == 3){
        if (signum == SIGRTMIN){
            signals_received_by_child++;
            kill(SIGRTMIN, getpid());
            printf("Child received %d. SIGRTMIN and sent it back.\n", signals_received_by_child);
        } else if (signum == SIGRTMAX){
            signals_received_by_child++;
            printf("Child received SIGRTMAX. So I gonna terminate program.");
            exit(EXIT_SUCCESS);
        } else {
            printf("Weird, mother send to child other signal than SIGRTMIN or SIGRTMAX, but I gonna cotinue.\n");
        }
    } else {
        if (signum == SIGUSR1){
            signals_received_by_child++;
            kill(SIGUSR1, getpid());
            printf("Child received %d. SIGUSR1 and sent it back.\n", signals_received_by_child);
        } else if (signum == SIGUSR2){
            signals_received_by_child++;
            printf("Child received SIGUSR2. So I gonna terminate program.");
            exit(EXIT_SUCCESS);
        } else {
            printf("Weird, mother send to child other signal than SIGUSR1 or SIGUSR2, but I gonna cotinue.\n");
        }
    }

}

void child_process(){
    struct sigaction action;
    action.sa_sigaction = child_catcher;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);

    sigset_t mask;
    sigfillset(&mask);

    if (type == 3){
        sigdelset(&mask, SIGRTMAX);
        sigdelset(&mask, SIGRTMIN);

        if (sigaction(SIGRTMAX, &action, NULL) == -1){
            printf("Something went wrong with sigaction() and SIGRTMAX.\n");
            exit(EXIT_FAILURE);
        }
        if (sigaction(SIGRTMIN, &action, NULL) == -1){
            printf("Something went wrong with sigaction() and SIGRTMIN.\n");
            exit(EXIT_FAILURE);
        }
    } else {
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGUSR2);

        if (sigaction(SIGUSR1, &action, NULL) == -1){
            printf("Something went wrong with sigaction() and SIGUSR1.\n");
            exit(EXIT_FAILURE);
        }
        if (sigaction(SIGUSR2, &action, NULL) == -1){
            printf("Something went wrong with sigaction() and SIGUSR2.\n");
            exit(EXIT_FAILURE);
        }
    }

    sigprocmask(SIG_SETMASK, &mask, NULL);

    while(1){
        sleep(1);
    }
}



int main(int argc, char** argv) {

    if (argc < 3){
        printf("You gave me wrong number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    int l;

    l = (int) strtol(argv[1], '\0', 10);
    type = (int) strtol(argv[2], '\0', 10);

    if (l < 1) {
        printf("You gave me wrong first argument - L > 0.\n");
        exit(EXIT_FAILURE);
    }

    if ( type < 1 || type > 3) {
        printf("You gave me wrong second argument - 0 < Type < 4.\n");
        exit(EXIT_FAILURE);
    }

    child = fork();

    if (child < 0){
        printf("Something went wrong with fork().\n");
        exit(EXIT_FAILURE);
    } else if (!child) {
        child_process();
    } else {
        mother_process(l);
    }

    print_statistics();

    return 0;
}
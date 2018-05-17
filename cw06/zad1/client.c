#define _XOPEN_SOURCE
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <memory.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "communication_protocol.h"


int public_queue_id;
int private_queue_id;
int client_id;

void SIGINT_handler(int signo){
    printf("Ups, CTRL + D was clicked. That's the end of our .\n");
    exit(1);
}

void register_rq(MSG* message, key_t private_key){
    sprintf(message -> text_msg, "%d", private_key);
    message -> type = REGISTER;
    message -> sender_PID = getpid();

    if(msgsnd(public_queue_id, message, MSG_SIZE, 0) == -1){
        printf("Ups, something went wrong with REGISTER request.\n");
        exit(EXIT_FAILURE);
    }
    if(msgrcv(private_queue_id, message, MSG_SIZE, 0, 0) == -1) {
        printf("Ups, something went wrong with catching REGISTER response.\n");
        exit(EXIT_FAILURE);
    }
    if(sscanf(message -> text_msg, "%d", &client_id) < 1){
        printf("Ups, something went wrong with reading REGISTER response.\n");
        exit(EXIT_FAILURE);
    }
    if(client_id < 0) {
        printf("Ups, server cannot have more clients! Try later.\n");
        exit(EXIT_FAILURE);
    }

    printf("Yeah, I have just registered! My id number is %d!\n", client_id);

}

void mirror_rq(MSG* message){
    message -> type = MIRROR;
    if(fgets(message -> text_msg, MAX_TEXT_MSG_LENGTH, stdin) == NULL){
        printf("Ups, something went wrong with getting stdin.\n");
        exit(EXIT_FAILURE);
    }
    if(msgsnd(public_queue_id, message, MSG_SIZE, 0) == -1){
        printf("Ups, something went wrong with MIRROR request.\n");
        exit(EXIT_FAILURE);
    }
    if(msgrcv(private_queue_id, message, MSG_SIZE, 0, 0) == -1) {
        printf("Ups, something went wrong with catching MIRROR response.\n");
        exit(EXIT_FAILURE);
    }
    print_msg(message);
}

void calc_rq(MSG* message){
    message -> type = CALC;
    if(fgets(message -> text_msg, MAX_TEXT_MSG_LENGTH, stdin) == NULL){
        printf("Ups, something went wrong with getting stdin.\n");
        exit(EXIT_FAILURE);
    }
    if(msgsnd(public_queue_id, message, MSG_SIZE, 0) == -1){
        printf("Ups, something went wrong with CALC request.\n");
        exit(EXIT_FAILURE);
    }
    if(msgrcv(private_queue_id, message, MSG_SIZE, 0, 0) == -1) {
        printf("Ups, something went wrong with catching CALC response.\n");
        exit(EXIT_FAILURE);
    }
    print_msg(message);
}

void time_rq(MSG* message){
    message -> type = TIME;
    if(msgsnd(public_queue_id, message, MSG_SIZE, 0) == -1){
        printf("Ups, something went wrong with TIME request.\n");
        exit(EXIT_FAILURE);
    }
    if(msgrcv(private_queue_id, message, MSG_SIZE, 0, 0) == -1) {
        printf("Ups, something went wrong with catching TIME response.\n");
        exit(EXIT_FAILURE);
    }
    print_msg(message);
}

void end_rq(MSG* message){
    message -> type = END;

    if(msgsnd(public_queue_id, message, MSG_SIZE, 0) == -1){
        printf("Ups, something went wrong with END request.\n");
        exit(EXIT_FAILURE);
    }
}

void stop_rq(MSG* message){
    message -> type = STOP;
    if(msgsnd(public_queue_id, message, MSG_SIZE, 0) == -1) {
        printf("Ups, something went wrong with STOP request.\n");
        exit(EXIT_FAILURE);
    }
}

void command_requestor(MSG* message, char* cmd){
    if(strcmp(cmd, "mirror") == 0){
        mirror_rq(message);
    } else if(strcmp(cmd, "calc") == 0){
        calc_rq(message);
    } else if(strcmp(cmd, "time") == 0){
        time_rq(message);
    } else if(strcmp(cmd, "end") == 0){
        end_rq(message);
    } else if(strcmp(cmd, "stop") == 0){
        stop_rq(message);
    } else
        printf("Ups, wrong command! Try again!\n");
}

void rm_queue(){
    if(private_queue_id <= -1) return;

    if(msgctl(public_queue_id, IPC_RMID, NULL) == -1){
        printf("Ups, something went wrong with deleting queue.\n");
        exit(EXIT_FAILURE);
    }

    printf("Yeah, client queue deleted successfully.\n");
}


int main(){

    if(atexit(rm_queue) == -1) {
        printf("Ups, something went wrong with registering atexit().\n");
        exit(EXIT_FAILURE);
    }

    if(signal(SIGINT, SIGINT_handler) == SIG_ERR) {
        printf("Ups, something went wrong with setting handler for SIGINT.\n");
        exit(EXIT_FAILURE);
    }

    char* path = getenv("HOME");
    if (path == NULL){
        printf("Ups, something went wrong with getting enviromental variable HOME.\n");
        exit(EXIT_FAILURE);
    }

    key_t public_key = ftok(path, SERVER_KEY);
    if(public_key == -1) {
        printf("Ups, something went wrong with generating public_key.\n");
        exit(EXIT_FAILURE);
    }

    public_queue_id = msgget(public_key, IPC_CREAT | 0666);
    if(public_queue_id == -1) {
        printf("Ups, something went wrong with creating public_queue.\n");
        exit(EXIT_FAILURE);
    }

    key_t private_key = ftok(path, getpid());
    if(private_key == -1) {
        printf("Ups, something went wrong with generating private_key.\n");
        exit(EXIT_FAILURE);
    }

    private_queue_id = msgget(private_key, IPC_CREAT | IPC_EXCL | 0666);
    if(private_queue_id == -1) {
        printf("Ups, something went wrong with creating private_queue.\n");
        exit(EXIT_FAILURE);
    }

    printf("Hi! I'm client and my PID is %d.\n", getpid());


    MSG message;
    register_rq (&message, private_key);
    char cmd[MAX_TEXT_MSG_LENGTH];

    while(1){
        printf("Please, type your request:");
        if (fgets(cmd, MAX_TEXT_MSG_LENGTH, stdin) == NULL){
            printf("Ups, something went wrong with reading your command. Try again!\n");
            continue;
        }

        message.sender_PID = getpid();
        int n = (int) strlen(cmd);
        if(cmd[n-1] == '\n') cmd[n-1] = 0;

        command_requestor(&message, cmd);
    }
}










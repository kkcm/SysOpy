#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "communication_protocol.h"

int clients_data[MAX_CLIENTS][2];
int client_counter = 0;
int active_server = 1;
int public_queue = -1;


void SIGINT_handler(int signo){
    printf("Ups, CTRL + D was clicked. That's the end of our adventure.\n");
    exit(1);
}

void init_clients_data(){
    for(int i=0; i < MAX_CLIENTS; i++){
        clients_data[i][0] = -1;
        clients_data[i][1] = -1;
    }
}

int get_client_queue_id(pid_t client_pid){
    for(int i=0; i < MAX_CLIENTS; i++){
        if(clients_data[i][0] == client_pid)
            return clients_data[i][1];
    }
    printf("Ups, something went wrong with finding client with this client_pid.\n");
    exit(EXIT_FAILURE);
}

int prepare_message(MSG* message){
    int client_qid = get_client_queue_id(message -> sender_PID);
    if(client_qid == -1) {
        printf("Ups, something went wrong with finding client with this client_pid.\n");
        exit(EXIT_FAILURE);
    }

    message -> type = message -> sender_PID;
    message -> sender_PID = getpid();

    return client_qid;
}

void mirror_executor(MSG* message){
    print_msg(message);
    int client_queue_id = prepare_message(message);

    int message_length = (int) strlen(message->text_msg);
    if(message->text_msg[message_length-1] == '\n')
        message_length -= 1;

    for (int i=0; i < message_length / 2; i++){
        char buff = message -> text_msg[i];
        message -> text_msg[i] = message -> text_msg[message_length - i - 1];
        message -> text_msg[message_length - i - 1] = buff;
    }

    if(msgsnd(client_queue_id, message, MSG_SIZE, 0) == -1){
        printf("Ups, something went wrong with MIRROR responding.\n");
        exit(EXIT_FAILURE);
    }
}

void calc_executor(MSG* message){
    print_msg(message);
    int client_queue_id = prepare_message(message);

    char cmd[MAX_TEXT_MSG_LENGTH + 12];
    sprintf(cmd, "echo '%s' | bc", message -> text_msg);
    FILE* calc = popen(cmd, "r");
    fgets(message -> text_msg, MAX_TEXT_MSG_LENGTH, calc);
    pclose(calc);

    if(msgsnd(client_queue_id, message, MSG_SIZE, 0) == -1){
        printf("Ups, something went wrong with CALC responding.\n");
        exit(EXIT_FAILURE);
    }
}

void time_executor(MSG* message){
    print_msg(message);
    int client_queue_id = prepare_message(message);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(message -> text_msg, "Actual time: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    if(msgsnd(client_queue_id, message, MSG_SIZE, 0) == -1){
        printf("Ups, something went wrong with TIME responding.\n");
        exit(EXIT_FAILURE);
    }
}

void end_executor(){
    active_server = 0;
}

void delete_client_from_clients_data(pid_t client_PID, key_t client_queue_id){
    for(int i=0; i<MAX_CLIENTS; i++){
        if(clients_data[i][0] == client_PID && clients_data[i][1] == client_queue_id){
            clients_data[client_counter][0] = -1;
            clients_data[client_counter][1] = -1;
            return;
        }
    }
    printf("Ups, something went wrong with deleting clients from clients_data.\n");
    exit(EXIT_FAILURE);
}

void stop_executor(MSG* message){
    print_msg(message);

    int client_queue_id = get_client_queue_id(message -> sender_PID);
    delete_client_from_clients_data(message -> sender_PID, client_queue_id);
    client_counter--;
}

void add_client_to_clients_data(pid_t client_PID, key_t client_queue_id){
    for(int i=0; i<MAX_CLIENTS; i++){
        if(clients_data[i][0] == -1 && clients_data[i][1] == -1){
            clients_data[client_counter][0] = client_PID;
            clients_data[client_counter][1] = client_queue_id;
            return;
        }
    }
    printf("Ups, something went wrong with adding clients to clients_data.\n");
    exit(EXIT_FAILURE);
}

void register_executor(MSG* message){


    key_t client_queue_key;
    if(sscanf(message -> text_msg, "%d", &client_queue_key) == -1){
        printf("Ups, something went wrong with reading client_queue_key.\n");
        exit(EXIT_FAILURE);
    };

    int client_queue_id = msgget(client_queue_key, 0);
    if(client_queue_id == -1) {
        printf("Ups, something went wrong with reading client_queue_id.\n");
        exit(EXIT_FAILURE);
    }

    pid_t client_PID = message -> sender_PID;
    message -> sender_PID = getpid();
    message -> type = REGISTER;

    if(client_counter < MAX_CLIENTS){
        add_client_to_clients_data(client_PID, client_queue_id);
        sprintf(message->text_msg, "%d", client_counter);
        client_counter++;
    } else {
        sprintf(message -> text_msg, "%d", -1);
        printf("Maximum amount of clients reached!\n");
    }


    if(msgsnd(client_queue_id, message, MSG_SIZE, 0) == -1){
        printf("Ups, something went wrong with REGISTER responding.\n");
        exit(EXIT_FAILURE);
    }

}


void public_queue_executor(struct MSG* message){
    if(message == NULL) return;
    switch(message -> type){
        case MIRROR:
            mirror_executor(message);
            break;
        case CALC:
            calc_executor(message);
            break;
        case TIME:
            time_executor(message);
            break;
        case END:
            end_executor();
            break;
        case STOP:
            stop_executor(message);
            break;
        case REGISTER:
            register_executor(message);
            break;
        default:
            break;
    }
}


void rm_queue(){
    if(msgctl(public_queue, IPC_RMID, NULL) == -1){
        printf("Ups, something went wrong with deleting queue.\n");
        exit(EXIT_FAILURE);
    }
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

    public_queue = msgget(public_key, IPC_CREAT | IPC_EXCL | 0666);
    if(public_queue == -1) {
        printf("Ups, something went wrong with creating public_queue.\n");
        exit(EXIT_FAILURE);
    }

    struct msqid_ds current_state;
    MSG message;
    init_clients_data();

    printf("Hi! I'm server and I'm running right.\n");

    while(1){
        if(active_server) {
            if(msgctl(public_queue, IPC_STAT, &current_state) == -1){
                printf("Ups, something went wrong with coping information from public_queue.\n");
                exit(EXIT_FAILURE);
            }
            if(!current_state.msg_qnum) continue;
        } else break;
        if(msgrcv(public_queue, &message, MSG_SIZE, 0, 0) == -1){
            printf("Ups, something went wrong with receiving message from public_queue.\n");
            exit(EXIT_FAILURE);
        }
        public_queue_executor(&message);
    }
    printf("That's the end for tooday. It was good work.\n");
    return 0;
}

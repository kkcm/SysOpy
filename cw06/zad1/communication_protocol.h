//
// Created by Kuba Kołoczek on 26.04.2018.
//

#ifndef COMMUNICATION_PROTOCOL_H_
#define COMMUNICATION_PROTOCOL_H_

#define MAX_CLIENTS 5
#define MAX_TEXT_MSG_LENGTH 256
#define SERVER_KEY 107

typedef struct MSG {
    long type;
    pid_t sender_PID;
    char text_msg [MAX_TEXT_MSG_LENGTH];
} MSG;

const size_t MSG_SIZE = sizeof(MSG) - sizeof(long);

void print_msg(MSG* msg){
    printf("Message from PID - %d | TYPE -  %li | TEXT - %s.\n", msg -> sender_PID, msg -> type, msg -> text_msg);
}

enum communicate{
    MIRROR = 1,
    CALC = 2,
    TIME = 3,
    END = 4,
    STOP = 5,
    REGISTER = 6
};

#endif //COMMUNICATION_PROTOCOL_H

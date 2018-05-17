
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_NUMBER_OF_ARGS 32
#define MAX_ARG_LENGTH 16
#define MAX_LINE_LENGTH 256
#define MAX_NUMBER_OF_COMMANDS 50

char** parse_task_from_file (char* line){
        char** result = calloc(MAX_NUMBER_OF_ARGS, sizeof(char*));
        char* arg_buffor;

        result[0] = calloc(MAX_ARG_LENGTH, sizeof(char));
        arg_buffor = strtok(line, " \n\t");
        strcpy(result[0], arg_buffor);

        int i = 1;
        arg_buffor = strtok(NULL, " \n\t");
        while(arg_buffor && i < MAX_NUMBER_OF_ARGS){
            result[i] = calloc(MAX_ARG_LENGTH, sizeof(char));
            strcpy(result[i], arg_buffor);
            arg_buffor = strtok(NULL, " \n\t");
            i++;
        }
        result[i] = NULL;

        return result;
    }

int exec_line(FILE* file, char* line_buffor, int counter){
    if (!fgets(line_buffor, MAX_LINE_LENGTH, file)) {
        printf("There is no more line or something went wrong with getting line.\n");
        return 1;
    }

    pid_t pid = fork();
    if(pid == 0) {
        int command_number = 0;
        int pipes[2][2];
        char *commands[MAX_NUMBER_OF_COMMANDS];

        commands[command_number] = strtok(line_buffor, "|");
        command_number++;

        while ((commands[command_number] = strtok(NULL, "|")) != NULL){
            command_number++;
        }

        int i;
        for (i = 0; i < command_number; i++){
            if (i > 0){
                close(pipes[i % 2][0]);
                close(pipes[i % 2][1]);
            }

            if (pipe(pipes[i % 2]) == -1){
                printf("Ups, something went wrong with creating pipe.\n");
                exit(1);
            }

            pid_t child_process = fork();

            if (child_process < 0){
                printf("Something went wrong with fork.\n");
                exit(1);
            }

            if (child_process == 0){
                char** parsed_line;
                parsed_line = parse_task_from_file(commands[i]);

                if ( i != command_number - 1){
                    close((pipes[i % 2][0]));
                    if(dup2(pipes[i % 2][1], STDOUT_FILENO) < 0){
                        printf("Ups, something went wrong with dup2.\n");
                        exit(1);
                    }
                }

                if (i != 0){
                    close(pipes[(i + 1) % 2][1]);
                    if(dup2(pipes[(i + 1) % 2][0], STDIN_FILENO) < 0){
                        printf("Ups, something went wrong with dup2.\n");
                        exit(1);
                    }
                }

                int new_exec;
                new_exec = execvp(parsed_line[0], parsed_line);

                if(new_exec==-1){
                    printf ("Ups, something wrong with new_exec.\n");
                    exit(1);
                }
                exit(1);
            }
        }

        close(pipes[i % 2][0]);
        close(pipes[i % 2][1]);
        wait(NULL);

        exit (0);
    }
    int status;
    wait(&status);
    if (status) {
        printf( "Ups, something went wrong with executing.\n");
        return 1;
    }
    return 0;
}

int main(int argc, char** argv) {

    if(argc < 2) {
        printf("Ups, you did't write enough argument.\n");
        return 1;
    }

    FILE* file_with_tasks = fopen(argv[1], "r");
    if(!file_with_tasks){
        printf("Ups, something went wrong with opening file.\n");
        return 1;
    }

    char* line_buffor = calloc(MAX_LINE_LENGTH, sizeof(char));

    int line_counter = 1;
    while(!exec_line(file_with_tasks, line_buffor, line_counter)){
        line_counter++;
    }
    fclose(file_with_tasks);
    free(line_buffor);

    return 0;
}
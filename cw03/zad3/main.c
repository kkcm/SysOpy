#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_NUMBER_OF_ARGS 32
#define MAX_ARG_LENGTH 16
#define MAX_LINE_LENGTH 256

void display_usage_time (struct rusage* start_time, struct rusage* end_time){
    struct rusage usage_time;

    timersub(&(end_time -> ru_stime), &(start_time -> ru_stime), &(usage_time.ru_stime));
    timersub(&(end_time -> ru_utime), &(start_time -> ru_utime), &(usage_time.ru_utime));

    printf("User CPU time used: %ld.%06d seconds.\n", usage_time.ru_utime.tv_sec, usage_time.ru_utime.tv_usec);
    printf("System CPU time used: %ld.%06d seconds.\n", usage_time.ru_stime.tv_sec, usage_time.ru_stime.tv_usec);
}

int set_limits(char* time_limit, char* memory_limit){
    long long int time_limit_int = strtol(time_limit, NULL, 10);
    long long int memory_limit_int_in_bytes = strtol(memory_limit, NULL, 10) * 1024 * 1024;

    struct rlimit cpu_time_rlimit;
    struct rlimit memory_rlimit;

    cpu_time_rlimit.rlim_cur = (rlim_t) time_limit_int;
    cpu_time_rlimit.rlim_max = (rlim_t) time_limit_int;

    memory_rlimit.rlim_cur = (rlim_t) memory_limit_int_in_bytes;
    memory_rlimit.rlim_max = (rlim_t) memory_limit_int_in_bytes;

    if (setrlimit(RLIMIT_CPU, &cpu_time_rlimit) != 0){
        printf("Ups, something went wrong with setting time limits.\n");
        return 1;
    }

    if (setrlimit(RLIMIT_AS, &memory_rlimit) != 0){
        printf("Ups, something went wrong with setting memory limits.\n");
        return 1;
    }
    return 0;
}

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

int exec_line(FILE* file, char* line_buffor, int counter, char* time_limit, char* memory_limit){
    if (!fgets(line_buffor, MAX_LINE_LENGTH, file)) {
        printf("\n\nThere is no more line or something went wrong with getting line.\n");
        return 1;
    }

    char** parsed_line;

    parsed_line = parse_task_from_file(line_buffor);


    int new_process;
    struct rusage start_time;
    struct rusage end_time;

    if(getrusage(RUSAGE_CHILDREN, &start_time)){
        printf("Something went wrong with reading time.\n");
        exit(1);
    }

    new_process = fork();

    if(new_process > 0){
        int status;
        wait(&status);

        if(getrusage(RUSAGE_CHILDREN, &end_time)){
            printf("Something went wrong with reading time.\n");
            exit(1);
        }

        display_usage_time(&start_time, &end_time);

        if (status != 0) {
            printf("\nSomething went wrong with %s command in line %d.\n", parsed_line[0], counter);
            exit(1);
        }
    }

    if (new_process == 0){
        set_limits(time_limit, memory_limit);

        printf("\nExecuting \"%s\" command with arguments ", parsed_line[0]);
        for (int i = 1; parsed_line[i] != NULL; i++){
            printf("\"%s\" ", parsed_line[i]);
        }
        printf("\n");

        execvp(parsed_line[0], parsed_line);
        printf("\n");
        exit(1);
    }

    if (new_process < 0){
        printf("Something went wrong with fork.\n");
        exit(1);
    }

    return 0;
}


int main(int argc, char** argv) {

    if (argc < 4){
        printf("You did't gave enough arguments. Please type file directory, time limit [s] and memory limit [MB]. \n");
        return 1;
    }

    FILE* file_with_tasks = fopen(argv[1], "r");
    if(!file_with_tasks){
        printf("Ups, something went wrong with opening file.\n");
        return 1;
    }

    char* time_limit = argv[2];
    char* memory_limit = argv[3];

    char* line_buffor = calloc(MAX_LINE_LENGTH, sizeof(char));

    int line_counter = 1;
    while(!exec_line(file_with_tasks, line_buffor, line_counter, time_limit, memory_limit)){
        line_counter++;
    }
    fclose(file_with_tasks);
    free(line_buffor);

    return 0;
}
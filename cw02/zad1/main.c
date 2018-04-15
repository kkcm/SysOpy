#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>


void generate_data(char* file_name, int numer_of_blocks, int block_size){

    int read_file = open("/dev/random", O_RDONLY);
    int write_file = open(file_name, O_RDWR | O_CREAT | O_TRUNC);

    if (write_file == -1){
        printf("Ups, something went wrong with opening file \"%s\".\n", file_name);
    } else if (read_file == -1){
        printf("Ups, something went wrong with opening file \"/dev/random\".\n");
    }

    char* buffor = calloc((size_t) block_size + 1, sizeof(char));

    for(int i=0; i<numer_of_blocks; i++) {
        read(read_file, buffor, (size_t) block_size);
        for (int j = 0; j < block_size; j++) {
            buffor[j] = (char) (abs(buffor[j]) % 52 + 65);
            if (buffor[j] > 90) {
                buffor[j] += 6;
            }
        }
        buffor[block_size] = 10;
        write(write_file, buffor, (size_t) block_size + 1);
    }

    close(read_file);
    close(write_file);
    free(buffor);
}


char* generate_file_name (char* file_name){
    char* tmp = calloc(sizeof(file_name) + 4, sizeof(char));
    strcat(tmp, file_name);
    strcat(tmp, ".txt");

    return tmp;
}

void sys_copy (char* file_name, char* file_name_2, int numer_of_blocks, int block_size){

    int read_file = open(file_name, O_RDONLY);
    int write_file = open(file_name_2, O_RDWR | O_CREAT | O_TRUNC);

    if (write_file == -1){
        printf("Ups, something went wrong with opening file \"%s\".\n", file_name_2);
    } else if (read_file == -1){
        printf("Ups, something went wrong with opening file \"%s\".\n", file_name);
    }

    char* buffor = calloc((size_t) block_size + 1, sizeof(char));

    for(int i=0; i<numer_of_blocks; i++) {
        read(read_file, buffor, (size_t) block_size + 1);
        write(write_file, buffor, (size_t) block_size + 1);
    }

    close(read_file);
    close(write_file);
    free(buffor);

}

void lib_copy(char* file_name, char* file_name_2, int numer_of_blocks, int block_size){
    FILE* read_file =  fopen (file_name, "r");
    FILE* write_file =  fopen (file_name_2, "w");

    if ((int) write_file == -1){
        printf("Ups, something went wrong with opening file \"%s\".\n", file_name_2);
    } else if ((int) read_file == -1){
        printf("Ups, something went wrong with opening file \"%s\".\n", file_name);
    }

    char* buffor = calloc((size_t) block_size + 1, sizeof(char));

    for (int i=0; i < numer_of_blocks; i++) {
        fread(buffor, sizeof(char), (size_t) block_size + 1, read_file);
        fwrite(buffor, sizeof(char), (size_t) block_size + 1, write_file);
    }

    fclose(read_file);
    fclose(write_file);
    free(buffor);
}

void sys_sort (char* file_name, int numer_of_blocks, int block_size){
    int read_file = open(file_name, O_RDWR);
    char* reg1 = calloc((size_t) block_size + 1, sizeof(char));
    char* reg2 = calloc((size_t) block_size + 1, sizeof(char));
    long int offset = (long int) ((block_size + 1) * sizeof(char));

    for(int i=0; i<numer_of_blocks; i++){
        lseek(read_file, i * offset, SEEK_SET);
        read(read_file, reg1, (size_t) block_size+1);

        for(int j=0; j<i; j++) {
            lseek(read_file, j * offset, SEEK_SET);
            read(read_file, reg2, (size_t) block_size+1);

            if(reg2[0] > reg1[0]){
                lseek(read_file, i * offset, SEEK_SET);
                write(read_file, reg2, (size_t) block_size+1);

                lseek(read_file, j * offset, SEEK_SET);
                write(read_file, reg1, (size_t) block_size+1);

                char *tmp = reg1;
                reg1 = reg2;
                reg2 = tmp;
            }
        }
    }

    close(read_file);
    free(reg1);
    free(reg2);
}

void lib_sort (char* file_name, int numer_of_blocks, int block_size){
    FILE* read_file =  fopen (file_name, "r+");
    char* reg1 = calloc((size_t) block_size + 1, sizeof(char));
    char* reg2 = calloc((size_t) block_size + 1, sizeof(char));
    long int offset = (long int) ((block_size + 1) * sizeof(char));

    for(int i=0; i<numer_of_blocks; i++){
        fseek(read_file, i * offset, 0);
        fread(reg1,sizeof(char), (size_t) block_size+1, read_file);

        for(int j=0; j<i; j++) {
            fseek(read_file, j * offset, 0);
            fread(reg2,sizeof(char), (size_t) block_size+1, read_file);

            if(reg2[0] > reg1[0]){
                fseek(read_file, i * offset, 0);
                fwrite(reg2,sizeof(char), (size_t) block_size+1, read_file);

                fseek(read_file, j * offset, 0);
                fwrite(reg1,sizeof(char), (size_t) block_size+1, read_file);

                char *tmp = reg1;
                reg1 = reg2;
                reg2 = tmp;
            }
        }
    }

    close(read_file);
    free(reg1);
    free(reg2);
}

long int count_time(struct timeval *time){
    long int result;
    result = (long int)(time -> tv_sec + 1000000 + time -> tv_usec);
    return result;
}


int main(int argc, char **argv) {

    char* operation = NULL;
    char* file_name = NULL;
    char* file_name_2 = NULL;
    char* function_type = NULL;
    int arg1 = -1;
    int arg2 = -1;

    FILE *results = fopen("wyniki.txt", "a");

    if (!results){
        printf ("There is some problem with writing to file!\n");
        return 1;
    }

    struct timeval real_time;
    struct rusage usage_time;
    long int system_start, system_end, user_start, user_end, real_start, real_end;
    gettimeofday(&real_time, NULL);
    getrusage(RUSAGE_SELF, &usage_time);

    system_start = count_time(&usage_time.ru_stime);
    user_start = count_time(&usage_time.ru_utime);
    real_start = count_time(&real_time);



    if (argc < 2){
        printf("You didn't give me any arguments. Firstly, you have to write kind of operation.\n");
        return 1;
    } else if (strcmp(argv[1], "generate") == 0){
        if(argc < 5){
            printf("You did't give me all required arguments. For generating data you have to write file name, number of blocks and length of blocks.\n");
            return 1;
        }

        operation = argv[1];
        file_name = generate_file_name(argv[2]);
        arg1 = (int) strtol(argv[3], NULL, 10);
        arg2 = (int) strtol(argv[4], NULL, 10);
        printf("Now, I gonna %s data to file \"%s\", %d blocks with %d length each.\n", operation, file_name, arg1, arg2);
        fprintf(results, "\nNow, I gonna %s data to file \"%s\", %d blocks with %d length each.\n", operation, file_name, arg1, arg2);

        generate_data(file_name, arg1, arg2);

    } else if (strcmp(argv[1], "sort") == 0){
        if (argc < 6) {
            printf("You did't give me all required arguments. For sorting data you have to write file name, number of blocks, length of blocks and type of using function.\n");
            return 1;
        }

        operation = argv[1];
        file_name = generate_file_name(argv[2]);
        arg1 = (int) strtol(argv[3], NULL, 10);
        arg2 = (int) strtol(argv[4], NULL, 10);

        if (strcmp(argv[5], "sys") == 0){
            function_type = argv[5];

            printf("Now, I gonna %s data in file \"%s\", %d blocks with %d length each, using \"%s\" function.\n", operation, file_name, arg1, arg2, function_type);
            fprintf(results, "\nNow, I gonna %s data in file \"%s\", %d blocks with %d length each, using \"%s\" function.\n", operation, file_name, arg1, arg2, function_type);

            sys_sort(file_name, arg1, arg2);

        } else if (strcmp(argv[5], "lib") == 0){
            function_type = argv[5];

            printf("Now, I gonna %s data in file \"%s\", %d blocks with %d length each, using \"%s\" function.\n", operation, file_name, arg1, arg2, function_type);
            fprintf(results, "\nNow, I gonna %s data in file \"%s\", %d blocks with %d length each, using \"%s\" function.\n", operation, file_name, arg1, arg2, function_type);

            lib_sort(file_name, arg1, arg2);
        } else {
            printf("You gave me wrong last argument. You can only choose one of this: sys or lib.\n");
            return 1;
        }

    } else if (strcmp(argv[1], "copy") == 0){
        if (argc < 7){
            printf("You did't give me all required arguments. For copying data you have to write source file name, destination file name, number of blocks, length of blocks and type of using function.\n");
            return 1;
        }

        operation = argv[1];
        file_name = generate_file_name(argv[2]);
        file_name_2 =  generate_file_name(argv[3]);
        arg1 = (int) strtol(argv[4], NULL, 10);
        arg2 = (int) strtol(argv[5], NULL, 10);

        if (strcmp(argv[6], "sys") == 0){
            function_type = argv[6];

            printf("Now, I gonna %s data from file \"%s\" to file \"%s\", %d blocks with %d length each, using \"%s\" function.\n", operation, file_name, file_name_2, arg1, arg2, function_type);
            fprintf(results, "\nNow, I gonna %s data from file \"%s\" to file \"%s\", %d blocks with %d length each, using \"%s\" function.\n", operation, file_name, file_name_2, arg1, arg2, function_type);

            sys_copy(file_name, file_name_2, arg1, arg2);

        } else if (strcmp(argv[6], "lib") == 0){
            function_type = argv[6];

            printf("Now, I gonna %s data from file \"%s\" to file \"%s\", %d blocks with %d length each, using \"%s\" function.\n", operation, file_name, file_name_2, arg1, arg2, function_type);
            fprintf(results, "\nNow, I gonna %s data from file \"%s\" to file \"%s\", %d blocks with %d length each, using \"%s\" function.\n", operation, file_name, file_name_2, arg1, arg2, function_type);

            lib_copy(file_name, file_name_2, arg1, arg2);
        } else {
            printf("You gave me wrong last argument. You can only choose one of this: sys or lib.\n");
            return 1;
        }

    } else {
        printf("You gave me wrong first argument. You can only choose one of this: generate, sort, copy.\n");
    }

    getrusage(RUSAGE_SELF, &usage_time);
    gettimeofday(&real_time, NULL);

    system_end = count_time(&usage_time.ru_stime);
    user_end = count_time(&usage_time.ru_utime);
    real_end = count_time(&real_time);

    printf("Real time: %.10ldµs \n"
                   "System time: %.10ldµs \n"
                   "User time: %.10ldµs \n",
           real_end - real_start, system_end - system_start, user_end - user_start);
    fprintf(results, "\nReal time: %.10ldµs \n"
                   "System time: %.10ldµs \n"
                   "User time: %.10ldµs \n",
           real_end - real_start, system_end - system_start, user_end - user_start);

    free(file_name);
    free(file_name_2);
    fclose(results);

    return 0;
}

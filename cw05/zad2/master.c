
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_LINE_SIZE 128

int main (int argc, char** argv){
    if (argc < 2){
        printf("Master: Ups, you didn't gave any argument. I need a FIFO name.\n");
        exit(1);
    }

    if(mkfifo(argv[1], S_IWUSR | S_IRUSR) == -1){
        printf("Master: Ups, something went wrong with creating FIFO.\n");
        exit(1);
    }

    FILE* fifo = fopen(argv[1], "r");
    if (!fifo) {
        printf("Master: Ups, something went wrong with opening FIFO.\n");
        exit(1);
    }

    char buffer_line[MAX_LINE_SIZE];

    while(fgets(buffer_line, MAX_LINE_SIZE, fifo)){
        write(1, buffer_line, strlen(buffer_line));
    }

    printf("Master: Yeah, I have just ended reading.\n");
    fclose(fifo);
    return 0;
}


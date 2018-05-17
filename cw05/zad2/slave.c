#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <memory.h>

#define MAX_LINE_SIZE 128


int main (int argc, char** argv) {
    if (argc < 3) {
        printf("Slave: Ups, you didn't gave any argument. I need a FIFO name and N.\n");
        exit(1);
    }

    int N = (int) strtol(argv[2], NULL, 10);

    int fifo = open(argv[1], O_WRONLY);

    if (fifo == -1) {
        printf("Slave: Ups, something went wrong with opening FIFO.\n");
        exit(1);
    }

    char buffers[2][MAX_LINE_SIZE];
    srand ((unsigned int) (time(NULL)));

    for(int i=0; i<N; i++){
 /*       FILE* date = popen("date", "r");
        if(!date){
            printf("Slave: Ups, something went wrong with opening date.\n");
            exit(1);
        }
  */     

        fgets(buffers[0], MAX_LINE_SIZE, popen("date", "r"));
        int pid = getpid();
        sprintf(buffers[1], "Slave: %d - %s.\n", pid, buffers[0]);
        write(fifo, buffers[1], strlen(buffers[1]));

    //    fclose(date);
        
        sleep((unsigned int) (rand() % 4 + 2));
    }
    
    close(fifo);
    return 0;
}


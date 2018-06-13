#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <semaphore.h>


#define FAILURE_EXIT(code, message, ...) {printf(message, ##__VA_ARGS__); exit(code);}

int P;
int K;
int N;
int L;
int descriptive_mode;
int search_mode;
int producers_finished = 0;
int production_index = 0;
int consumption_index = 0;

unsigned int nk;

char* configuration_file_path;
char text_file_path[FILENAME_MAX];
char** buffer;

FILE* text_file;

pthread_t* producers_threads;
pthread_t* consumers_threads;

sem_t* buffers_semaphore;


int is_search_length(int line_length){
    return search_mode == (line_length < L ? -1 : line_length > L ? 1 : 0);
}

int give_sign(){
    return search_mode == -1 ? '<' : search_mode == 1 ? '>' : '=';
}

void* producer(void* arguments){
    int index;
    char line[LINE_MAX];

    while(fgets(line, LINE_MAX, text_file) != NULL){
        if(descriptive_mode) printf("Producer %d: I gonna read next line: %s", (int) pthread_self(), line);
        sem_wait(&buffers_semaphore[N]);

        sem_wait(&buffers_semaphore[N+2]);

        index = production_index;
        if(descriptive_mode) printf("Producer %d: I gonna take buffer index %d.\n", (int) pthread_self(), index);
        production_index = (production_index + 1) % N;

        sem_wait(&buffers_semaphore[index]);
        sem_post(&buffers_semaphore[N]);

        buffer[index] = calloc(strlen(line) + 1, sizeof(char));
        strcpy(buffer[index], line);
        if(descriptive_mode) printf("Producer %d: I've just copied line to buffer at index: %d.\n", (int) pthread_self(), index);

        sem_post(&buffers_semaphore[index]);
    }
    if(descriptive_mode) printf("Producer %d: I've just finished reading.\n", (int) pthread_self());
    return NULL;
}

void* consumer(void* arguments){
    int index;
    int line_length;
    char* line;

    while(1){
        sem_wait(&buffers_semaphore[N+1]);

        while(buffer[consumption_index] == NULL){
            sem_post(&buffers_semaphore[N+1]);
            if(producers_finished){
            //    sem_post(&buffers_semaphore[N+1]);
                if(descriptive_mode) printf("Consumer %d: Production is finished so I have nothing to do. That's the end of my journey.\n", (int) pthread_self());
                return NULL;
            }
            sem_wait(&buffers_semaphore[N+1]);
        }

        index = consumption_index;
        if(descriptive_mode) printf("Consumer %d: I gonna take buffer at index %d.\n", (int) pthread_self(), index);
        consumption_index = (consumption_index + 1) % N;

        sem_wait(&buffers_semaphore[index]);
    //    sem_post(&buffers_semaphore[N+1]);

        line = buffer[index];
        buffer[index] = NULL;
        if(descriptive_mode) printf("Consumer %d: I've just read line from the buffer at index %d.\n", (int) pthread_self(), index);

        sem_post(&buffers_semaphore[N+2]);
        sem_post(&buffers_semaphore[N+1]);
        sem_post(&buffers_semaphore[index]);

        if (line != NULL) {
            line_length = (int) strlen(line);
            if (is_search_length(line_length)) {
                if (descriptive_mode)
                    printf("Consumer %d: This line has length %d %c %d.\n", (int) pthread_self(), line_length,
                           give_sign(), L);
                printf("Consumer %d: Text at index %d: %s", (int) pthread_self(), index, line);
            }
        }

    //    sem_post(&buffers_semaphore[N+2]);
    //    sem_post(&buffers_semaphore[N+1]);
    //    sem_post(&buffers_semaphore[index]);

        free(line);
    }
    return NULL;
}

void signal_handler(int signo){
    printf("I've just received %s. I hate to cancel all threads.\n", signo == SIGINT ? "SIGINT" : "SIGALRM");
    for(int i = 0; i < P; i++){
        pthread_cancel(producers_threads[i]);
    }

    for(int i = 0; i < K; i++){
        pthread_cancel(consumers_threads[i]);
    }
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){

    if(argc < 2) FAILURE_EXIT(1, "Oh, no! You have no argument.\n I'm waiting for path to configuration file.\n");
    configuration_file_path = argv[1];

//wczytanie konfiguracji
    FILE* configuration = fopen(configuration_file_path, "r");
    if(configuration == NULL) FAILURE_EXIT(2, "Oh no! Something went wrong with opening configuration file.\n");
    fscanf(configuration, "%d %d %d %s %d %d %d %d", &P, &K, &N, text_file_path, &L, &search_mode, &descriptive_mode, &nk);
    printf("Your configuration:\nP: %d\nK: %d\nN: %d\nL: %d\nnk: %d\ndescriptive_mode: %d\nsearch_mode: %d\ntext_file_path: %s\n",
           P, K, N, L, nk, descriptive_mode, search_mode, text_file_path);
    fclose(configuration);

//sygnały
    signal(SIGINT, signal_handler);
    if(nk > 0) signal(SIGALRM, signal_handler);

//otwarcie pliku z tekstem oraz przygotowanie buffora
    text_file = fopen(text_file_path, "r");
    if (text_file == NULL) FAILURE_EXIT(2, "Oh, no! Somethnig went wrong with opening file with text.\n");
    buffer = calloc((size_t) N, sizeof(char*));

// przygotowanie tablic wątków, tablicy mutexów oraz warunków sprawdzających
    producers_threads = calloc((size_t) P, sizeof(pthread_t));
    consumers_threads = calloc((size_t) K, sizeof(pthread_t));

    buffers_semaphore = calloc((size_t) (N+3), sizeof(sem_t));
    for(int i=0; i < N+2; i++){
        sem_init(&buffers_semaphore[i], 0, 1);
    }
    sem_init(&buffers_semaphore[N+2], 0,(unsigned int) N);

//oddpalenie wątków

    for(int i=0; i < P; i++){
        pthread_create(&producers_threads[i], NULL, producer, NULL);
    }

    for(int i=0; i < K; i++){
        pthread_create(&consumers_threads[i], NULL, consumer, NULL);
    }

//ustawienie alarmu
    if(nk > 0) alarm(nk);

//czekanie na wykonanie wątków

    for(int i=0; i < P; i++){
        pthread_join(producers_threads[i], NULL);
    }

    producers_finished = 1;

    for(int i=0; i < K; i++){
        pthread_join(consumers_threads[i], NULL);
    }

//sprzątanie

    fclose(text_file);

    for (int i=0; i < N; i++){
        if (buffer[i]) free(buffer[i]);
    }
    free(buffer);

    for (int i = 0; i < N+3; i++){
        sem_destroy(&buffers_semaphore[i]);
    }
    free(buffers_semaphore);


    return 0;
}
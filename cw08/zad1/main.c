#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/times.h>
#include <zconf.h>


#define FAILURE_EXIT(code, message, ...) {printf(message, ##__VA_ARGS__); exit(code);}

unsigned char** image;
unsigned char** filtered_image;
float** filter;

char* picture_file_path;
char* filter_file_path;
char* result_file_path;


int width;
int height;
int max_color_value;
int filter_size;
int image_size;
int part_size;
int thread_number;

void read_image(){
    FILE* file = fopen(picture_file_path, "r");
    if (file == NULL) FAILURE_EXIT(2, "Oh no! Something went wrong with opening picture.\n");
    fscanf(file, "P2 %d %d %d", &width, &height, &max_color_value);

    image = calloc(height, sizeof(unsigned char*));

    for(int i=0; i<height; i++){
        image[i] = calloc(width, sizeof(unsigned char));
        for(int j=0; j<width; j++){
            fscanf(file, "%d ", &image[i][j]);
        }
    }
    fclose(file);
}

void read_filter(){
    FILE* file = fopen(filter_file_path, "r");
    if (file == NULL) FAILURE_EXIT(2, "Oh no! Something went wrong with opening filter.\n");
    fscanf(file, "%d\n", &filter_size);

    filter = calloc(filter_size, sizeof(float*));

    for(int i=0; i<filter_size; i++){
        filter[i] = calloc(filter_size, sizeof(float));
        for(int j=0; j<filter_size; j++){
            fscanf(file, "%f ", &filter[i][j]);
        }
    }
    fclose(file);
}

void save_filtered_image(){
    FILE* file = fopen(result_file_path, "w");
    if (file == NULL) FAILURE_EXIT(2, "Oh no! Something went wrong with openting file to save results.\n");
    fprintf(file, "P2\n%d %d\n%d\n", width,height, max_color_value);
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            fprintf(file, "%d ", filtered_image[i][j]);
        }
    }
    fprintf(file, "\n");
    fclose(file);
}

int max(int a, int b){
    return a > b ? a : b;
}

int min(int a, int b){
    return a < b ? a : b;
}


int filter_image_pixel(int x, int y){
    double sum = 0;

    int half_filter_size = (int) floor(filter_size / 2);

    for (int filter_height = 0; filter_height < filter_size; filter_height++){
        for (int filter_width = 0; filter_width < filter_size; filter_width++){
            int i_y = min((height - 1), max(0, y - half_filter_size + filter_height));
            int i_x = min((width - 1), max(0, x - half_filter_size + filter_width));
            sum += image[i_y][i_x] * filter[filter_height][filter_width];
        }
    }
    return (int) max(0, min(round(sum), 255));
}

void* filter_image_part(void *arguments){
    int thread_num = *(int *) arguments;

    int start_pixel = thread_num * part_size;
    int end_pixel = thread_num != thread_number - 1 ? start_pixel + part_size : image_size;

    for (int i=start_pixel; i<end_pixel; i++) {
        filtered_image[i / width][i % width] = filter_image_pixel(i % width, i / width);
    }
    return NULL;
}


void filter_image(){
    image_size = width * height;
    part_size = image_size / thread_number;
    pthread_t *thread = calloc(thread_number, sizeof(pthread_t));

    filtered_image = calloc(height, sizeof(unsigned char*));
    for(int i=0; i<height; i++) {
        filtered_image[i] = calloc(width, sizeof(unsigned char));
    }

    for (int i=0; i<thread_number; i++){
        int *argument = calloc(1, sizeof(int));
        *argument = i;
        pthread_create(&thread[i], NULL, filter_image_part, argument);
    }

    for(int i=0; i<thread_number; i++){
        pthread_join(thread[i], 0);
    }

}

void save_times_and_run_infos(clock_t* real_time, struct tms* tms_time){
    FILE* file = fopen("Times.txt", "a");
    if (file == NULL) FAILURE_EXIT(2, "Oh no! Something went wrong with opening file \"Times.txt\".\n");

    fprintf(file, "Image: %s. ", picture_file_path);
    fprintf(file, "Image size: %dx%d.\n", height, width);
    fprintf(file, "Filter: %s. ", filter_file_path);
    fprintf(file, "Filter size: %dx%d.\n", filter_size, filter_size);
    fprintf(file, "Threads number: %d.\n", thread_number);
    fprintf(file, "\n");
    fprintf(file, "Real time:   %.0lf    ", (double) (real_time[1] - real_time[0]));
    fprintf(file, "User time:   %.0lf    ", (double) (tms_time[1].tms_utime - tms_time[0].tms_utime));
    fprintf(file, "System time: %.0lf \n", (double) (tms_time[1].tms_stime - tms_time[0].tms_stime));
    fprintf(file, "\n\n");
    fclose(file);
}

int main(int argc, char *argv[]){

    if (argc < 5) FAILURE_EXIT(1, "Oh, no! You have to less arguments.\n I'm waiting for thread number, picture file path, filter file path, result file path\n");
    thread_number = (int) strtol(argv[1], NULL, 10);
    if (thread_number <= 0) FAILURE_EXIT(1, "Oh no! Thread number must be greater than 0.");
    picture_file_path = argv[2];
    filter_file_path = argv[3];
    result_file_path = argv[4];


    read_image();
    read_filter();

    clock_t real_time[2] = {0, 0};
    struct tms tms_time[2];

    real_time[0] = times(&tms_time[0]);
    filter_image();
    real_time[1] = times(&tms_time[1]);

    save_filtered_image();
    save_times_and_run_infos(real_time, tms_time);

    return 0;
}
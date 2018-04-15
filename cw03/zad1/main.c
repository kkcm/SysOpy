#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <ftw.h>
#include <unistd.h>

char* sign;
time_t user_date;

int check_date(time_t modification_date) {
    double diff = difftime(modification_date, user_date);
    if (*sign == '=')
        return diff == 0 ? 1 : 0;
    else if (*sign == '<')
        return diff < 0 ? 1 : 0;
    else if (*sign == '>')
        return diff > 0 ? 1 : 0;

    return 0;
}


int print_file_info(const char* path, const struct stat* stats, int tflag, struct FTW *ftwbuf){

    if (S_ISREG(stats->st_mode) && check_date(stats -> st_mtime)) {

        printf("Absolute path: %s\n", realpath(path, NULL));
        printf("Size: %d\n", (int) stats->st_size);

        printf("File permissions: ");
        printf((S_ISDIR(stats->st_mode)) ? "d" : "-");
        printf((stats->st_mode & S_IRUSR) ? "r" : "-");
        printf((stats->st_mode & S_IWUSR) ? "w" : "-");
        printf((stats->st_mode & S_IXUSR) ? "x" : "-");
        printf((stats->st_mode & S_IRGRP) ? "r" : "-");
        printf((stats->st_mode & S_IWGRP) ? "w" : "-");
        printf((stats->st_mode & S_IXGRP) ? "x" : "-");
        printf((stats->st_mode & S_IROTH) ? "r" : "-");
        printf((stats->st_mode & S_IWOTH) ? "w" : "-");
        printf((stats->st_mode & S_IXOTH) ? "x" : "-");

        printf("\nModification time: %s\n\n\n", ctime(&(stats->st_mtime)));

    }
    return 0;
}

int search_directory_tree (char* path){
    DIR* stream = opendir(path);
    if (stream == NULL){
        printf("Something went wrong with getting to the directory.\n");
        return 1;
    }
    struct dirent* file;
    struct stat* stats = calloc(1, sizeof(struct stat));
    char path_buffor[PATH_MAX + 1];
    while((file = readdir(stream)) != NULL){
        if(strcmp(file -> d_name, ".") == 0 || strcmp(file -> d_name, "..") == 0) continue;

        strcpy(path_buffor, path);
        strcat(path_buffor, "/");
        strcat(path_buffor, file -> d_name);

        if(!lstat(path_buffor, stats)){
            if(S_ISDIR(stats -> st_mode)){

                pid_t child_pid;
                child_pid = fork();

                if(child_pid<0){
                    printf("%s\n","Fork failure." );
                    exit(1);
                }

                if(child_pid  == 0)
                {
                    if(search_directory_tree(path_buffor)){
                        printf("Ups, something went wrong.\n");
                    }
                    exit(0);
                }

                if (child_pid > 0){
                    wait(NULL);
                }


            } else if(S_ISREG(stats -> st_mode)){
                print_file_info(path_buffor, stats, FTW_F, NULL);
            }
        }
    }
    closedir(stream);
    return 0;
}



int main(int argc, char** argv) {
    char* path = NULL;

    if (argc < 6){
        printf("You didn't give me enough arguments. You should type [path] ['<'|'=='|'>'] [YYYY-MM-DD] [HH:MM:SS] [stat|ntfw]");
        return 1;
    }

    path = argv[1];

    if(!(strcmp(argv[2], "=") == 0 | strcmp(argv[2], "<") == 0 | strcmp(argv[2], ">") == 0 )){
        printf("Second argument is wrong. You can only type = or > or <.\n");
        return 1;
    }

    sign = argv[2];

    char* time_arg = calloc(20, sizeof(char));
    strcpy(time_arg, argv[3]);
    time_arg[10] = ' ';
    strcat(time_arg, argv[4]);
    time_arg[19] = '\0';

    struct tm time;
    if(!strptime(time_arg, "%Y-%m-%d %H:%M:%S", &time)){
        printf("Something is wrong with your date. Please check if it is like that -> [YYYY-MM-DD] [HH:MM:SS]\n");
        return 1;
    }

    user_date = mktime(&time);

    char buffer[20];
    strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", &time);

    if(strcmp(argv[5], "stat") == 0){
        printf("Now, I gonna run program in path - %s, with sign - %s, with date - %s, in version - \"stat\".\n\n", path, sign, buffer);

        if(search_directory_tree(path)){
            printf("Ups, something went wrong.\n");
        };

    } else if(strcmp(argv[5], "nftw") == 0) {
        printf("Now, I gonna run program in path - %s, with sign - %s, with date - %s, in version - \"nftw\".\n\n", path, sign, buffer);
        if(nftw(path, print_file_info, 10, FTW_PHYS) != 0){
            printf("Ups, something went wrong.\n");
        };

    } else {
        printf("You gave me wrong last argument. You can only choose \"stat\" or \"nftw\".\n");
    }

    return 0;
}
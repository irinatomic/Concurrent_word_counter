#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "main.h"
#include "map.h"

HashMap mapa;

void* main_thread_work(){

    char input[MAX_WORD_LEN];
    unsigned int len;

    struct file arguments[MAX_THREADS];      // max 20 arguments for 2 threads
    pthread_t threads[MAX_THREADS];          // max 20 threads
    int k = 0;

    while(fgets(input, sizeof(input), stdin) != NULL) {  

        input[strcspn(input, "\n")] = '\0';
        char* token = strtok(input, " ");

        if(strcmp(token, "_count_") == 0){

            // token = strtok(NULL, " ");
            // arguments[k] = (struct file) {token, NULL, 0};
            // pthread_create(&threads[k], NULL, scanner_work, (void*) (arguments + k));
            // k++;

        } else if(strcmp(token, "_stop_") == 0)
            break;
        
        else {
            search_result* res = map_get_frequency(token);
            printf("%s: %d \n", token, res->frequency);
        }                                              
    }

    for(int i = 0; i < MAX_THREADS; i++){
        pthread_join(threads[i], NULL);
    }
}

void *scanner_work(void *_args){

    struct file *args = (struct file *) _args;
    char* filename = args->file_name;

    struct stat fileStat;
    stat(filename, &fileStat);
    time_t prev_mod_time = fileStat.st_mtime;
    int prev_length = fileStat.st_size;

    go_through_file(args, 0);               // first time we go through the whole file

    while(1){
        
        struct stat fileStat;
        stat(filename, &fileStat);
        time_t mod_time = fileStat.st_mtime;

        if(prev_mod_time != mod_time){

            args->mod_time = mod_time;
            args->length = fileStat.st_size;
            go_through_file(_args, prev_length);
            prev_mod_time = mod_time;
            prev_length = fileStat.st_size;
        }

        sleep(5);                             // sleep 5 seconds
    }
}

void go_through_file(void *_args, int prev_length){

    struct file *args = (struct file *) _args;
    char file_name[256];
    strcpy(file_name, args->file_name);



}

int main(){

    //init the hash map
    map_init();

    //main thread
    pthread_t main_thread;
    pthread_create(&main_thread, NULL, main_thread_work, NULL);    
    pthread_join(main_thread, NULL);

    return 0;
}


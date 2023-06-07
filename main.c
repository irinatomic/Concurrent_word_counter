#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include "main.h"
#include "map.h"

HashMap mapa;
int exit_thread;

void* main_thread_work(){

    char input[MAX_WORD_LEN];
    unsigned int len;

    struct file arguments[MAX_THREADS];      // max 20 arguments for 2 threads
    pthread_t threads[MAX_THREADS];          // max 20 threads
    int k = 0;
    exit_thread = 0;

    while(fgets(input, sizeof(input), stdin) != NULL) {  

        input[strcspn(input, "\n")] = '\0';
        char* token = strtok(input, " ");

        if(strcmp(token, "_count_") == 0){
            
            // token = strtok(NULL, " ");
            // arguments[k] = (struct file) {token, NULL, 0};
            // pthread_create(&threads[k], NULL, scanner_work, (void*) (arguments + k));
            // k++;

        } else if(strcmp(token, "_stop_") == 0){
            exit_thread = 1;
            break;
        }
        
        else {  

            char* prev_token = token;
            while(token != NULL){
                prev_token = token;
                token = strtok(NULL, " ");
            }

            search_result* res = map_get_frequency(&mapa, prev_token);
            if(res->frequency == -1)
                printf("Word %s does not exist\n", prev_token);
            else
                printf("%s: %d \n", prev_token, res->frequency);
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

        if(exit_thread == 1)
            pthread_exit(NULL);
    
        struct stat fileStat;
        stat(filename, &fileStat);
        time_t mod_time = fileStat.st_mtime;

        if(prev_mod_time != mod_time){

            args->mod_time = mod_time;
            args->length = fileStat.st_size;
            go_through_file(args, prev_length);
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

    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", file_name);
        return;
    }
    
    fseek(file, prev_length, SEEK_SET);

    HashMap temp_map;
    map_init(&temp_map);                            //temp map for words and their frequencies in file

    char word[MAX_WORD_LEN];
    while (fscanf(file, "%s", word) == 1) {         //fscanf uses whitespace chars as delimiters (' ', \t, \n)
        for (int i = 0; word[i] != '\0'; i++) {
            if(!isalpha(word[i])) continue;
        }
        map_add_word(&temp_map, word, 1);
    }

    fclose(file);

    // Merge the temporary map with the main map
    merge_maps(&mapa, &temp_map);
    sleep(5);
}

void merge_maps(HashMap* main_map, HashMap* temp_map) {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        Node* current = temp_map->array[i];
        while (current != NULL) {
            map_add_word(main_map, current->key, current->value);
            current = current->next;
        }
    }
}

int main(){

    //init the hash map
    map_init(&mapa);

    //main thread
    pthread_t main_thread;
    pthread_create(&main_thread, NULL, main_thread_work, NULL);    
    pthread_join(main_thread, NULL);

    return 0;
}

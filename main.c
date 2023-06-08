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
    char* token;

    while(fgets(input, sizeof(input), stdin) != NULL) {  

        input[strcspn(input, "\n")] = '\0';
        token = strtok(input, " ");

        if(strcmp(token, "_count_") == 0){
            
            token = strtok(NULL, " ");
            printf("TOKEN %s\n", token);
            arguments[k] = (struct file) {token, NULL, 0};
            pthread_create(threads + k, NULL, scanner_work, (void*) (arguments + k));
            k++;

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
                printf("Word %s: frequency %d \n", prev_token, res->frequency);

            token = NULL;
            input[0] = '\n';
        }                                              
    }

    for(int i = 0; i < MAX_THREADS; i++){
        pthread_join(threads[i], NULL);
    }
}

void *scanner_work(void *_args){

    struct file *args = (struct file *) _args;
    char* filename = args->file_name;
        printf("FILE NAME %s\n", filename);

    filename[strlen(filename)] = '\0';
        printf("FILE NAME %s\n", filename);


    struct stat fileStat;
    stat(filename, &fileStat);
    args->mod_time = fileStat.st_mtime;
    args->length = fileStat.st_size;

    go_through_file(args, 0);            // First time we go through the whole file
    sleep(5);

    while (1) {
        if (exit_thread == 1)
            pthread_exit(NULL);

        stat(filename, &fileStat);

        if (args->mod_time != fileStat.st_mtime) {
            printf("Duzina %d\n", fileStat.st_size);
            args->mod_time = fileStat.st_mtime;
            go_through_file(args, args->length);
            args->length = fileStat.st_size;
        }

        sleep(5);                                   // Sleep for 5 seconds
    }
}

void go_through_file(struct file* args, int prev_length){

    char* file_name = args->file_name;
    file_name[strlen(file_name)] = '\0';
    printf("FILE NAME %s\n", file_name);
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", file_name);
        return;
    }
    
    fseek(file, prev_length, SEEK_SET);

    HashMap temp_map;
    map_init(&temp_map);                            //temp map for words and their frequencies in file

    char line[4096];
    char word[MAX_WORD_LEN];
    while (fgets(line, sizeof(line), file) != NULL) {
        int length = strlen(line);
        if (line[length - 1] == '\n') {
            line[length - 1] = '\0';                // Remove the newline character at the end
        }

        char* token = strtok(line, " ");
        while (token != NULL) {
            int i = 0;
            while (token[i] != '\0') {
                if (isalpha(token[i])) {
                    map_add_word(&temp_map, token, 1);
                    printf("REC %s\n", token);
                    break;
                }
                i++;
            }
            token = strtok(NULL, " ");
        }
    }

    // Merge the temporary map with the main map
    merge_maps(&mapa, &temp_map);
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

    //init mutex
    pthread_mutex_init(&map_mutex, NULL);

    //init the hash map
    map_init(&mapa);

    //main thread
    pthread_t main_thread;
    pthread_create(&main_thread, NULL, main_thread_work, NULL);    
    pthread_join(main_thread, NULL);

    //destroy mutex
    pthread_mutex_destroy(&map_mutex);

    return 0;
}

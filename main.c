#include "main.h"
#include "map.h"

HashMap mapa;
HashMap stopWords;
int exit_thread;
char filesArray[MAX_FILES][MAX_FILENAME_LEN];

void* main_thread_work(){

    char input[MAX_WORD_LEN];
    unsigned int len;
    int filesArrayIndex = 0;

    struct file arguments[MAX_THREADS];         // max 20 arguments for 20 threads
    pthread_t threads[MAX_THREADS];             // max 20 threads
    int k = 0;
    exit_thread = 0;
    char* token;

    while(fgets(input, sizeof(input), stdin) != NULL) {  

        input[strcspn(input, "\n")] = '\0';
        token = strtok(input, " ");

        if(strcmp(token, "_count_") == 0){
            
            token = strtok(NULL, " ");
            strcpy(filesArray[filesArrayIndex++], token);
            arguments[k].file_name = strdup(token); 
            arguments[k].mod_time = NULL;
            arguments[k].length = 0; 
            pthread_create(&threads[k], NULL, scanner_work, (void*) (arguments + k));
            k++;

        } else if(strcmp(token, "_stop_") == 0){
            exit_thread = 1;
            break;

        } else if(strcmp(token, "_stopwords_") == 0){

            token = strtok(NULL, " ");
            if(token == NULL) continue;
            char* stops_filename = token = strdup(token);
            add_stopwords(stops_filename);
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

void add_stopwords(char* filename){

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filename);
        return;
    }

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
                    stopwords_add_word(&stopWords, token, 1);
                    break;
                }
                i++;
            }
            token = strtok(NULL, " ");
        }
    }

}

void *scanner_work(void *_args){

    struct file *args = (struct file *) _args;
    char* filename = args->file_name;
    filename[strlen(filename)] = '\0';

    struct stat fileStat0;
    stat(filename, &fileStat0);
    time_t mod_time = fileStat0.st_mtime;
    int prev_len = fileStat0.st_size;

    go_through_file(filename, 0);            // First time we go through the whole file
    sleep(5);

    while (1) {
        if (exit_thread == 1)
            pthread_exit(NULL);

        struct stat fileStat;
        stat(filename, &fileStat);

        if (mod_time != fileStat.st_mtime) {
            mod_time = fileStat.st_mtime;
            go_through_file(filename, prev_len);
            prev_len = fileStat.st_size;
        }

        sleep(5);                                   // Sleep for 5 seconds
    }
}

void go_through_file(char* filename, int prev_length){

    filename[strlen(filename)] = '\0';
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filename);
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
                if (isalpha(token[i]) && stopwords_cointains(&stopWords, token) == 0) {
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
    pthread_mutex_init(&stopwords_mutex, NULL);

    //init the hash map
    map_init(&mapa);
    map_init(&stopWords);

    //main thread
    pthread_t main_thread;
    pthread_create(&main_thread, NULL, main_thread_work, NULL);    
    pthread_join(main_thread, NULL);

    //destroy mutex
    pthread_mutex_destroy(&map_mutex);
    pthread_mutex_destroy(&stopwords_mutex);

    return 0;
}

#include "main.h"
#include "map.h"

int hash(char* key) {

    //pretvori sva slova u mala
    char* temp = key;
    while(*temp != '\0'){
        if(*temp >= 'A' && *temp <= 'Z')
            *temp = *temp + ('a' - 'A');
        temp++;
    }

    unsigned long hash = 5381;                  // Dan bernstein (djb2): http://www.cse.yorku.ca/~oz/hash.html
    int c;

    while (c = *key++)
        hash = ((hash << 5) + hash) + c;        /* hash * 33 + c */

    return hash % ARRAY_SIZE;
}

void map_add_word(HashMap* mapa, char* key, int value) {

    unsigned int index = hash(key);
    Node* current = mapa->array[index];
    Node* prev = NULL;

    // Search for the existing word in the linked list
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            pthread_mutex_lock(&map_mutex);             //mutex lock
            current->value += value;                    //word exists, update the value
            pthread_mutex_unlock(&map_mutex);           //mutex unlock
            return;
        }
        prev = current;
        current = current->next;
    }

    // Word does not exist, create a new node and insert it at the beginning of the linked list
    Node* new_node = malloc(sizeof(Node));
    new_node->key = strdup(key);
    new_node->value = value;
    new_node->next = NULL;

    if (prev == NULL)
        mapa->array[index] = new_node;                  // linked list is empty, make the new node the first node
    else
        prev->next = new_node;                          // insert the new node at the beginning of the linked list
}

search_result* map_get_frequency(HashMap* map, char* key) {

    search_result* result = malloc(sizeof(search_result));
    result->key = NULL;
    result->frequency = -1;

    int index = hash(key);
    Node* current = map->array[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            pthread_mutex_lock(&map_mutex);                          //mutex lock
            result->key = current->key;
            result->frequency = current->value;
            pthread_mutex_unlock(&map_mutex);                        //mutex unlock
            break;
        }
        current = current->next;
    }

    return result;
}

void map_init(HashMap* mapa) {

    for (int i = 0; i < ARRAY_SIZE; i++) {
        mapa->array[i] = NULL;
    }
}

void stopwords_add_word(HashMap* mapa, char* key, int value){

    unsigned int index = hash(key);
    Node* current = mapa->array[index];
    Node* prev = NULL;

    // Search for the existing word in the linked list
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            pthread_mutex_lock(&stopwords_mutex);             //mutex lock
            current->value += value;                    //word exists, update the value
            pthread_mutex_unlock(&stopwords_mutex);           //mutex unlock
            return;
        }
        prev = current;
        current = current->next;
    }

    // Word does not exist, create a new node and insert it at the beginning of the linked list
    Node* new_node = malloc(sizeof(Node));
    new_node->key = strdup(key);
    new_node->value = value;
    new_node->next = NULL;

    if (prev == NULL)
        mapa->array[index] = new_node;                  // linked list is empty, make the new node the first node
    else
        prev->next = new_node;                          // insert the new node at the beginning of the linked list
}

extern int stopwords_cointains(HashMap* map, char* key){

    int counter = 0;

    int index = hash(key);
    Node* current = map->array[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            pthread_mutex_lock(&map_mutex);                          //mutex lock
            counter = current->key;
            pthread_mutex_unlock(&map_mutex);                        //mutex unlock
            break;
        }
        current = current->next;
    }

    return counter;
}
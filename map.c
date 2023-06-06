#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "map.h"

extern HashMap mapa;

int hash(const char* key) {

    unsigned int hash_value = 0;
    int prime = 31;        //prime number for better distribution

    while (*key != '\0') {
        hash_value = (hash_value * prime) + (*key);
        key++;
    }

    return hash_value % ARRAY_SIZE;
}

void map_add_word(char* key, int value) {

    unsigned int index = hash(key);
    Node* current = mapa.array[index];
    Node* prev = NULL;

    // Search for the existing word in the linked list
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            current->value += value;                    //word exists, update the value
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

    if (prev == NULL) {
        // The linked list is empty, make the new node the first node
        mapa.array[index] = new_node;
    } else {
        // Insert the new node at the beginning of the linked list
        prev->next = new_node;
    }
}


int map_get_frequency(const char* key) {

    unsigned int index = hash(key);

    Node* current = mapa.array[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }

    // not found
    return -1;   
}

void map_init() {

    for (int i = 0; i < ARRAY_SIZE; i++) {
        mapa.array[i] = NULL;
    }
}



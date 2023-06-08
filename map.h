#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ARRAY_SIZE      5011

typedef struct search_result {
	char* key; 
	int frequency; 
} search_result;

typedef struct Node {
    char* key;
    int value;
    struct Node* next;
} Node;

typedef struct HashMap {
    Node* array[ARRAY_SIZE];
} HashMap;

extern int hash(char* key);
extern void map_init(HashMap* map);
extern void map_add_word(HashMap* mapa, char* key, int value);
extern search_result* map_get_frequency(HashMap* map, char* key);

#endif
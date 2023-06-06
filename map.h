#define ARRAY_SIZE      5011

typedef struct Node {
    char* key;
    int value;
    struct Node* next;
} Node;

typedef struct HashMap {
    Node* array[ARRAY_SIZE];
} HashMap;

extern void map_init();
extern void map_add_word(char* key, int value);
extern int map_get_frequency(const char* key);
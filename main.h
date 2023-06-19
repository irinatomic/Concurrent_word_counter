#ifndef MAIN_H
#define MAIN_H

#include <sys/stat.h>
#include <ctype.h>
#include <pthread.h>
#include "map.h"

#define MAX_WORD_LEN    64             //longest word length with \0
#define MAX_THREADS     20

// list of file names
#define MAX_FILES         20
#define MAX_FILENAME_LEN  50

typedef struct file {
	char* file_name; 
	time_t mod_time; 
    int length;
} scanned_file;

// start of program
extern void* main_thread_work();

// scanner thread
extern void *scanner_work(void *_args);
extern void go_through_file(char* filename, int prev_length);
extern void merge_maps(HashMap* main_map, HashMap* temp_map);

// our mutex
pthread_mutex_t map_mutex;
pthread_mutex_t stopwords_mutex;

// stopwords
extern void add_stopwords(char* filename);

#endif 
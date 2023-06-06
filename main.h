#ifndef MAIN_H
#define MAIN_H

#include <sys/stat.h>
#include "map.h"

#define MAX_WORD_LEN    64             //longest word length with \0
#define MAX_THREADS     20

typedef struct file {
	char file_name[256]; 
	time_t mod_time; 
    int length;
	int* exit_thread;
} scanned_file;


// start of program
extern void* main_thread_work();

// scanner thread
extern void *scanner_work(void *_args);
extern void go_through_file(void *_args, int prev_length);
extern void merge_maps(HashMap* main_map, HashMap* temp_map);

#endif 
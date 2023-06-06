#include <sys/stat.h>

typedef struct search_result {
	char* key; 
	int frequency; 
} search_result;

typedef struct file{
	char file_name[256]; 
	time_t mod_time; 
    int length;
} scanned_file;



// start of program
extern void map_init();
extern void scanner_init();

// scanner thread
extern void *scanner_work(void *_args);
extern void go_through_file(void *_args, int prev_length);

// pretraga frekvencije reci
extern search_result* map_get_frequency(char *word);
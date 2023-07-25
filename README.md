## Homework 4 for Uni (Operating Systems)

The purpose of this homework for to create a program that keeps track of word frequency. The user inputs path to files
and the words which frequencies he wants to read. The files are processes paralelly. The words are stored in one
central hash table which every thread has access to.

### User interaction

** _count_ <path/to/file> ** <br>
This command adds a new file to the system. A scanner thread for the file is created. 

** _stopwords_ <path/to/file> ** <br>
This command adds a file with stop words that will be ignored when scanning the files in the future.

** _stop_ **
Stops the whole proram and all the threads (Main thread and Scanner threads).

### Scanner thread
This thread keeps a track of the file it has been assigned. First, it reads the whole file (assuming the file only has
ASCII characters - words divided by spaces, tab or enter). Afterwards, every 5 seconds it checks if the file has been
modified. If it has, it reads the file again. Modification in our use-case can only  be addition of new words. 

### Hash Map
We have created a custom hash map (an array of lists to deal with collisions). For this purpose we have used Dan Bernstein
hash function. The code should be improved so that every index in the map has it's own mutex so it can be fully concurrent. 

### Running in VS Code
1. Install extension code runner
2. Go to Settings 
3. Type 'Code runner' into search bar
4. Enable 'Code Runner: Run in Terminal'

Run program from the terminal:
```
gcc main.c map.c -o program
./program
```
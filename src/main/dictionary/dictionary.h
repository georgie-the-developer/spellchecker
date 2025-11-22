/*
Is responsible for:

- loading a dictionariy into memory
- checking if a given word is in dictionary (hashtable)
- suggesting a most sutable word substitute if the word is not in the dictionary (optional; BK-tree)
- freeing a dictionary from memory

*/

#ifndef DICTIONARY
#define DICTIONARY

#include <stdbool.h>
#include <iostream>

extern const char *DEFAULT_DICTIONARY_FILENAME;
extern const int DICT_HASHTABLE_SIZE;

struct hashtable_entry
{
    char *word;
    struct hashtable_entry *next;
};
struct checkResult
{
    int line;
    int ch_pos;
    bool correct;
    int *word;
};
/* !! OVERLOADING EXAMPLE !! */
void load_dict(const char *filename); // loads a custom dictionary into RAM
void load_dict();
void free_dict(void);

void store(char *word);

int hash(char *word); // returns a hash for a given word

bool check(char *word); // checks whether the word is in dictionary or not

void print_dict(void); // debug-only

#endif
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
#include "../../helpers/hash_string/hash_string.h"

extern const char *DEFAULT_DICTIONARY_FILENAME;
extern const int DICT_HASHTABLE_SIZE;

struct hashtable_entry
{
    char *word;
    struct hashtable_entry *next;
};
/* !! OVERLOADING EXAMPLE !! */
void load_dict(const char *filename); // loads a custom dictionary into RAM
void load_dict();
void free_dict(void);

void store(char *word);

bool check(char *word); // checks whether the word is in dictionary or not

void print_dict(void); // debug-only

#endif
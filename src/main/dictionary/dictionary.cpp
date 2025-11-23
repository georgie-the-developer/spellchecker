#include "./dictionary.h"
#include <stdlib.h>
#include <string.h>

#include "../../helpers/get_word/get_word.h"

#ifdef _WIN32
const char *DEFAULT_DICTIONARY_FILENAME = "..\\spellchecker\\resources\\default_dictionary.txt";
#else
const char *DEFAULT_DICTIONARY_FILENAME = "../spellchecker/resources/default_dictionary.txt";
#endif

const int DICT_HASHTABLE_SIZE = 100000;
struct hashtable_entry *dictionary[DICT_HASHTABLE_SIZE] = {NULL};

void load_dict(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    char *word;
    if (!file)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
        system("pause");
    }
    while ((word = get_word(file)) != NULL)
    {
        // store the word to hashtable (call a separate function)
        store(word);
    }
    fclose(file);
    print_dict();
}

void load_dict()
{
    load_dict(DEFAULT_DICTIONARY_FILENAME);
}

bool check(char *word)
{
    int key = hash_string(word, DICT_HASHTABLE_SIZE); // get the hash
    hashtable_entry *current = dictionary[key];       // get the pointer to the first element
    while (current != NULL)
    {
        if (!strcmp(current->word, word)) // if the match is found, return true
            return true;
        current = current->next; // go down the linked list
    }
    return false;
}

// stores a given word in the hashtable
void store(char *word)
{
    if (check(word))
    {
        free(word);
        return;
    }
    int key = hash_string(word, DICT_HASHTABLE_SIZE);
    hashtable_entry *entry = (hashtable_entry *)malloc(sizeof(hashtable_entry));
    entry->word = word;
    entry->next = dictionary[key];
    dictionary[key] = entry;
}

// debug-only
void print_dict(void)
{
    int counter = 0;
    for (int i = 0; i < DICT_HASHTABLE_SIZE; i++)
    {
        if (dictionary[i] != NULL)
        {
            hashtable_entry *current = dictionary[i];
            while (current != NULL)
            {
                // printf("%s at pos %d; ", current->word, i);
                counter++;
                current = current->next;
            }
        }
    }
    printf("Number of records: %d \n", counter);
}
// hashtable freeing logic
void free_dict(void)
{
    for (int i = 0; i < DICT_HASHTABLE_SIZE; i++)
    {
        if (dictionary[i] != NULL)
        {
            hashtable_entry *current = dictionary[i];
            while (current != NULL)
            {
                hashtable_entry *next = current->next;
                free(current->word);
                free(current);
                current = next;
            }
        }
    }
}

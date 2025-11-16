#include "./dictionary.h"
#include <stdlib.h>
#include <string.h>

#include "../../helpers/get_word/get_word.h"

const char *DEFAULT_DICTIONARY_FILENAME = "../spellchecker/resources/default_dictionary.txt";
const int HASH_SIZE = 100000;
struct hashtable_entry *hashtable[HASH_SIZE];
void load(const char *filename)
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

void load()
{
    load(DEFAULT_DICTIONARY_FILENAME);
}

bool check(char *word)
{
    int key = hash(word); // get the hash
    if (hashtable[key] != NULL)
    {
        hashtable_entry *current = hashtable[key]; // get the pointer to the first element
        while (current != NULL)
        {
            if (!strcmp(current->word, word)) // if the match is found, return true
                return true;
            current = current->next; // go down the linked list
        }
    }
    return false;
}

// stores a given word in the hashtable
void store(char *word)
{
    int key = hash(word);
    struct hashtable_entry *entry;
    struct hashtable_entry *current;
    if (hashtable[key] == NULL)
    {
        entry = (hashtable_entry *)malloc(sizeof(hashtable_entry));
        entry->word = word;
        entry->next = NULL;
        hashtable[key] = entry;
    }
    else
    {
        current = hashtable[key];
        // iterate over the linked list
        while (true)
        {
            if (!strcmp(current->word, word))
            {
                free(word);
                return; // return in case the word is already stored
            }

            if (current->next == NULL)
                break;
            else
                current = current->next;
        }
        // only then initialize our variable
        entry = (hashtable_entry *)malloc(sizeof(hashtable_entry));
        entry->word = word;
        entry->next = NULL;
        current->next = entry;
    }
}

// returns a hash for the provided word
int hash(char *str)
{
    unsigned long hash = 5381;
    char c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash % 100000; // limit range
}
// debug-only
void print_dict(void)
{
    int counter = 0;
    for (int i = 0; i < HASH_SIZE; i++)
    {
        if (hashtable[i] != NULL)
        {
            hashtable_entry *current = hashtable[i];
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
    for (int i = 0; i < HASH_SIZE; i++)
    {
        if (hashtable[i] != NULL)
        {
            hashtable_entry *current = hashtable[i];
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

// standard libraries
#include <iostream>
#include <time.h>
#include <string.h>
// custom files
#include "../dictionary/dictionary.h"
#include "./spellcheck.h"
#include "../../helpers/get_string/get_string.h"
#include "../../helpers/get_word/get_word.h"
#include "../../helpers/case_fold/case_fold.h"
#include "../../helpers/utf8_strlen/utf8_strlen.h"
#include "../../helpers/hash_string/hash_string.h"

const int MAX_ERROR_COUNT = 50;

const int MISSPELL_HASHTABLE_SIZE = 10000;
misspelling_entry *misspellings_hashtable[MISSPELL_HASHTABLE_SIZE];

int main(int argc, char **argv)
{
    char *filename;

    clock_t begin, end;
    double dictionary_load_time;
    double spellcheck_time;
    bool result;

    // check argc
    if (argc > 2)
    {
        printf("Too many arguments provided\n");
        return 1;
    }
    load_casefold_table(); // load_dict a casefold table
    // call load_dict on dictionary file
    begin = clock();
    if (argc == 2)
    {
        load_dict(argv[1]); // load_dict custom dictionary
    }
    else
    {
        load_dict(); // load_dict default dictionary
    }
    end = clock();
    dictionary_load_time = (double)(end - begin) / CLOCKS_PER_SEC * 1000;
    printf("Time spent loading dictionary: %f ms\n", dictionary_load_time);
    // spellcheck
    while (true)
    {
        printf("==========\n");
        // prompt user for a filename
        filename = get_string((char *)"Provide a filename (to exit a program, type in \"exit\", \"quit\" or \"q\"):");
        if (!strcmp(filename, "exit") || !strcmp(filename, "quit") || !strcmp(filename, "q"))
        {
            free(filename);
            break;
        }

        printf("\n");
        begin = clock();
        result = spellcheck(filename);
        if (!result)
            continue;
        end = clock();
        free(filename);
        printf("\n");
        spellcheck_time = (double)(end - begin) / CLOCKS_PER_SEC * 1000;
        printf("Time spent checking the file: %f ms\n\n", spellcheck_time);
    }
    free_dict();
    free_casefold_table();
    free_misspelling_table();
    return 0;
}

bool spellcheck(char *filename)
{
    // load_dict file
    bool correct = false;
    int word_count = 0;
    int error_count = 0;
    int line = 1;
    int col = 1;
    char *word;

    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        perror("Failed to open the file");
        return false;
    }

    while ((word = get_word(file, &line, &col)) != NULL)
    {
        word_count++;
        correct = check(word);
        // we should use a custom strlen
        if (!correct)
        {
            store_misspelling_record(word);
            if (error_count <= MAX_ERROR_COUNT)
            {
                printf("Misspelling at line %d, col %d: %s\n", line, col - utf8_strlen(word), word);
            }
            error_count++;
        }
        free(word);
    }
    if (error_count > MAX_ERROR_COUNT)
    {
        printf("...\n");
        printf("There are more than %d misspellings in this text. The rest of the error messages were suppressed in order to keep the console clean\n\n", MAX_ERROR_COUNT);
    }
    printf("Number of misspellings: %d, of them unique: %d\n", error_count, misspelling_record_count());
    printf("Number of correctly spelled words: %d \n", word_count - error_count);
    printf("Total number of words: %d \n", word_count);

    fclose(file);

    return true;
}

bool check_misspelling_record(char *word)
{
    int key = hash_string(word, MISSPELL_HASHTABLE_SIZE);     // get the hash
    misspelling_entry *current = misspellings_hashtable[key]; // get the pointer to the first element
    while (current != NULL)
    {
        if (!strcmp(current->word, word)) // if the match is found, return true
            return true;
        current = current->next; // go down the linked list
    }
    return false;
}

// stores a given word in the hashtable
void store_misspelling_record(char *word)
{
    int key = hash_string(word, MISSPELL_HASHTABLE_SIZE);
    if (check_misspelling_record(word))
    {
        misspelling_entry *current = misspellings_hashtable[key];
        while (strcmp(current->word, word))
            current = current->next;
        current->count++;
        return;
    }
    misspelling_entry *entry = (misspelling_entry *)malloc(sizeof(misspelling_entry));
    int word_len = strlen(word);
    entry->word = (char *)malloc(word_len);
    memcpy(entry->word, word, word_len);
    entry->count = 1;
    entry->next = misspellings_hashtable[key];
    misspellings_hashtable[key] = entry;
}
void free_misspelling_table()
{
    for (int i = 0; i < MISSPELL_HASHTABLE_SIZE; i++)
    {
        if (misspellings_hashtable[i] != NULL)
        {
            misspelling_entry *current = misspellings_hashtable[i];
            while (current != NULL)
            {
                misspelling_entry *next = current->next;
                free(current->word);
                free(current);
                current = next;
            }
        }
    }
}

int misspelling_record_count()
{
    int count = 0;
    for (int i = 0; i < MISSPELL_HASHTABLE_SIZE; i++)
    {
        if (misspellings_hashtable[i] != NULL)
        {
            misspelling_entry *current = misspellings_hashtable[i];
            while (current != NULL)
            {
                current = current->next;
                count++;
            }
        }
    }
    return count;
}

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
// ./resources/texts/austen.txt
const int MAX_ERROR_COUNT = 50;
// ./resources/default_dictionary.txt
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
    printf("Number of misspellings: %d \n", error_count);
    printf("Number of correctly spelled words: %d \n", word_count - error_count);
    printf("Total number of words: %d \n", word_count);

    fclose(file);

    return true;
}

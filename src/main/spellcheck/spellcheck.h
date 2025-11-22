/*
Is responsible for:

- receciving command-line arguments // done
- checking if not too much arguments provided // done
- calling load_dict() (dictionary.h) function on dictionary file // done
- outputting how much time it took to load_dict the dictionary // done
- prompting user for a file to spellcheck (should be implemented with loop logic) // done

- asking if the user wants to use a word suggestion feature on misspelled words (optional; y/n only)
- looping through the file while keeping track of the lines and char positions (char positions will be a challenge, actually)
- calling check() (dictionary.h) function on each word and outputting the result. If the word is not found, it is considered
    misspelled and therefore information about it is outputted.
- outputting how much time it took for the check to happen
- displaying how much words were in the text, how much were misspelled, how much are correct
    and what are top 10 most misspelled words in the text

- exiting if user provides "exit", "q" or "quit" instead of a file name // done

*/
#include <stdbool.h>
// standard libraries
#include <string.h>
// custom files
#include "../../helpers/get_word/get_word.h"
#include "../../helpers/case_fold/case_fold.h"
#include "../../helpers/utf8_strlen/utf8_strlen.h"
#include "../../helpers/hash_string/hash_string.h"
#include "../dictionary/dictionary.h"

#ifndef SPELLCHECK
#define SPELLCHECK

// returns true if the spellcheck was successfull, and false in case of an error
bool spellcheck(char *filename);
typedef struct misspelling_entry
{
    char *word;
    int count;
    misspelling_entry *next;
} misspelling_entry;

bool check_misspelling_record(char *word);
void store_misspelling_record(char *word);
void free_misspelling_table();
int misspelling_record_count();

#endif
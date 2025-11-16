#ifndef GET_WORD
#define GET_WORD

#include <iostream>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "../case_fold/case_fold.h"
#include "../handle_unexpected_nullptr/handle_unexpected_nullptr.h"

// OVERLOADING!!!
char *get_word(FILE *file);
char *get_word(FILE *file, int *p_line, int *p_ch_pos);

// internal encoding-related functions
static int detect_utf8_length(unsigned char c);

#endif
#ifndef CASE_FOLD
#define CASE_FOLD

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct folded_record
{
    struct folded_record *next;
    uint32_t *cps; // code points
    uint32_t key_cp;
    char cp_count; // code points count
} folded_record;

extern const char *CASEFOLD_FILE;

char *case_fold(const unsigned char *seq, int len);

uint32_t utf8_decode(const unsigned char *seq, int len);
int utf8_encode(uint32_t cp, unsigned char *out);

uint32_t *casefold_lookup(uint32_t cp, char *cp_count);
uint32_t hash_uint32(uint32_t num);

void load_casefold_table();
bool set_casefold_table_record(folded_record *record);
folded_record *parse_casefold_record(FILE *file);
char *get_record_line(FILE *file);

void free_casefold_table();

#endif

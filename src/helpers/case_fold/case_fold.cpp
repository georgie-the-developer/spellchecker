#include "./case_fold.h"
#include "../handle_unexpected_nullptr/handle_unexpected_nullptr.h"

#ifdef _WIN32
const char *CASEFOLD_FILE = "..\\spellchecker\\src\\helpers\\case_fold\\case_folding.txt";
#else
const char *CASEFOLD_FILE = "../spellchecker/src/helpers/case_fold/case_folding.txt";
#endif

static const short CASEFOLD_TABLE_SIZE = 1000;
static folded_record *casefold_table[CASEFOLD_TABLE_SIZE] = {NULL};

void free_casefold_table()
{
    for (int i = 0; i < CASEFOLD_TABLE_SIZE; i++)
    {
        if (casefold_table[i] != NULL)
        {
            folded_record *current = casefold_table[i];
            while (current != NULL)
            {
                folded_record *next = current->next;
                free(current->cps);
                free(current);
                current = next;
            }
        }
    }
}
// loads case folding table from file
void load_casefold_table()
{
    FILE *file = fopen(CASEFOLD_FILE, "rb");
    handle_unexpected_nullptr(file, "filesystem");
    // parse as long as there is a file to parse
    while (1)
    {
        folded_record *record = parse_casefold_record(file);

        if (!record)
            break;

        // store record to the hash table
        bool ok = set_casefold_table_record(record);

        if (!ok)
        {
            printf("A memory error happened.\n");
            system("pause");
            exit(EXIT_FAILURE);
        }
    }
    fclose(file);
}
bool set_casefold_table_record(folded_record *record)
{
    uint32_t hash = hash_uint32(record->key_cp);

    // insert the record at the beginning of the hash table linked list
    record->next = casefold_table[hash];
    casefold_table[hash] = record;

    return true;
}
// returns a dynamically allocated record struct that contains parsed data from a single non-comment line
folded_record *parse_casefold_record(FILE *file)
{
    folded_record *record = (folded_record *)malloc(sizeof(folded_record));
    handle_unexpected_nullptr(record, "malloc");

    record->cp_count = 1;

    record->cps = (uint32_t *)malloc(sizeof(uint32_t) * record->cp_count);
    handle_unexpected_nullptr(record->cps, "malloc");

    char *line = get_record_line(file);
    if (line == NULL)
    {
        free(record->cps);
        free(record);
        return NULL;
    }
    char *current = strtok(line, ";");
    char column = 1;
    while (current != NULL)
    {
        switch (column)
        {
        case 1:
            record->key_cp = (uint32_t)strtol(current, NULL, 16);
            column++;
            break;
        case 2:
            column++;
            break;
        case 3:
            char *value_cp = strtok(current, " ");
            while (value_cp != NULL)
            {
                record->cps = (uint32_t *)realloc(record->cps, record->cp_count * sizeof(uint32_t));
                handle_unexpected_nullptr(record->cps, "realloc");

                record->cps[record->cp_count - 1] = (uint32_t)strtol(value_cp, NULL, 16);
                record->cp_count++;

                value_cp = strtok(NULL, " ");
            }
            break;
        }
        current = strtok(NULL, ";");
    }
    free(line);
    return record;
    // split the line by ';' and ' '
}
// returns a spaceless line without a leading comment
char *get_record_line(FILE *file)
{
    unsigned char buf[1];
    bool is_comment = false;
    int capacity = 1;
    char *line = (char *)malloc(capacity);
    handle_unexpected_nullptr(line, "malloc");

    char column = 1;
    while (fread(buf, 1, 1, file))
    {
        // return the non-comment line
        if (buf[0] == '#')
        {
            is_comment = true;
        }
        else if (buf[0] == '\n')
        {
            is_comment = false;
            if (capacity > 1)
            {
                line[capacity - 1] = '\0';
                return line;
            }
            continue;
        }
        if (is_comment)
            continue;

        if (buf[0] != ' ' || column == 3)
        {
            if (buf[0] == ';')
                column++;

            line[capacity - 1] = buf[0];

            line = (char *)realloc(line, ++capacity);
            handle_unexpected_nullptr(line, "realloc");
        }
    }
    free(line);
    return NULL;
}
// so we need a parsing function
// a table recording one (probably)
// and to make our parsing function lesmallocs complex we need to split it into subfunctions

/* -----done----- */

// fold one UTF-8 sequence
char *case_fold(const unsigned char *seq, int len)
{

    char res_cap = 1;
    char *res = (char *)malloc(res_cap);
    handle_unexpected_nullptr(res, "malloc");

    unsigned char current_seq[4];
    // 1. decode a given sequence
    uint32_t key_cp = utf8_decode(seq, len);

    // 2. look up for a corresponding sequence(s)
    char cp_count;
    uint32_t *value_cps = casefold_lookup(key_cp, &cp_count); // we'll get to that later
    // 3. encode the corresponding sequence(s)
    if (cp_count > 0)
    {

        for (int i = 0; i < cp_count; i++)
        {
            char out_len = utf8_encode(value_cps[i], current_seq);
            res_cap += out_len;
            res = (char *)realloc(res, res_cap);
            handle_unexpected_nullptr(res, "realloc");

            memcpy(res + res_cap - out_len - 1, current_seq, out_len);
        }
        // 4. return the new sequence
    }
    // return the old sequence if no match found
    else
    {
        res_cap += len;
        res = (char *)realloc(res, res_cap);
        handle_unexpected_nullptr(res, "realloc");

        memcpy(res, seq, len);
    }
    free(value_cps);
    res[res_cap - 1] = '\0';
    return res;
}

// takes in a key code point, and returns its casefolded equivalent; writes the count of the cps to the passed pointer
uint32_t *casefold_lookup(uint32_t cp, char *cp_count)
{

    uint32_t *res;
    // ASCII fast path
    if (cp >= 'A' && cp <= 'Z')
    {
        res = (uint32_t *)malloc(sizeof(uint32_t));
        handle_unexpected_nullptr(res, "malloc");
        res[0] = cp + 0x20; // a-z
        *cp_count = 1;
    }
    else
    {
        folded_record *record = casefold_table[hash_uint32(cp)];

        while (record != NULL && cp != record->key_cp)
            record = record->next;

        if (record == NULL)
        {
            *cp_count = 0;
            return NULL;
        }

        res = (uint32_t *)malloc(sizeof(uint32_t) * record->cp_count);
        handle_unexpected_nullptr(res, "malloc");
        memcpy(res, record->cps, record->cp_count * sizeof(uint32_t));
        *cp_count = record->cp_count;
    }
    return res;
}
uint32_t hash_uint32(uint32_t num)
{
    return num % CASEFOLD_TABLE_SIZE;
}

// decode utf8 to unicode code point
uint32_t utf8_decode(const unsigned char *seq, int len)
{
    switch (len)
    {
    case 1:
        return seq[0];
    case 2:
        return ((seq[0] & 0x1F) << 6) | (seq[1] & 0x3F);
    case 3:
        return ((seq[0] & 0x0F) << 12) | ((seq[1] & 0x3F) << 6) | (seq[2] & 0x3F);
    case 4:
        return ((seq[0] & 0x07) << 18) | ((seq[1] & 0x3F) << 12) | ((seq[2] & 0x3F) << 6) | (seq[3] & 0x3F);
    default:
        return 0xFFFD; // replacement char
    }
}

// encode code point back to utf8
// returns length, overwrites the contents of out
int utf8_encode(uint32_t cp, unsigned char *out)
{
    if (cp <= 0x7F)
    {
        out[0] = cp;
        return 1;
    }
    else if (cp <= 0x7FF)
    {
        out[0] = 0xC0 | (cp >> 6);
        out[1] = 0x80 | (cp & 0x3F);
        return 2;
    }
    else if (cp <= 0xFFFF)
    {
        out[0] = 0xE0 | (cp >> 12);
        out[1] = 0x80 | ((cp >> 6) & 0x3F);
        out[2] = 0x80 | (cp & 0x3F);
        return 3;
    }
    else
    {
        out[0] = 0xF0 | (cp >> 18);
        out[1] = 0x80 | ((cp >> 12) & 0x3F);
        out[2] = 0x80 | ((cp >> 6) & 0x3F);
        out[3] = 0x80 | (cp & 0x3F);
        return 4;
    }
}
// as a result, we want a hashtable that stores uppercase codepoint and its lowercase equivalent
// as key-value pairs. Probably we should implement an integer hash function
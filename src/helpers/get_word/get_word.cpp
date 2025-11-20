#include "./get_word.h"

static int detect_utf8_length(unsigned char c)
{
    if ((c & 0x80) == 0x00)
        return 1;
    else if ((c & 0xE0) == 0xC0)
        return 2;
    else if ((c & 0xF0) == 0xE0)
        return 3;
    else if ((c & 0xF8) == 0xF0)
        return 4;
    return 1; // fallback
}

bool is_valid_utf8_sequence(const unsigned char *seq, int len)
{
    // this code is gpt-generated. I won't even try to explain it as it
    // contains way too many logical operations
    if (len == 1)
    {
        // 0xxxxxxx
        return (seq[0] & 0x80) == 0x00;
    }
    else if (len == 2)
    {
        // 110xxxxx 10xxxxxx
        return (seq[0] & 0xE0) == 0xC0 &&
               (seq[1] & 0xC0) == 0x80 &&
               (seq[0] & 0xFE) != 0xC0; // reject overlong
    }
    else if (len == 3)
    {
        // 1110xxxx 10xxxxxx 10xxxxxx
        if (!((seq[0] & 0xF0) == 0xE0 &&
              (seq[1] & 0xC0) == 0x80 &&
              (seq[2] & 0xC0) == 0x80))
            return false;
        uint32_t cp = ((seq[0] & 0x0F) << 12) |
                      ((seq[1] & 0x3F) << 6) |
                      (seq[2] & 0x3F);
        return cp >= 0x800 && !(cp >= 0xD800 && cp <= 0xDFFF);
    }
    else if (len == 4)
    {
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        if (!((seq[0] & 0xF8) == 0xF0 &&
              (seq[1] & 0xC0) == 0x80 &&
              (seq[2] & 0xC0) == 0x80 &&
              (seq[3] & 0xC0) == 0x80))
            return false;
        uint32_t cp = ((seq[0] & 0x07) << 18) |
                      ((seq[1] & 0x3F) << 12) |
                      ((seq[2] & 0x3F) << 6) |
                      (seq[3] & 0x3F);
        return cp >= 0x10000 && cp <= 0x10FFFF;
    }
    return false;
}

char *get_word(FILE *file)
{
    int line = 0;
    int ch_pos = 0;
    return get_word(file, &line, &ch_pos);
}
char *get_word(FILE *file, int *line, int *col)
{
    /*
    should read a word from a file meanwhile:
        - working with each utf8 code point:
            - validating each code point, replacing the invalid ones with U+FFFD (�)
            - lowercasing each code point if there is a lowercase equivalent of it
        - incrementing ine and col for each code point
        - terminating the word when a delimiter is reached
    */

    // buffers
    unsigned char buf[1]; // used to get bytes from file
    unsigned char seq[4]; // used to temporarily store a utf8 sequence
    // single code point related
    unsigned char c; // a copy of buf[0] (used twice)
    int char_len;    // the length of the sequence

    // word-related
    int capacity = 1;
    char *word = (char *)malloc(capacity);
    // check if the word pointer is valid
    handle_unexpected_nullptr(word, "malloc");
    // check if the file pointer is valid
    handle_unexpected_nullptr(file, "filesystem");

    // read one byte into buffer
    while (fread(buf, 1, 1, file) != 0)
    {
        // detect the length of the codepoint
        c = buf[0];
        char_len = detect_utf8_length(c);
        // read the rest of the bytes into seq
        seq[0] = c; // first byte already read
        if (char_len > 1)
        {
            size_t n = fread(seq + 1, 1, char_len - 1, file);
            // if the code point is invalid, handle as delimeter
            if (n != (size_t)(char_len - 1) || !is_valid_utf8_sequence(seq, char_len))
            {
                if (capacity > 1)
                {
                    word[capacity - 1] = '\0';
                    return word;
                }
                continue;
            }
        }
        else if (isascii(buf[0]) && !isalpha(buf[0]) && (buf[0] != '-' && buf[0] != '\''))
        {
            if (capacity > 1)
            {
                ungetc(buf[0], file);
                word[capacity - 1] = '\0';
                return word;
            }
            if (buf[0] == '\n')
            {
                (*line)++;
                (*col) = 1;
            }
            else
            {
                (*col)++;
            }
            continue;
        }

        // folded version
        char *new_seq = case_fold(seq, char_len);
        int new_len = strlen(new_seq);
        capacity += new_len;
        word = (char *)realloc(word, capacity);
        handle_unexpected_nullptr(word, "realloc");
        memcpy(word + capacity - new_len - 1, new_seq, new_len);
        free(new_seq);

        // unfolded version
        // capacity += char_len;
        // word = (char *)realloc(word, capacity);
        // handle_unexpected_nullptr(word, "realloc");
        // memcpy(word + capacity - char_len - 1, seq, char_len);

        (*col)++;
    }
    if (capacity > 1)
    {
        word[capacity - 1] = '\0';
        return word;
    }
    free(word);
    return NULL;
}

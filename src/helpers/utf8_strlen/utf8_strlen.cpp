#include "./utf8_strlen.h"
int detect_utf8_length(unsigned char c)
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

int utf8_strlen(unsigned char *str)
{
    int len = 0;
    while ((*str) != '\0')
    {
        char char_len = detect_utf8_length(*str);
        str += char_len;
        len++;
    }
    return len;
}
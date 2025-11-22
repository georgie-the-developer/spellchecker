#include "./hash_string.h"
int hash_string(char *str, size_t limit)
{
    unsigned long hash = 5381;
    char c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash % limit; // limit range
}
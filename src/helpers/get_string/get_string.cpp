#include "./get_string.h"
#include <iostream>
#include "../handle_unexpected_nullptr/handle_unexpected_nullptr.h"

char *get_string(char *message)
{
    char current;
    char *string;
    int capacity = 1;

    printf("%s\n", message);
    string = (char *)malloc(capacity);
    handle_unexpected_nullptr(string, "malloc");
    do
    {
        current = getchar();
        string[capacity - 1] = current;
        string = (char *)realloc(string, ++capacity);
        handle_unexpected_nullptr(string, "realloc");
    } while (current != '\n');

    string[capacity - 2] = '\0';

    return string;
}
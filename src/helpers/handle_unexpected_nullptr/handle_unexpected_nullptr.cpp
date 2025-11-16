#include "./handle_unexpected_nullptr.h"

void handle_unexpected_nullptr(void *ptr, const char *errmsg)
{
    if (ptr == nullptr)
    {
        perror(errmsg);
        system("pause");
        exit(EXIT_FAILURE);
    }
}
#include "./main.h"
int main(int argc, char **argv)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
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
        load_dict(argv[1]); // load custom dictionary
    }
    else
    {
        load_dict(); // load default dictionary
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
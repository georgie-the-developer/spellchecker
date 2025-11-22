#include "./spellcheck.h"

const int MAX_ERROR_COUNT = 50;

const int MISSPELL_HASHTABLE_SIZE = 10000;
misspelling_entry *misspellings_hashtable[MISSPELL_HASHTABLE_SIZE];

bool spellcheck(char *filename)
{
    // load_dict file
    bool correct = false;
    int word_count = 0;
    int error_count = 0;
    int line = 1;
    int col = 1;
    char *word;

    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        perror("Failed to open the file");
        return false;
    }

    while ((word = get_word(file, &line, &col)) != NULL)
    {
        word_count++;
        correct = check(word);
        // we should use a custom strlen
        if (!correct)
        {
            store_misspelling_record(word);
            if (error_count <= MAX_ERROR_COUNT)
            {
                printf("Misspelling at line %d, col %d: %s\n", line, col - utf8_strlen(word), word);
            }
            error_count++;
        }
        free(word);
    }
    if (error_count > MAX_ERROR_COUNT)
    {
        printf("...\n");
        printf("There are more than %d misspellings in this text. The rest of the error messages were suppressed in order to keep the console clean\n\n", MAX_ERROR_COUNT);
    }
    printf("Number of misspellings: %d, of them unique: %d\n", error_count, misspelling_record_count());
    printf("Number of correctly spelled words: %d \n", word_count - error_count);
    printf("Total number of words: %d \n", word_count);

    fclose(file);

    return true;
}

bool check_misspelling_record(char *word)
{
    int key = hash_string(word, MISSPELL_HASHTABLE_SIZE);     // get the hash
    misspelling_entry *current = misspellings_hashtable[key]; // get the pointer to the first element
    while (current != NULL)
    {
        if (!strcmp(current->word, word)) // if the match is found, return true
            return true;
        current = current->next; // go down the linked list
    }
    return false;
}

// stores a given word in the hashtable
void store_misspelling_record(char *word)
{
    int key = hash_string(word, MISSPELL_HASHTABLE_SIZE);
    if (check_misspelling_record(word))
    {
        misspelling_entry *current = misspellings_hashtable[key];
        while (strcmp(current->word, word))
            current = current->next;
        current->count++;
        return;
    }
    misspelling_entry *entry = (misspelling_entry *)malloc(sizeof(misspelling_entry));
    int word_len = strlen(word);
    entry->word = (char *)malloc(word_len);
    memcpy(entry->word, word, word_len);
    entry->count = 1;
    entry->next = misspellings_hashtable[key];
    misspellings_hashtable[key] = entry;
}
void free_misspelling_table()
{
    for (int i = 0; i < MISSPELL_HASHTABLE_SIZE; i++)
    {
        if (misspellings_hashtable[i] != NULL)
        {
            misspelling_entry *current = misspellings_hashtable[i];
            while (current != NULL)
            {
                misspelling_entry *next = current->next;
                free(current->word);
                free(current);
                current = next;
            }
        }
    }
}

int misspelling_record_count()
{
    int count = 0;
    for (int i = 0; i < MISSPELL_HASHTABLE_SIZE; i++)
    {
        if (misspellings_hashtable[i] != NULL)
        {
            misspelling_entry *current = misspellings_hashtable[i];
            while (current != NULL)
            {
                current = current->next;
                count++;
            }
        }
    }
    return count;
}

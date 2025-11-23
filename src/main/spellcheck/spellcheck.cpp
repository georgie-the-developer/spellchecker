#include "./spellcheck.h"

const int MAX_ERROR_COUNT = 50;

const int MISSPELL_HASHTABLE_SIZE = 1000000;
static misspelling_entry *misspelling_hashtable[MISSPELL_HASHTABLE_SIZE];

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
        printf("There are more than %d misspellings in this text. The rest of the error messages were suppressed in order to keep the console clean\n", MAX_ERROR_COUNT);
    }
    printf("\n");
    printf("Number of misspellings: %d, of them unique: %d\n", error_count, misspelling_record_count());
    printf("Number of correctly spelled words: %d \n", word_count - error_count);
    printf("Total number of words: %d \n", word_count);

    fclose(file);

    return true;
}

bool check_misspelling_record(char *word)
{
    int key = hash_string(word, MISSPELL_HASHTABLE_SIZE);
    misspelling_entry *current = misspelling_hashtable[key]; // get the pointer to the first element
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
        return;
    misspelling_entry *entry = (misspelling_entry *)malloc(sizeof(misspelling_entry));
    size_t len = strlen(word) + 1;
    entry->word = (char *)malloc(len);
    memcpy(entry->word, word, len);

    entry->next = misspelling_hashtable[key];
    misspelling_hashtable[key] = entry;
}
void free_misspelling_table()
{
    for (int i = 0; i < MISSPELL_HASHTABLE_SIZE; i++)
    {
        misspelling_entry *current = misspelling_hashtable[i];
        while (current != NULL)
        {
            misspelling_entry *next = current->next;
            free(current->word);
            free(current);
            current = next;
        }
        misspelling_hashtable[i] = NULL;
    }
}

int misspelling_record_count()
{
    int count = 0;
    for (int i = 0; i < MISSPELL_HASHTABLE_SIZE; i++)
    {
        misspelling_entry *current = misspelling_hashtable[i];
        while (current != NULL)
        {
            current = current->next;
            count++;
        }
    }
    return count;
}
// ./resources/polish_dictionary.txt
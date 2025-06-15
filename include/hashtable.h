#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol.h"

// An entry in the hash table
typedef struct ht_entry {
    char *key;          // The key (string)
    Symbol *value;      // Pointer to the Symbol struct
    struct ht_entry *next; // Pointer to the next entry in case of collision (chaining)
} ht_entry_t;

// The hash table structure
typedef struct {
    ht_entry_t **entries; // Array of pointers to entries (buckets)
    unsigned int size;    // The number of buckets in the hash table
    unsigned int count;   // The number of items currently in the hash table
} ht_t;

// Hash function (djb2)
static unsigned long hash_function(const char *str) 
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

// Creates a new hash table entry
static ht_entry_t *create_entry(const char *key, Symbol *value) 
{
    ht_entry_t *entry = (ht_entry_t *)malloc(sizeof(ht_entry_t));
    if (!entry) 
    {
        perror("Failed to allocate hash table entry");
        return NULL;
    }

    entry->key = strdup(key); // Duplicate the key string
    if (!entry->key) 
    {
        perror("Failed to duplicate key for hash table entry");
        free(entry);
        return NULL;
    }

    entry->value = value; // Store the pointer to the Symbol
    entry->next = NULL;
    return entry;
}

// Frees a hash table entry (but not the Symbol it points to)
static void free_entry(ht_entry_t *entry) 
{
    if (!entry) return;
    free(entry->key);
    free_sym(entry->value);
    free(entry);
}

ht_t *ht_create(unsigned int size) {
    if (size < 1) return NULL;

    ht_t *table = (ht_t *)malloc(sizeof(ht_t));
    if (!table) 
    {
        perror("Failed to allocate hash table");
        return NULL;
    }

    table->entries = (ht_entry_t **)calloc(size, sizeof(ht_entry_t *));
    if (!table->entries) 
    {
        perror("Failed to allocate hash table buckets");
        free(table);
        return NULL;
    }

    table->size = size;
    table->count = 0;
    return table;
}

void free_ht(ht_t *table) 
{
    if (!table) return;

    for (unsigned int i = 0; i < table->size; i++) 
    {
        ht_entry_t *entry = table->entries[i];
        while (entry != NULL) 
        {
            ht_entry_t *next_entry = entry->next;
            free_entry(entry);
            entry = next_entry;
        }
    }
    free(table->entries);
    free(table);
}

int ht_put(ht_t *table, const char *key, Symbol *value) 
{
    if (!table || !key) return -1; // Value can be NULL if intended

    unsigned int index = hash_function(key) % table->size;
    ht_entry_t *current_entry = table->entries[index];
    ht_entry_t *prev_entry = NULL;

    // Traverse the chain for this bucket
    while (current_entry != NULL) 
    {
        if (strcmp(current_entry->key, key) == 0) 
        {
            // Key found, update the value.
            free_sym(current_entry->value);
            current_entry->value = value;
            return 0; // Success
        }
        prev_entry = current_entry;
        current_entry = current_entry->next;
    }

    // Key not found, create a new entry
    ht_entry_t *new_entry = create_entry(key, value);
    if (!new_entry) 
    {
        return -1; // Failure in create_entry
    }

    // Add the new entry to the chain
    if (prev_entry == NULL) 
    { // Bucket was empty
        table->entries[index] = new_entry;
    } 
    else 
    { // Add to the end of the chain
        prev_entry->next = new_entry;
    }
    table->count++;

    // Optional: Implement resizing if load factor (table->count / table->size) gets too high.
    // For example: if ((float)table->count / table->size > 0.75) { ht_resize(table, table->size * 2); }

    return 0; // Success
}

Symbol *ht_get(ht_t *table, const char *key) 
{
    if (!table || !key) return NULL;

    unsigned int index = hash_function(key) % table->size;
    ht_entry_t *entry = table->entries[index];

    while (entry != NULL) 
    {
        if (strcmp(entry->key, key) == 0) 
        {
            Symbol* copy = copy_sym(entry->value); // Return a copy of the Symbol
            return copy;
        }
        entry = entry->next;
    }
    return NULL; // Key not found
}

void ht_remove(ht_t *table, const char *key) 
{
    if (!table || !key) return;

    unsigned int index = hash_function(key) % table->size;
    ht_entry_t *current_entry = table->entries[index];
    ht_entry_t *prev_entry = NULL;

    while (current_entry != NULL) 
    {
        if (strcmp(current_entry->key, key) == 0) 
        {
            if (prev_entry == NULL) 
            { // Entry is the head of the list
                table->entries[index] = current_entry->next;
            } 
            else 
            {
                prev_entry->next = current_entry->next;
            }
            free_entry(current_entry);
            table->count--;
            return;
        }
        prev_entry = current_entry;
        current_entry = current_entry->next;
    }
}

void ht_show(ht_t* table, int indent_level) 
{
    if (!table) 
    {
        printf("%*sHash table is NULL.\n", indent_level * 2, "");
        return;
    }
    printf("%*sHash Table Contents (Size: %u, Count: %u):\n", indent_level * 2, "", table->size, table->count);
    for (unsigned int i = 0; i < table->size; i++) 
    {
        ht_entry_t *entry = table->entries[i];
        if (entry != NULL) 
        {
            printf("%*sBucket %u: ", (indent_level + 1) * 2, "", i);
            while (entry != NULL) 
            {
                printf("[\"%s\" -> Symbol(Tag: %d, Name: %s)] ", entry->key, entry->value->tag, entry->value->name); 
                if (entry->next != NULL) 
                {
                    printf("-> ");
                }
                entry = entry->next;
            }
            printf("\n");
        }
    }
}
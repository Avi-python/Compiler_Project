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
unsigned long hash_function(const char *str);

// Creates a new hash table entry
ht_entry_t *create_entry(const char *key, Symbol *value);

// Frees a hash table entry (but not the Symbol it points to)
void free_entry(ht_entry_t *entry);

ht_t *ht_create(unsigned int size);
void free_ht(ht_t *table);
int ht_put(ht_t *table, const char *key, Symbol *value);
Symbol *ht_get(ht_t *table, const char *key);
void ht_remove(ht_t *table, const char *key);
void ht_show(ht_t* table, int indent_level, FILE* log_file);
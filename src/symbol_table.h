#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "hashtable.h"

#define TABLE_SIZE 128

typedef struct sym_entry {
    ht_t *table;
    struct sym_entry *prev;
    struct sym_entry *left;
    struct sym_entry *right;
} sym_t;

sym_t* create_symbol_table()
{
    sym_t *result = (sym_t *)malloc(sizeof(sym_t));
    if (!result) {
        perror("Failed to allocate symbol table entry");
        return NULL;
    }
    result->table = ht_create(TABLE_SIZE);
    result->prev = NULL;
    return result;
}

Symbol* get_symbol(sym_t* sym_table, const char* name)
{
    if (sym_table == NULL) return NULL;

    Symbol* s = ht_get(sym_table->table, name);

    if (s != NULL) return s;
    return get_symbol(sym_table->prev, name);
}

Symbol* insert_symbol(sym_t* sym_table, const char* name, int tag)
{
    ht_t *table = sym_table->table;
    Symbol *symbol = create_sym(name, tag);
    if(ht_put(table, name, symbol) == -1) // TODO : error handle not very complete
    {
        return NULL;
    }
    return copy_sym(symbol);
}

void show_symbol_table(sym_t* sym_table)
{
    // TODO : simple implementation
    ht_show(sym_table->table);
}

void free_symbol_table(sym_t* sym_table)
{
    if(sym_table == NULL) return;
    free_ht(sym_table->table);
    free_symbol_table(sym_table->left);
    free_symbol_table(sym_table->right);
    free(sym_table);
}


#endif // SYMBOL_TABLE_H
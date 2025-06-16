#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "hashtable.h" // Assumes hashtable.h and symbol.h are correct

#define TABLE_SIZE 128
#define INITIAL_CHILDREN_CAPACITY 4 // Define an initial capacity for children array

typedef struct sym_entry {
    ht_t *table;
    struct sym_entry *parent;
    int depth;
    struct sym_entry **children; // Array of child symbol tables
    int num_children;          // Number of children
    int children_capacity;     // Capacity of the children array
} sym_t;

sym_t* create_symbol_table();

// Function to add a child to a parent symbol table
void add_child_symbol_table(sym_t* parent_table, sym_t* child_table);

// Get symbol: Traverses up the parent chain
Symbol* get_symbol(sym_t* current_scope, const char* name);
Symbol* check_symbol_in_current_scope(sym_t* current_scope, const char* name);
Symbol* insert_symbol(sym_t* target_scope, const char* name, int tag);
void show_symbol_table_recursive(sym_t* sym_table, int indent_level, FILE* log_file);

// Wrapper to start showing the symbol table tree from a given root
void show_entire_symbol_tree(sym_t* root_sym_table, FILE* log_file);

// This function shows the current scope and its parents up to the global scope.
void show_current_and_parent_scopes(sym_t* current_scope, FILE* log_file);

// Frees the given symbol table and all its descendant children tables.
void free_all_symbol_tables(sym_t* sym_table);

#endif // SYMBOL_TABLE_H
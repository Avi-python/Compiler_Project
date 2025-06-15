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

sym_t* create_symbol_table() 
{
    sym_t *new_scope = (sym_t *)malloc(sizeof(sym_t));
    if (!new_scope) {
        perror("Failed to allocate symbol table entry");
        return NULL;
    }
    new_scope->table = ht_create(TABLE_SIZE);
    if (!new_scope->table) {
        perror("Failed to allocate hash table for new scope");
        free(new_scope);
        return NULL;
    }
    new_scope->parent = NULL;
    new_scope->depth = 0;

    // Initialize children members
    new_scope->children = (struct sym_entry **)malloc(INITIAL_CHILDREN_CAPACITY * sizeof(struct sym_entry *));
    if (!new_scope->children) {
        perror("Failed to allocate children array for symbol table");
        free_ht(new_scope->table); // Clean up hash table
        free(new_scope);
        return NULL;
    }
    new_scope->num_children = 0;
    new_scope->children_capacity = INITIAL_CHILDREN_CAPACITY;
    return new_scope;
}

// Function to add a child to a parent symbol table
void add_child_symbol_table(sym_t* parent_table, sym_t* child_table) 
{
    if (!parent_table || !child_table) return;

    child_table->parent = parent_table;
    child_table->depth = parent_table->depth + 1;

    if (parent_table->num_children >= parent_table->children_capacity) 
    {
        int new_capacity = parent_table->children_capacity == 0 ? INITIAL_CHILDREN_CAPACITY : parent_table->children_capacity * 2;
        struct sym_entry **new_children_array = (struct sym_entry **)realloc(parent_table->children, new_capacity * sizeof(struct sym_entry *));
        if (!new_children_array) {
            perror("Failed to reallocate children array for symbol table");
            // Child not added, parent remains as is. Potential leak if child isn't managed otherwise.
            return;
        }
        parent_table->children = new_children_array;
        parent_table->children_capacity = new_capacity;
    }
    parent_table->children[parent_table->num_children++] = child_table;
}

// Get symbol: Traverses up the parent chain
Symbol* get_symbol(sym_t* current_scope, const char* name)
{
    sym_t* temp_scope = current_scope;
    while (temp_scope != NULL) 
    {
        Symbol* s = ht_get(temp_scope->table, name);
        if (s != NULL) return s;
        temp_scope = temp_scope->parent; // Check parent scope
    }
    return NULL; // Not found in any scope up to global
}

Symbol* check_symbol_in_current_scope(sym_t* current_scope, const char* name)
{
    if (!current_scope || !current_scope->table) 
    {
        return NULL;
    }
    return ht_get(current_scope->table, name);
}

Symbol* insert_symbol(sym_t* target_scope, const char* name, int tag)
{
    if(!target_scope || !target_scope->table)
    {
        fprintf(stderr, "Error: Attempting to insert into a NULL or uninitialized symbol table.\n");
        return NULL;
    }

    Symbol *symbol = create_sym(name, tag); // Assuming create_sym is defined
    if (!symbol) {
        perror("Failed to create symbol for insertion");
        return NULL;
    }

    if (ht_put(target_scope->table, name, symbol) == -1) {
        free_sym(symbol);
        return NULL;
    }
    return symbol;
}

// Shows a single symbol table and its children recursively
void show_symbol_table_recursive(sym_t* sym_table, int indent_level) 
{
    if (!sym_table) return;

    // Print indentation
    for (int i = 0; i < indent_level; ++i) 
    {
        printf("  "); // 2 spaces per indent level
    }

    printf("--- Scope (Depth: %d, Addr: %p, Parent: %p, Children: %d) ---\n",
           sym_table->depth, (void*)sym_table, (void*)sym_table->parent, sym_table->num_children);
    
    if (sym_table->table) 
    {
        ht_show(sym_table->table, indent_level + 1); 
    } 
    else 
    {
        for (int i = 0; i < indent_level + 1; ++i) printf("  ");
        printf("(Hash table not initialized)\n");
    }

    // Recursively show children
    for (int i = 0; i < sym_table->num_children; ++i) 
    {
        show_symbol_table_recursive(sym_table->children[i], indent_level + 1);
    }
}

// Wrapper to start showing the symbol table tree from a given root
void show_entire_symbol_tree(sym_t* root_sym_table) 
{
    printf("\n===== Entire Symbol Table Tree Dump =====\n");
    show_symbol_table_recursive(root_sym_table, 0);
    printf("=======================================\n\n");
}

// This function shows the current scope and its parents up to the global scope.
void show_current_and_parent_scopes(sym_t* current_scope) 
{
    printf("\n===== Symbol Table Dump (Current & Parents) =====\n");
    sym_t* temp = current_scope;
    int indent = 0; // Basic indentation for this linear view
    while(temp != NULL) 
    {
        for(int i=0; i < indent; ++i) printf("  ");
        printf("--- Scope (Depth: %d, Addr: %p, Parent: %p, Children: %d) ---\n",
           temp->depth, (void*)temp, (void*)temp->parent, temp->num_children);
        if (temp->table) 
        {
            ht_show(temp->table, indent + 1); // Pass indent level to ht_show
        } 
        else 
        {
            for(int i=0; i < indent + 1; ++i) printf("  ");
            printf("    (Hash table not initialized)\n");
        }
        temp = temp->parent;
        indent++; // Increase indent for parent, though this view is linear
    }
    printf("==============================================\n\n");
}

// Frees the given symbol table and all its descendant children tables.
void free_all_symbol_tables(sym_t* sym_table) 
{
    if (sym_table == NULL) return;

    // 1. Recursively free all children
    for (int i = 0; i < sym_table->num_children; i++) 
    {
        free_all_symbol_tables(sym_table->children[i]);
    }

    // 2. Free the array that held the children pointers
    if (sym_table->children) 
    {
        free(sym_table->children);
        sym_table->children = NULL; // Set to NULL after freeing
    }
    // Reset children count and capacity (optional, as struct is about to be freed)
    sym_table->num_children = 0;
    sym_table->children_capacity = 0;

    // 3. Free the hash table of the current scope
    if (sym_table->table) 
    {
        free_ht(sym_table->table); // free_ht is responsible for freeing ht_entry_t and Symbols
        sym_table->table = NULL;   // Set to NULL after freeing
    }

    // 4. Free the symbol table struct itself
    free(sym_table);
}

#endif // SYMBOL_TABLE_H
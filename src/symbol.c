#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "symbol.h"

Symbol* create_sym(const char* name, int tag) 
{
    Symbol* s = (Symbol*)malloc(sizeof(Symbol));
    if (!s) 
    {
        perror("Failed to allocate memory for Symbol");
        return NULL;
    }

    s->name = strdup(name);
    if (!s->name) 
    {
        perror("Failed to duplicate name for Symbol");
        free(s);
        return NULL;
    }

    s->tag = tag;
    s->num_params = 0;
    s->type_list = NULL;

    return s;
}

Symbol* copy_sym(Symbol *s) 
{
    if (!s) return NULL;

    Symbol *copy = (Symbol *)malloc(sizeof(Symbol));
    if (!copy) 
    {
        perror("Failed to allocate memory for Symbol copy");
        return NULL;
    }

    copy->name = strdup(s->name);
    if (!copy->name) 
    {
        perror("Failed to duplicate name for Symbol copy");
        free(copy);
        return NULL;
    }

    copy->tag = s->tag;

    // TODO : and other fields if needed

    return copy;
}

void free_sym(Symbol *s) 
{
    if (!s) return;
    free(s->name);
    free(s->type_list);

    // TODO : and other fields if needed

    free(s); // Finally, free the Symbol struct itself
}
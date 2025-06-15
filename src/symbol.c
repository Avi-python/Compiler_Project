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

    s->name = strdup(name); // Duplicate the string
    if (!s->name) 
    {
        perror("Failed to duplicate name for Symbol");
        free(s);
        return NULL;
    }

    s->tag = tag;

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

    // TODO : and other fields if needed

    free(s); // Finally, free the Symbol struct itself
}
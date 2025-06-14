#ifndef SYMBOL_H
#define SYMBOL_H

typedef struct {
    char* name;
    int tag;
    // TODO : offset ?
} Symbol;

Symbol* create_sym(const char* name, int tag);
Symbol* copy_sym(Symbol *s);
void free_sym(Symbol *s);

#endif // SYMBOL_H
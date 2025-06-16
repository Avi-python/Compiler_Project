#ifndef SYMBOL_H
#define SYMBOL_H

typedef struct {
    char* name;
    int tag;
    int num_params;
    int* type_list;
    // TODO : offset ?
} Symbol;

Symbol* create_sym(const char* name, int tag);
Symbol* copy_sym(Symbol *s);
void free_sym(Symbol *s);

#endif // SYMBOL_H
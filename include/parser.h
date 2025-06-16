#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "ast.h"
#include "symbol_table.h"

// Struct to hold the results of parsing
typedef struct {
    int error_count;
    ASTNode* ast_root;
    sym_t* global_sym_table;
} ParseResult;

// Function to parse the source code
// It handles opening/closing the source file and log file.
// Returns the parsing results.
// If a fatal setup error occurs (e.g., cannot open files), error_count in ParseResult will be -1.
ParseResult parse_source_file(const char* source_filename);

#endif // PARSER_H

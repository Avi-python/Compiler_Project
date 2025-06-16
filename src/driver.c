#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "ast.h"
#include "symbol_table.h"

// External declaration from scanner.l, if you need to call it directly (e.g. for errors)
extern void show_and_free_errors(); 

int main(int argc, char **argv) 
{
    if (argc < 2) 
    {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* source_filename = argv[1];
    ParseResult result = parse_source_file(source_filename);

    if (result.error_count == -1) 
    {
        fprintf(stderr, "A fatal error occurred during parser setup. Exiting.\n");
        return EXIT_FAILURE;
    }

    if (result.error_count > 0) {
        printf("Displaying errors recorded by the lexer/parser:\n");
        // This function is declared in scanner.l (via error.h perhaps) 
        // and prints errors stored in its global error array.
        // Ensure it's correctly linked and available.
        show_and_free_errors(); 
    }

    printf("--- Driver: Parser Execution Finished ---\n");
    printf("Number of errors: %d\n", result.error_count);

    if (result.ast_root) 
    {
        printf("AST Root Node Type: %d\n", result.ast_root->type); 
        visualize_ast(result.ast_root, "ast_output.dot");
    } else 
    {
        printf("AST Root is NULL.\n");
    }

    if (result.global_sym_table) 
    {
        // show symbol table in symbol_table_logs.txt

        FILE* symbol_table_logs_file = fopen("symbol_table_logs.txt", "w");
        if (!symbol_table_logs_file ) {
            perror("Failed to open log file");
            result.error_count = -1; // Indicate fatal error
        }
        else
        {
            fprintf(symbol_table_logs_file, "Global Symbol Table Root: %p\n", (void*)result.global_sym_table);
            show_entire_symbol_tree(result.global_sym_table, symbol_table_logs_file);
        }
    }
    else {
        printf("Global Symbol Table is NULL.\n");
    }

    // Semantic Analysis Stage (Placeholder)
    if (result.error_count == 0 && result.ast_root != NULL) 
    {
        // semantic_analysis(result.ast_root, result.global_sym_table);
        // int semantic_errors = perform_semantic_analysis(result.ast_root, result.global_sym_table);
        // if (semantic_errors > 0) {
        //     printf("Semantic analysis found %d errors.\n", semantic_errors);
        // } else {
        //     printf("Semantic analysis successful.\n");
        //     // Proceed to code generation or other phases
        // }
    } else if (result.error_count > 0) 
    {
        printf("Skipping semantic analysis due to parsing errors.\n");
    } else 
    {
        printf("Skipping semantic analysis as AST root is NULL (e.g. empty input).\n");
    }

    if (result.ast_root) 
    {
        printf("Freeing AST...\n");
        free_ast(result.ast_root);
        result.ast_root = NULL;
    }
    if (result.global_sym_table) 
    {
        printf("Freeing symbol tables...\n");
        free_all_symbol_tables(result.global_sym_table);
        result.global_sym_table = NULL;
    }

    printf("Driver finished.\n");
    return (result.error_count > 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}
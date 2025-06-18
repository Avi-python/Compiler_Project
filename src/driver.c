#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "parser.h"
#include "ast.h"
#include "symbol_table.h"
#include "tokens.h"
#include "codegen.h"
#include "semantic.h"

// External declaration from scanner.l, if you need to call it directly (e.g. for errors)
extern char* yyfilename;
extern void show_and_free_errors(); 
void save_error_details(const char* type, const char* msg, int line, int col, const char* filename_to_use);

// Compilation options
typedef struct {
    char* input_file;
    char* output_file;
    int generate_ir_only;     // -S flag: generate LLVM IR only
    int verbose;              // -v flag: verbose output
    int help;                 // -h flag: show help
} CompilerOptions;

void print_help(const char* program_name) 
{
    printf("Usage: %s [options] <source_file>\n", program_name);
    printf("Options:\n");
    printf("  -o <file>    Specify output file name\n");
    printf("  -S           Generate LLVM IR only (.ll file)\n");
    printf("  -v           Verbose output\n");
    printf("  -h           Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s program.c                    # Compile to executable 'program'\n", program_name);
    printf("  %s -S program.c                 # Generate program.ll (LLVM IR)\n", program_name);
    printf("  %s -c program.c                 # Generate program.o (object file)\n", program_name);
    printf("  %s -o myprogram program.c       # Compile to executable 'myprogram'\n", program_name);
}

CompilerOptions parse_arguments(int argc, char** argv)
{
    CompilerOptions opts = {0};
    int opt;
    
    while ((opt = getopt(argc, argv, "o:Scvh")) != -1) 
    {
        switch (opt) {
            case 'o':
                opts.output_file = strdup(optarg);
                break;
            case 'S':
                opts.generate_ir_only = 1;
                break;
            case 'v':
                opts.verbose = 1;
                break;
            case 'h':
                opts.help = 1;
                break;
            default:
                fprintf(stderr, "Unknown option: %c\n", opt);
                opts.help = 1;
                break;
        }
    }
    
    if (optind < argc) opts.input_file = strdup(argv[optind]);
    
    return opts;
}

char* get_output_filename(const CompilerOptions* opts) 
{
    if (opts->output_file) return strdup(opts->output_file);

    if (!opts->input_file) return strdup("a.out");
    
    char* base_name = strdup(opts->input_file);
    char* dot = strrchr(base_name, '.');
    if (dot) *dot = '\0';  // Remove extension
    
    char* output_name = malloc(strlen(base_name) + 10);
    if (opts->generate_ir_only) 
    {
        sprintf(output_name, "%s.ll", base_name);
    } 
    else 
    {
        strcpy(output_name, base_name);
    }
    
    free(base_name);
    return output_name;
}

int execute_command(const char* command, int verbose) 
{
    if (verbose) printf("Executing: %s\n", command);
    
    int result = system(command);
    if (result != 0) 
    {
        fprintf(stderr, "Command failed with exit code: %d\n", result);
        fprintf(stderr, "Command was: %s\n", command);
    }
    return result;
}

int main(int argc, char **argv) 
{
    CompilerOptions opts = parse_arguments(argc, argv);
    
    if (opts.help || !opts.input_file) 
    {
        print_help(argv[0]);
        return opts.help ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    if (opts.verbose) printf("Compiling: %s\n", opts.input_file);

    // Parse the source file
    ParseResult result = parse_source_file(opts.input_file);

    if (result.error_count == -1) 
    {
        fprintf(stderr, "A fatal error occurred during parser setup. Exiting.\n");
        return EXIT_FAILURE;
    }

    // Perform semantic analysis
    int semantic_errors = 0;
    if (result.ast_root != NULL) 
    {
        semantic_errors = semantic_analysis(result.ast_root, result.global_sym_table);
    } 
    else 
    {
        printf("Skipping semantic analysis as AST root is NULL (e.g. empty input).\n");
    }

    if (result.error_count + semantic_errors > 0) 
    {
        printf("Displaying errors recorded by the lexer/parser/semantic analysis:\n");
        show_and_free_errors();
        
        // Don't proceed with code generation if there are errors
        if (result.ast_root) free_ast(result.ast_root);
        if (result.global_sym_table) free_all_symbol_tables(result.global_sym_table);
        return EXIT_FAILURE;
    }

    // Generate output filename
    char* output_filename = get_output_filename(&opts);
    
    // Initialize code generation
    char ir_filename[256];
    if (opts.generate_ir_only) 
    {
        strcpy(ir_filename, output_filename);
    }
    else
    {
        // Create temporary IR file for intermediate compilation
        sprintf(ir_filename, "%s.tmp.ll", output_filename);
    }
    
    codegen_init(ir_filename);
    
    if (opts.verbose) printf("Generating LLVM IR...\n");
    
    // Generate LLVM IR from AST
    int codegen_result = codegen_generate(result.ast_root, result.global_sym_table);
    
    if (codegen_result != 0) 
    {
        fprintf(stderr, "Code generation failed.\n");
        codegen_dispose();
        free(output_filename);
        if (result.ast_root) free_ast(result.ast_root);
        if (result.global_sym_table) free_all_symbol_tables(result.global_sym_table);
        return EXIT_FAILURE;
    }
    
    // Print LLVM IR to file
    codegen_print_ir();
    
    if (opts.verbose) printf("LLVM IR generated successfully: %s\n", ir_filename);
    
    // Handle different compilation modes
    int final_result = EXIT_SUCCESS;
    
    if (opts.generate_ir_only) 
    {
        // Just generate IR - we're done
        printf("LLVM IR generated: %s\n", output_filename);
    }
    else
    {
        // Compile IR to object file
        codegen_to_object();
        codegen_to_assembly();
    }

    codegen_dispose();
    
    // Generate additional output files if verbose
    if (opts.verbose) 
    {
        if (result.ast_root) 
        {
            visualize_ast(result.ast_root, "ast_output.dot");
            printf("AST visualization saved to: ast_output.dot\n");
        }
        
        if (result.global_sym_table) 
        {
            FILE* symbol_table_logs_file = fopen("symbol_table_logs.txt", "w");
            if (symbol_table_logs_file) 
            {
                fprintf(symbol_table_logs_file, "Global Symbol Table Root: %p\n", (void*)result.global_sym_table);
                show_entire_symbol_tree(result.global_sym_table, symbol_table_logs_file);
                fclose(symbol_table_logs_file);
                printf("Symbol table saved to: symbol_table_logs.txt\n");
            }
        }
    }
    
    // Clean up
    free(output_filename);
    if (opts.output_file) free(opts.output_file);
    if (opts.input_file) free(opts.input_file);
    
    if (result.ast_root) 
    {
        free_ast(result.ast_root);
    }
    if (result.global_sym_table) 
    {
        free_all_symbol_tables(result.global_sym_table);
    }

    if (opts.verbose) 
    {
        printf("Compilation completed %s.\n", 
               (final_result == EXIT_SUCCESS) ? "successfully" : "with errors");
    }
    
    return final_result;
}
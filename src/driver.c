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

// External declaration from scanner.l, if you need to call it directly (e.g. for errors)
extern char* yyfilename;
extern void show_and_free_errors(); 
void save_error_details(const char* type, const char* msg, int line, int col, const char* filename_to_use);

// Global variable to track semantic errors
static int semantic_error_count = 0;

// Global variable to track current function's return type
static int current_function_return_type = VOID; // VOID

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

// Function declarations for semantic analysis
int semantic_analysis(ASTNode* ast_root, sym_t* global_sym_table);
void analyze_node(ASTNode* node, sym_t* symbol_table);
void analyze_program_node(ProgramNode* node, sym_t* symbol_table);
void analyze_function_definition(FunctionDefinitionNode* node, sym_t* symbol_table);
void analyze_variable_declaration(ASTNode* node, sym_t* symbol_table);
void analyze_assignment(AssignmentStatementNode* node, sym_t* symbol_table);
void analyze_function_call(FunctionCallNode* node, sym_t* symbol_table);
void analyze_return_statement(ReturnStatementNode* node, sym_t* symbol_table);
void analyze_binary_expression(BinaryExpressionNode* node, sym_t* symbol_table);
void analyze_if_statement(IfStatementNode* node, sym_t* symbol_table);
void analyze_while_statement(WhileStatementNode* node, sym_t* symbol_table);
void analyze_compound_statement(CompoundStatementNode* node, sym_t* symbol_table);

// Helper function declarations
int get_node_type(ASTNode* node, sym_t* symbol_table);
int count_parameters(ASTNode* param_list);
int count_arguments(ASTNode* arg_list);
int* get_parameter_types(ASTNode* param_list, int count);
int* get_argument_types(ASTNode* arg_list, int count);
int types_compatible(int type1, int type2);
const char* type_to_string(int type);
int has_return_statement(ASTNode* node);

// Main semantic analysis function
int semantic_analysis(ASTNode* ast_root, sym_t* global_sym_table) 
{
    if (!ast_root || !global_sym_table) return 0;
    
    semantic_error_count = 0;
    
    analyze_node(ast_root, global_sym_table);
    
    return semantic_error_count;
}

// Recursive function to analyze each AST node
void analyze_node(ASTNode* node, sym_t* symbol_table) 
{
    if (!node) return;
    
    switch (node->type) 
    {
        case NODE_PROGRAM:
            analyze_program_node((ProgramNode*)node, symbol_table);
            break;
        case NODE_FUNCTION_DEFINITION:
            analyze_function_definition((FunctionDefinitionNode*)node, symbol_table);
            break;
        case NODE_GLOBAL_VARIABLE_DECLARATION:
        case NODE_LOCAL_VARIABLE_DECLARATION:
            analyze_variable_declaration(node, symbol_table);
            break;
        case NODE_ASSIGNMENT_STATEMENT:
            analyze_assignment((AssignmentStatementNode*)node, symbol_table);
            break;
        case NODE_FUNCTION_CALL:
            analyze_function_call((FunctionCallNode*)node, symbol_table);
            break;
        case NODE_RETURN_STATEMENT:
            analyze_return_statement((ReturnStatementNode*)node, symbol_table);
            break;
        case NODE_BINARY_EXPRESSION:
            analyze_binary_expression((BinaryExpressionNode*)node, symbol_table);
            break;
        case NODE_IF_STATEMENT:
            analyze_if_statement((IfStatementNode*)node, symbol_table);
            break;
        case NODE_WHILE_STATEMENT:
            analyze_while_statement((WhileStatementNode*)node, symbol_table);
            break;
        case NODE_COMPOUND_STATEMENT:
            analyze_compound_statement((CompoundStatementNode*)node, symbol_table);
            break;
        default:
            break;
    }
    
    if (node->next) analyze_node(node->next, symbol_table);
}

void analyze_program_node(ProgramNode* node, sym_t* symbol_table) 
{
    if (node->start) analyze_node(node->start, symbol_table);
}

void analyze_function_definition(FunctionDefinitionNode* node, sym_t* symbol_table) 
{
    if (!node->type || !node->name_identifier) return;
    
    int return_type = get_node_type(node->type, symbol_table);
    
    IdentifierNode* func_name = (IdentifierNode*)node->name_identifier;
    if (!func_name || !func_name->symbol) return;
    
    // Count parameters and get their types
    int param_count = count_parameters(node->params);
    int* param_types = get_parameter_types(node->params, param_count);
    
    // Update symbol with parameter information
    func_name->symbol->num_params = param_count;
    func_name->symbol->type_list = param_types;
    
    // Store current function context for return type checking
    current_function_return_type = return_type;
    
    // Analyze function parameters
    if (node->params) analyze_node(node->params, symbol_table);
    
    // Analyze function body
    if (node->body) analyze_node(node->body, symbol_table);
    
    // Check if non-void function has return statement
    if (return_type != VOID) 
    {
        if (!function_has_complete_return_coverage(node->body))
        {
            char error_msg[256];
            sprintf(error_msg, "Function '%s' with non-void return type all paths must have a return statement", 
                    func_name->symbol->name);
            save_error_details("semantic error", error_msg, node->base.lineno, node->base.colno, yyfilename);
            semantic_error_count++;
        }
    }
}

void analyze_variable_declaration(ASTNode* node, sym_t* symbol_table) 
{
    ASTNode* type_node = NULL;
    ASTNode* declarators = NULL;
    
    if (node->type == NODE_GLOBAL_VARIABLE_DECLARATION) 
    {
        GlobalVariableDeclarationNode* gvd = (GlobalVariableDeclarationNode*)node;
        type_node = gvd->type;
        declarators = gvd->start;
    } 
    else if (node->type == NODE_LOCAL_VARIABLE_DECLARATION) 
    {
        LocalVariableDeclarationNode* lvd = (LocalVariableDeclarationNode*)node;
        type_node = lvd->type;
        declarators = lvd->start;
    }
    
    if (!type_node) return;
    
    int var_type = get_node_type(type_node, symbol_table);
    
    // Check each declarator
    ASTNode* current = declarators;
    while (current) 
    {
        if (current->type == NODE_VARIABLE_DECLARATOR) 
        {
            VariableDeclaratorNode* vd = (VariableDeclaratorNode*)current;
            if (vd->expression) 
            {
                int expr_type = get_node_type(vd->expression, symbol_table);
                if (!types_compatible(var_type, expr_type)) 
                {
                    char error_msg[256];
                    sprintf(error_msg, "Type mismatch in variable initialization: expected %s, got %s",
                            type_to_string(var_type), type_to_string(expr_type));
                    save_error_details("semantic error", error_msg, vd->base.lineno, vd->base.colno, yyfilename);
                    semantic_error_count++;
                }
            }
        }
        current = current->next;
    }
}

void analyze_assignment(AssignmentStatementNode* node, sym_t* symbol_table) 
{
    if (!node->identifier || !node->expression) return;
    
    int lhs_type = get_node_type(node->identifier, symbol_table);
    int rhs_type = get_node_type(node->expression, symbol_table);
    
    if (!types_compatible(lhs_type, rhs_type)) 
    {
        char error_msg[256];
        sprintf(error_msg, "Type mismatch in assignment: cannot assign %s to %s",
                type_to_string(rhs_type), type_to_string(lhs_type));
        save_error_details("semantic error", error_msg, node->base.lineno, node->base.colno, yyfilename);
        semantic_error_count++;
    }
    
    // Analyze the expression
    analyze_node(node->expression, symbol_table);
}

void analyze_function_call(FunctionCallNode* node, sym_t* symbol_table) 
{
    if (!node->identifier) return;

    IdentifierNode* func_name = (IdentifierNode*)node->identifier;
    if (!func_name || !func_name->symbol) return;
    
    Symbol* func_symbol = func_name->symbol;
    
    // Count actual arguments
    int arg_count = count_arguments(node->params);
    
    // Check parameter count
    if (func_symbol->num_params != arg_count) 
    {
        char error_msg[256];
        sprintf(error_msg, "Function '%s' expects %d parameters, but %d were provided",
                func_symbol->name, func_symbol->num_params, arg_count);
        save_error_details("semantic error", error_msg, node->base.lineno, node->base.colno, yyfilename);
        semantic_error_count++;
    } 
    else 
    {
        // Check parameter types if counts match
        int* arg_types = get_argument_types(node->params, arg_count);
        if (arg_types && func_symbol->type_list) 
        {
            for (int i = 0; i < arg_count; i++) 
            {
                if (!types_compatible(func_symbol->type_list[i], arg_types[i])) 
                {
                    char error_msg[256];
                    sprintf(error_msg, "Parameter %d type mismatch in call to '%s': expected %s, got %s",
                            i + 1, func_symbol->name,
                            type_to_string(func_symbol->type_list[i]),
                            type_to_string(arg_types[i]));
                    save_error_details("semantic error", error_msg, node->base.lineno, node->base.colno, yyfilename);
                    semantic_error_count++;
                }
            }
        }
        if (arg_types) free(arg_types);
    }
    
    if (node->params) analyze_node(node->params, symbol_table);
}

void analyze_return_statement(ReturnStatementNode* node, sym_t* symbol_table) 
{
    if (node->expression) 
    {
        int return_expr_type = get_node_type(node->expression, symbol_table);
        
        if (!types_compatible(current_function_return_type, return_expr_type)) 
        {
            char error_msg[256];
            sprintf(error_msg, "Return type mismatch: expected %s, got %s",
                    type_to_string(current_function_return_type),
                    type_to_string(return_expr_type));
            save_error_details("semantic error", error_msg, node->base.lineno, node->base.colno, yyfilename);
            semantic_error_count++;
        }
        
        analyze_node(node->expression, symbol_table);
    }
    else
    {
        if (current_function_return_type != VOID) 
        {
            char error_msg[256];
            sprintf(error_msg, "Non-void function must return a value");
            save_error_details("semantic error", error_msg, node->base.lineno, node->base.colno, yyfilename);
            semantic_error_count++;
        }
    }
}

void analyze_binary_expression(BinaryExpressionNode* node, sym_t* symbol_table) 
{
    if (!node->left || !node->right) return;
    
    int left_type = get_node_type(node->left, symbol_table);
    int right_type = get_node_type(node->right, symbol_table);
    
    if (node->op == PLUS || node->op == MINUS || node->op == MUL || node->op == DIV) 
    {
        if ((left_type != INT && left_type != CHAR) || 
            (right_type != INT && right_type != CHAR)) 
        {
            char error_msg[256];
            sprintf(error_msg, "Arithmetic operation requires numeric operands");
            save_error_details("semantic error", error_msg, node->base.lineno, node->base.colno, yyfilename);
            semantic_error_count++;
        }
    }
    
    analyze_node(node->left, symbol_table);
    analyze_node(node->right, symbol_table);
}

void analyze_if_statement(IfStatementNode* node, sym_t* symbol_table) {
    if (node->condition) {
        analyze_node(node->condition, symbol_table);
    }
    if (node->body) {
        analyze_node(node->body, symbol_table);
    }
    if (node->else_body) {
        analyze_node(node->else_body, symbol_table);
    }
}

void analyze_while_statement(WhileStatementNode* node, sym_t* symbol_table) {
    if (node->condition) {
        analyze_node(node->condition, symbol_table);
    }
    if (node->body) {
        analyze_node(node->body, symbol_table);
    }
}

void analyze_compound_statement(CompoundStatementNode* node, sym_t* symbol_table) {
    if (node->start) {
        analyze_node(node->start, symbol_table);
    }
}

// Helper function implementations
int get_node_type(ASTNode* node, sym_t* symbol_table) 
{
    if (!node) return -1;
    
    switch (node->type) 
    {
        case NODE_TYPE: 
        {
            TypeNode* type_node = (TypeNode*)node;
            return type_node->type;
        }
        case NODE_IDENTIFIER: 
        {
            IdentifierNode* id_node = (IdentifierNode*)node;
            if (id_node->symbol) return id_node->symbol->tag;
            return -1;
        }
        case NODE_NUMBER_LITERAL:
            return INT;
        case NODE_BINARY_EXPRESSION: 
        {
            BinaryExpressionNode* bin_node = (BinaryExpressionNode*)node;
            // For arithmetic operations, result is int
            if (bin_node->op == PLUS || bin_node->op == MINUS || 
                bin_node->op == MUL || bin_node->op == DIV) { 
                return INT; // INT
            }
            // For comparison operations, result is int (0 or 1)
            return INT; // INT
        }
        case NODE_FUNCTION_CALL: 
        {
            FunctionCallNode* fc_node = (FunctionCallNode*)node;
            IdentifierNode* func_name = (IdentifierNode*)fc_node->identifier;
            if (func_name && func_name->symbol) return func_name->symbol->tag;
            return -1;
        }
        default:
            return -1;
    }
}

int count_parameters(ASTNode* param_list) 
{
    int count = 0;
    ASTNode* current = param_list;
    
    while (current) 
    {
        if (current->type == NODE_PARAMETER) count++;
        current = current->next;
    }
    
    return count;
}

int count_arguments(ASTNode* arg_list) 
{
    int count = 0;
    ASTNode* current = arg_list;
    
    while (current) 
    {
        count++;
        current = current->next;
    }
    
    return count;
}

int* get_parameter_types(ASTNode* param_list, int count) 
{
    if (count == 0) return NULL;
    
    int* types = (int*)malloc(sizeof(int) * count);
    if (!types) return NULL;
    
    ASTNode* current = param_list;
    int index = 0;
    
    while (current && index < count) 
    {
        if (current->type == NODE_PARAMETER) 
        {
            ParameterNode* param = (ParameterNode*)current;
            types[index] = get_node_type(param->type, NULL);
            index++;
        }
        current = current->next;
    }
    
    return types;
}

int* get_argument_types(ASTNode* arg_list, int count) 
{
    if (count == 0) return NULL;
    
    int* types = (int*)malloc(sizeof(int) * count);
    if (!types) return NULL;
    
    ASTNode* current = arg_list;
    int index = 0;
    
    while (current && index < count) 
    {
        types[index] = get_node_type(current, NULL);
        index++;
        current = current->next;
    }
    
    return types;
}

int types_compatible(int type1, int type2) 
{
    // Exact match
    if (type1 == type2) return 1;
    
    // int and char are compatible for assignments and operations
    if ((type1 == INT && type2 == CHAR) || (type1 == CHAR && type2 == INT)) return 1;
    
    return 0;
}

const char* type_to_string(int type) 
{
    switch (type) {
        case INT: return "int";
        case CHAR: return "char";
        case VOID: return "void";
        default: return "unknown";
    }
}

// Updated function to check if ALL execution paths have return statements
// This is crucial for non-void functions to ensure they always return a value
int has_return_statement(ASTNode* node) 
{
    if (!node) return 0;
    
    if (node->type == NODE_RETURN_STATEMENT) return 1;
    
    switch (node->type) 
    {
        case NODE_COMPOUND_STATEMENT: 
        {
            CompoundStatementNode* cs = (CompoundStatementNode*)node;
            ASTNode* current = cs->start;
            while (current) 
            {
                if (has_return_statement(current)) return 1;
                current = current->next;
            }
            return 0;
        }
        case NODE_IF_STATEMENT: 
        {
            IfStatementNode* if_node = (IfStatementNode*)node;
            int then_has_return = has_return_statement(if_node->body);
            
            if (if_node->else_body) 
            {
                int else_has_return = has_return_statement(if_node->else_body);
                return then_has_return && else_has_return;
            } else 
            {
                return 0;
            }
        }
        case NODE_WHILE_STATEMENT: 
        {
            // While loops cannot guarantee that their body will execute
            // Therefore, they cannot guarantee a return statement
            // Even if the body has a return, the loop might not execute at all
            return 0;
        }
        case NODE_PROGRAM:
        case NODE_FUNCTION_DEFINITION:
        case NODE_GLOBAL_VARIABLE_DECLARATION:
        case NODE_LOCAL_VARIABLE_DECLARATION:
        case NODE_VARIABLE_DECLARATOR:
        case NODE_PARAMETER:
        case NODE_ASSIGNMENT_STATEMENT:
        case NODE_BINARY_EXPRESSION:
        case NODE_FUNCTION_CALL:
        case NODE_IDENTIFIER:
        case NODE_NUMBER_LITERAL:
        case NODE_TYPE:
        case NODE_ERROR:
        default:
            // These node types don't contain return statements
            return 0;
    }
}

int function_has_complete_return_coverage(ASTNode* function_body) 
{
    if (!function_body) return 0;
    
    return has_return_statement(function_body);
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
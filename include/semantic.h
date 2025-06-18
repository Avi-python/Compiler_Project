#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symbol_table.h"

// Main semantic analysis function
int semantic_analysis(ASTNode* ast_root, sym_t* global_sym_table);

// Node analysis functions
void analyze_node(ASTNode* node, sym_t* symbol_table);
void analyze_program_node(ProgramNode* node, sym_t* symbol_table);
void analyze_function_definition(FunctionDefinitionNode* node, sym_t* symbol_table);
void analyze_variable_declaration(ASTNode* node, sym_t* symbol_table);
void analyze_assignment(AssignmentStatementNode* node, sym_t* symbol_table);
void analyze_function_call(FunctionCallNode* node, sym_t* symbol_table);
void analyze_return_statement(ReturnStatementNode* node, sym_t* symbol_table);
void analyze_print_statement(PrintStatementNode* node, sym_t* symbol_table);
void analyze_binary_expression(BinaryExpressionNode* node, sym_t* symbol_table);
void analyze_if_statement(IfStatementNode* node, sym_t* symbol_table);
void analyze_while_statement(WhileStatementNode* node, sym_t* symbol_table);
void analyze_compound_statement(CompoundStatementNode* node, sym_t* symbol_table);

// Helper functions
int get_node_type(ASTNode* node, sym_t* symbol_table);
int count_parameters(ASTNode* param_list);
int count_arguments(ASTNode* arg_list);
int* get_parameter_types(ASTNode* param_list, int count);
int* get_argument_types(ASTNode* arg_list, int count);
int types_compatible(int type1, int type2);
const char* type_to_string(int type);
int has_return_statement(ASTNode* node);
int function_has_complete_return_coverage(ASTNode* function_body);

// Global variable to track current function's return type
extern int current_function_return_type;

#endif // SEMANTIC_H
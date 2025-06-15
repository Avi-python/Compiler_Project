#ifndef AST_H
#define AST_H

#include "symbol.h"

typedef enum {
    NODE_PROGRAM,
    NODE_FUNCTION_DEFINITION,
    NODE_GLOBAL_VARIABLE_DECLARATION,
    NODE_LOCAL_VARIABLE_DECLARATION,
    NODE_VARIABLE_DECLARATOR,
    NODE_PARAMETER,
    NODE_COMPOUND_STATEMENT,
    NODE_IF_STATEMENT,
    NODE_WHILE_STATEMENT,
    NODE_RETURN_STATEMENT,
    NODE_ASSIGNMENT_STATEMENT,
    NODE_BINARY_EXPRESSION,
    NODE_FUNCTION_CALL,
    NODE_IDENTIFIER,
    NODE_NUMBER_LITERAL,
    NODE_TYPE,
    NODE_ERROR
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    struct ASTNode* next;
    struct ASTNode* root; 
} ASTNode;

typedef struct {
    ASTNode base;
    Symbol* symbol;
} IdentifierNode;
IdentifierNode* create_identifier_node(Symbol* symbol);

typedef struct {
    ASTNode base;
    ASTNode* start;
} ProgramNode;
ProgramNode* create_program_node(ASTNode* start);

typedef struct {
    ASTNode base;
    ASTNode* type;
    ASTNode* name_identifier;
    ASTNode* params; // list of ParameterNode
    ASTNode* body;
} FunctionDefinitionNode;
FunctionDefinitionNode* create_function_definition_node(ASTNode* type, ASTNode* name_identifier, ASTNode* params, ASTNode* body);

typedef struct {
    ASTNode base;
    ASTNode* type;
    ASTNode* start; // list of VariableDeclaratorNode
} GlobalVariableDeclarationNode;
GlobalVariableDeclarationNode* create_global_variable_declaration_node(ASTNode* type, ASTNode* start);

typedef struct {
    ASTNode base;
    ASTNode* identifier;
    ASTNode* expression;
} VariableDeclaratorNode;
VariableDeclaratorNode* create_variable_declarator_node(ASTNode* identifier, ASTNode* expression);

typedef struct {
    ASTNode base;
    ASTNode* expression;
} ReturnStatementNode;
ReturnStatementNode* create_return_statement_node(ASTNode* expression);

typedef struct {
    ASTNode base;
    ASTNode* type;
    ASTNode* start; // list of VariableDeclaratorNode
} LocalVariableDeclarationNode;
LocalVariableDeclarationNode* create_local_variable_declaration_node(ASTNode* type, ASTNode* start);

typedef struct {
    ASTNode base;
    ASTNode* type;
    ASTNode* name_identifier;
} ParameterNode; 
ParameterNode* create_parameter_node(ASTNode* type, ASTNode* name_identifier);

typedef struct {
    ASTNode base;
    ASTNode* start; // list of StatementNode
} CompoundStatementNode;
CompoundStatementNode* create_compound_statement_node(ASTNode* start);

typedef struct {
    ASTNode base;
    ASTNode* condition; // expression
    ASTNode* body;
    ASTNode* else_body;
} IfStatementNode;
IfStatementNode* create_if_statement_node(ASTNode* condition, ASTNode* body, ASTNode* else_body);

typedef struct {
    ASTNode base;
    ASTNode* condition;
    ASTNode* body;
} WhileStatementNode;
WhileStatementNode* create_while_statement_node(ASTNode* condition, ASTNode* body);

typedef struct {
    ASTNode base;
    ASTNode* identifier;
    ASTNode* expression;
} AssignmentStatementNode;
AssignmentStatementNode* create_assignment_statement_node(ASTNode* identifier, ASTNode* expression);

typedef struct {
    ASTNode base;
    ASTNode* identifier;
    ASTNode* params;
} FunctionCallNode;
FunctionCallNode* create_function_call_node(ASTNode* identifier, ASTNode* params);

typedef struct {
    ASTNode base;
    int op;
    ASTNode* left; 
    ASTNode* right;
} BinaryExpressionNode;
BinaryExpressionNode* create_binary_expression_node(int op, ASTNode* left, ASTNode* right);

typedef struct {
    ASTNode base;
    int value;
} NumberNode;
NumberNode* create_number_literal_node(int value);

typedef struct {
    ASTNode base;
    int type;
} TypeNode;
TypeNode* create_type_node(int type_specifier);

typedef struct {
    ASTNode base;
} ErrorNode;
ErrorNode* create_error_node();

// Function to free an AST tree
void free_ast(ASTNode* node);

#endif // AST_H
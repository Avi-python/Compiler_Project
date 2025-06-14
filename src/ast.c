#include "ast.h"
#include <stdlib.h>

ProgramNode* create_program_node(ASTNode* start) {
    ProgramNode* node = (ProgramNode*)malloc(sizeof(ProgramNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_PROGRAM;
    node->base.next = NULL;
    node->base.root = (ASTNode*)node; // Program node is its own root
    node->start = start;
    return node;
}

FunctionDefinitionNode* create_function_definition_node(ASTNode* type, ASTNode* name_identifier, ASTNode* params, ASTNode* body) {
    FunctionDefinitionNode* node = (FunctionDefinitionNode*)malloc(sizeof(FunctionDefinitionNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_FUNCTION_DEFINITION;
    node->base.next = NULL;
    node->base.root = NULL;
    node->type = type;
    node->name_identifier = name_identifier;
    node->params = params;
    node->body = body;
    return node;
}

GlobalVariableDeclarationNode* create_global_variable_declaration_node(ASTNode* type, ASTNode* start) {
    GlobalVariableDeclarationNode* node = (GlobalVariableDeclarationNode*)malloc(sizeof(GlobalVariableDeclarationNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_GLOBAL_VARIABLE_DECLARATION;
    node->base.next = NULL;
    node->base.root = NULL;
    node->type = type;
    node->start = start;
    return node;
}

LocalVariableDeclarationNode* create_local_variable_declaration_node(ASTNode* type, ASTNode* start) {
    LocalVariableDeclarationNode* node = (LocalVariableDeclarationNode*)malloc(sizeof(LocalVariableDeclarationNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_LOCAL_VARIABLE_DECLARATION;
    node->base.next = NULL;
    node->base.root = NULL;
    node->type = type;
    node->start = start;
    return node;
}

VariableDeclaratorNode* create_variable_declarator_node(ASTNode* identifier, ASTNode* expression) {
    VariableDeclaratorNode* node = (VariableDeclaratorNode*)malloc(sizeof(VariableDeclaratorNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_VARIABLE_DECLARATOR;
    node->base.next = NULL;
    node->base.root = NULL;
    node->identifier = identifier;
    node->expression = expression;
    return node;
}

ParameterNode* create_parameter_node(ASTNode* type, ASTNode* name_identifier) {
    ParameterNode* node = (ParameterNode*)malloc(sizeof(ParameterNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_PARAMETER;
    node->base.next = NULL;
    node->base.root = NULL;
    node->type = type;
    node->name_identifier = name_identifier;
    return node;
}

CompoundStatementNode* create_compound_statement_node(ASTNode* start) {
    CompoundStatementNode* node = (CompoundStatementNode*)malloc(sizeof(CompoundStatementNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_COMPOUND_STATEMENT;
    node->base.next = NULL;
    node->base.root = NULL;
    node->start = start;
    return node;
}

IfStatementNode* create_if_statement_node(ASTNode* condition, ASTNode* body, ASTNode* else_body) {
    IfStatementNode* node = (IfStatementNode*)malloc(sizeof(IfStatementNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_IF_STATEMENT;
    node->base.next = NULL;
    node->base.root = NULL;
    node->condition = condition;
    node->body = body;
    node->else_body = else_body;
    return node;
}

WhileStatementNode* create_while_statement_node(ASTNode* condition, ASTNode* body) {
    WhileStatementNode* node = (WhileStatementNode*)malloc(sizeof(WhileStatementNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_WHILE_STATEMENT;
    node->base.next = NULL;
    node->base.root = NULL;
    node->condition = condition;
    node->body = body;
    return node;
}

ReturnStatementNode* create_return_statement_node(ASTNode* expression) {
    ReturnStatementNode* node = (ReturnStatementNode*)malloc(sizeof(ReturnStatementNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_RETURN_STATEMENT;
    node->base.next = NULL;
    node->base.root = NULL;
    node->expression = expression;
    return node;
}

AssignmentStatementNode* create_assignment_statement_node(ASTNode* identifier, ASTNode* expression) {
    AssignmentStatementNode* node = (AssignmentStatementNode*)malloc(sizeof(AssignmentStatementNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_ASSIGNMENT_STATEMENT;
    node->base.next = NULL;
    node->base.root = NULL;
    node->identifier = identifier;
    node->expression = expression;
    return node;
}

BinaryExpressionNode* create_binary_expression_node(int op, ASTNode* left, ASTNode* right) {
    BinaryExpressionNode* node = (BinaryExpressionNode*)malloc(sizeof(BinaryExpressionNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_BINARY_EXPRESSION;
    node->base.next = NULL;
    node->base.root = NULL;
    node->op = op;
    node->left = left;
    node->right = right;
    return node;
}

FunctionCallNode* create_function_call_node(ASTNode* identifier, ASTNode* params) {
    FunctionCallNode* node = (FunctionCallNode*)malloc(sizeof(FunctionCallNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_FUNCTION_CALL;
    node->base.next = NULL;
    node->base.root = NULL;
    node->identifier = identifier;
    node->params = params;
    return node;
}

IdentifierNode* create_identifier_node(Symbol* symbol) {
    IdentifierNode* node = (IdentifierNode*)malloc(sizeof(IdentifierNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_IDENTIFIER;
    node->base.next = NULL;
    node->base.root = NULL;
    node->symbol = symbol;
    return node;
}

NumberNode* create_number_literal_node(int value) {
    NumberNode* node = (NumberNode*)malloc(sizeof(NumberNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_NUMBER_LITERAL;
    node->base.next = NULL;
    node->base.root = NULL;
    node->value = value;
    return node;
}

TypeNode* create_type_node(int type_specifier) {
    TypeNode* node = (TypeNode*)malloc(sizeof(TypeNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_TYPE;
    node->base.next = NULL;
    node->base.root = NULL;
    node->type = type_specifier;
    return node;
}

ErrorNode* create_error_node() {
    ErrorNode* node = (ErrorNode*)malloc(sizeof(ErrorNode));
    if (node == NULL) return NULL;
    
    node->base.type = NODE_ERROR;
    node->base.next = NULL;
    node->base.root = NULL;
    return node;
}

// Function to free an AST
void free_ast(ASTNode* node) {
    if (node == NULL) {
        return;
    }

    // For a full AST, handle each node type:
    switch (node->type) {
        case NODE_PROGRAM:
            free_ast(((ProgramNode*)node)->start);
            break;
        case NODE_FUNCTION_DEFINITION:
            free_ast(((FunctionDefinitionNode*)node)->type);
            free_ast(((FunctionDefinitionNode*)node)->name_identifier);
            free_ast(((FunctionDefinitionNode*)node)->params);
            free_ast(((FunctionDefinitionNode*)node)->body);
            break;
        case NODE_GLOBAL_VARIABLE_DECLARATION:
            free_ast(((GlobalVariableDeclarationNode*)node)->type);
            free_ast(((GlobalVariableDeclarationNode*)node)->start);
            break;
        case NODE_LOCAL_VARIABLE_DECLARATION:
            free_ast(((LocalVariableDeclarationNode*)node)->type);
            free_ast(((LocalVariableDeclarationNode*)node)->start);
            break;
        case NODE_VARIABLE_DECLARATOR:
            free_ast(((VariableDeclaratorNode*)node)->identifier);
            free_ast(((VariableDeclaratorNode*)node)->expression);
            break;
        case NODE_PARAMETER:
            free_ast(((ParameterNode*)node)->type);
            free_ast(((ParameterNode*)node)->name_identifier);
            break;
        case NODE_COMPOUND_STATEMENT:
            free_ast(((CompoundStatementNode*)node)->start);
            break;
        case NODE_IF_STATEMENT:
            free_ast(((IfStatementNode*)node)->condition);
            free_ast(((IfStatementNode*)node)->body);
            free_ast(((IfStatementNode*)node)->else_body);
            break;
        case NODE_WHILE_STATEMENT:
            free_ast(((WhileStatementNode*)node)->condition);
            free_ast(((WhileStatementNode*)node)->body);
            break;
        case NODE_RETURN_STATEMENT:
            free_ast(((ReturnStatementNode*)node)->expression);
            break;
        case NODE_ASSIGNMENT_STATEMENT:
            free_ast(((AssignmentStatementNode*)node)->identifier);
            free_ast(((AssignmentStatementNode*)node)->expression);
            break;
        case NODE_BINARY_EXPRESSION:
            free_ast(((BinaryExpressionNode*)node)->left);
            free_ast(((BinaryExpressionNode*)node)->right);
            break;
        case NODE_FUNCTION_CALL:
            free_ast(((FunctionCallNode*)node)->identifier);
            free_ast(((FunctionCallNode*)node)->params);
            break;
        case NODE_IDENTIFIER:
        case NODE_NUMBER_LITERAL:
        case NODE_TYPE:
        case NODE_ERROR:
            // No children to free
            break;
    }

    // Free all nodes in a 'next' linked list
    ASTNode* next_sibling = node->next;
    free(node); // Free the current node itself
    free_ast(next_sibling); // Then free the rest of the list
}

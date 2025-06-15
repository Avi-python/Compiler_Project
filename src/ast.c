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

// Recursive helper function to traverse the AST and write DOT representation
void visualize_ast_recursive(ASTNode* node, FILE* fp) {
    if (!node) return;

    // Define the current node with a unique ID (using pointer address) and label
    fprintf(fp, "  node%p [label=\"", (void*)node);

    switch (node->type) {
        case NODE_PROGRAM: {
            ProgramNode* pNode = (ProgramNode*)node;
            fprintf(fp, "Program");
            break;
        }
        case NODE_FUNCTION_DEFINITION: {
            FunctionDefinitionNode* fdNode = (FunctionDefinitionNode*)node;
            fprintf(fp, "FunctionDefinition");
            break;
        }
        case NODE_GLOBAL_VARIABLE_DECLARATION: {
            GlobalVariableDeclarationNode* gvdNode = (GlobalVariableDeclarationNode*)node;
            fprintf(fp, "GlobalVariableDeclaration");
            break;
        }
        case NODE_LOCAL_VARIABLE_DECLARATION: {
            LocalVariableDeclarationNode* lvdNode = (LocalVariableDeclarationNode*)node;
            fprintf(fp, "LocalVariableDeclaration");
            break;
        }
        case NODE_VARIABLE_DECLARATOR: {
            VariableDeclaratorNode* vdNode = (VariableDeclaratorNode*)node;
            fprintf(fp, "VariableDeclarator");
            break;
        }
        case NODE_PARAMETER: {
            ParameterNode* paramNode = (ParameterNode*)node;
            fprintf(fp, "Parameter");
            break;
        }
        case NODE_COMPOUND_STATEMENT: {
            CompoundStatementNode* csNode = (CompoundStatementNode*)node;
            fprintf(fp, "CompoundStatement");
            break;
        }
        case NODE_IF_STATEMENT: {
            IfStatementNode* ifNode = (IfStatementNode*)node;
            fprintf(fp, "IfStatement");
            break;
        }
        case NODE_WHILE_STATEMENT: {
            WhileStatementNode* whileNode = (WhileStatementNode*)node;
            fprintf(fp, "WhileStatement");
            break;
        }
        case NODE_RETURN_STATEMENT: {
            ReturnStatementNode* retNode = (ReturnStatementNode*)node;
            fprintf(fp, "ReturnStatement");
            break;
        }
        case NODE_ASSIGNMENT_STATEMENT: {
            AssignmentStatementNode* asNode = (AssignmentStatementNode*)node;
            fprintf(fp, "AssignmentStatement");
            break;
        }
        case NODE_BINARY_EXPRESSION: {
            BinaryExpressionNode* binOpNode = (BinaryExpressionNode*)node;
            // Assuming you have a way to map op code to string, e.g., a helper function or switch
            fprintf(fp, "BinaryExpression: Op %d", binOpNode->op);
            break;
        }
        case NODE_FUNCTION_CALL: {
            FunctionCallNode* fcNode = (FunctionCallNode*)node;
            fprintf(fp, "FunctionCall");
            break;
        }
        case NODE_IDENTIFIER: {
            IdentifierNode* idNode = (IdentifierNode*)node;
            fprintf(fp, "Identifier: %s", idNode->symbol ? idNode->symbol->name : "NULL_SYMBOL");
            break;
        }
        case NODE_NUMBER_LITERAL: {
            NumberNode* numNode = (NumberNode*)node;
            fprintf(fp, "Number: %d", numNode->value);
            break;
        }
        case NODE_TYPE: {
            TypeNode* typeNode = (TypeNode*)node;
            // Assuming you have a way to map type specifier to string
            fprintf(fp, "Type: %d", typeNode->type);
            break;
        }
        case NODE_ERROR: {
            fprintf(fp, "ErrorNode");
            break;
        }
        default:
            fprintf(fp, "Unknown Node: %d", node->type);
            break;
    }
    fprintf(fp, "\"];\n");

    // Process children and create edges
    switch (node->type) {
        case NODE_PROGRAM: {
            ProgramNode* pNode = (ProgramNode*)node;
            if (pNode->start) {
                visualize_ast_recursive(pNode->start, fp);
                fprintf(fp, "  node%p -> node%p [label=\"start\"];\n", (void*)node, (void*)pNode->start);
            }
            break;
        }
        case NODE_FUNCTION_DEFINITION: {
            FunctionDefinitionNode* fdNode = (FunctionDefinitionNode*)node;
            if (fdNode->type) {
                visualize_ast_recursive(fdNode->type, fp);
                fprintf(fp, "  node%p -> node%p [label=\"type\"];\n", (void*)node, (void*)fdNode->type);
            }
            if (fdNode->name_identifier) {
                visualize_ast_recursive(fdNode->name_identifier, fp);
                fprintf(fp, "  node%p -> node%p [label=\"name\"];\n", (void*)node, (void*)fdNode->name_identifier);
            }
            if (fdNode->params) {
                visualize_ast_recursive(fdNode->params, fp);
                fprintf(fp, "  node%p -> node%p [label=\"params\"];\n", (void*)node, (void*)fdNode->params);
            }
            if (fdNode->body) {
                visualize_ast_recursive(fdNode->body, fp);
                fprintf(fp, "  node%p -> node%p [label=\"body\"];\n", (void*)node, (void*)fdNode->body);
            }
            break;
        }
        case NODE_GLOBAL_VARIABLE_DECLARATION: {
            GlobalVariableDeclarationNode* gvdNode = (GlobalVariableDeclarationNode*)node;
            if (gvdNode->type) {
                visualize_ast_recursive(gvdNode->type, fp);
                fprintf(fp, "  node%p -> node%p [label=\"type\"];\n", (void*)node, (void*)gvdNode->type);
            }
            if (gvdNode->start) { // List of VariableDeclaratorNode
                visualize_ast_recursive(gvdNode->start, fp);
                fprintf(fp, "  node%p -> node%p [label=\"declarators\"];\n", (void*)node, (void*)gvdNode->start);
            }
            break;
        }
        case NODE_LOCAL_VARIABLE_DECLARATION: {
            LocalVariableDeclarationNode* lvdNode = (LocalVariableDeclarationNode*)node;
            if (lvdNode->type) {
                visualize_ast_recursive(lvdNode->type, fp);
                fprintf(fp, "  node%p -> node%p [label=\"type\"];\n", (void*)node, (void*)lvdNode->type);
            }
            if (lvdNode->start) { // List of VariableDeclaratorNode
                visualize_ast_recursive(lvdNode->start, fp);
                fprintf(fp, "  node%p -> node%p [label=\"declarators\"];\n", (void*)node, (void*)lvdNode->start);
            }
            break;
        }
        case NODE_VARIABLE_DECLARATOR: {
            VariableDeclaratorNode* vdNode = (VariableDeclaratorNode*)node;
            if (vdNode->identifier) {
                visualize_ast_recursive(vdNode->identifier, fp);
                fprintf(fp, "  node%p -> node%p [label=\"identifier\"];\n", (void*)node, (void*)vdNode->identifier);
            }
            if (vdNode->expression) {
                visualize_ast_recursive(vdNode->expression, fp);
                fprintf(fp, "  node%p -> node%p [label=\"expression\"];\n", (void*)node, (void*)vdNode->expression);
            }
            break;
        }
        case NODE_PARAMETER: {
            ParameterNode* paramNode = (ParameterNode*)node;
            if (paramNode->type) {
                visualize_ast_recursive(paramNode->type, fp);
                fprintf(fp, "  node%p -> node%p [label=\"type\"];\n", (void*)node, (void*)paramNode->type);
            }
            if (paramNode->name_identifier) {
                visualize_ast_recursive(paramNode->name_identifier, fp);
                fprintf(fp, "  node%p -> node%p [label=\"name\"];\n", (void*)node, (void*)paramNode->name_identifier);
            }
            break;
        }
        case NODE_COMPOUND_STATEMENT: {
            CompoundStatementNode* csNode = (CompoundStatementNode*)node;
            if (csNode->start) { // List of StatementNode
                visualize_ast_recursive(csNode->start, fp);
                fprintf(fp, "  node%p -> node%p [label=\"statements\"];\n", (void*)node, (void*)csNode->start);
            }
            break;
        }
        case NODE_IF_STATEMENT: {
            IfStatementNode* ifNode = (IfStatementNode*)node;
            if (ifNode->condition) {
                visualize_ast_recursive(ifNode->condition, fp);
                fprintf(fp, "  node%p -> node%p [label=\"condition\"];\n", (void*)node, (void*)ifNode->condition);
            }
            if (ifNode->body) {
                visualize_ast_recursive(ifNode->body, fp);
                fprintf(fp, "  node%p -> node%p [label=\"body\"];\n", (void*)node, (void*)ifNode->body);
            }
            if (ifNode->else_body) {
                visualize_ast_recursive(ifNode->else_body, fp);
                fprintf(fp, "  node%p -> node%p [label=\"else_body\"];\n", (void*)node, (void*)ifNode->else_body);
            }
            break;
        }
        case NODE_WHILE_STATEMENT: {
            WhileStatementNode* whileNode = (WhileStatementNode*)node;
            if (whileNode->condition) {
                visualize_ast_recursive(whileNode->condition, fp);
                fprintf(fp, "  node%p -> node%p [label=\"condition\"];\n", (void*)node, (void*)whileNode->condition);
            }
            if (whileNode->body) {
                visualize_ast_recursive(whileNode->body, fp);
                fprintf(fp, "  node%p -> node%p [label=\"body\"];\n", (void*)node, (void*)whileNode->body);
            }
            break;
        }
        case NODE_RETURN_STATEMENT: {
            ReturnStatementNode* retNode = (ReturnStatementNode*)node;
            if (retNode->expression) {
                visualize_ast_recursive(retNode->expression, fp);
                fprintf(fp, "  node%p -> node%p [label=\"expression\"];\n", (void*)node, (void*)retNode->expression);
            }
            break;
        }
        case NODE_ASSIGNMENT_STATEMENT: {
            AssignmentStatementNode* asNode = (AssignmentStatementNode*)node;
            if (asNode->identifier) {
                visualize_ast_recursive(asNode->identifier, fp);
                fprintf(fp, "  node%p -> node%p [label=\"identifier\"];\n", (void*)node, (void*)asNode->identifier);
            }
            if (asNode->expression) {
                visualize_ast_recursive(asNode->expression, fp);
                fprintf(fp, "  node%p -> node%p [label=\"expression\"];\n", (void*)node, (void*)asNode->expression);
            }
            break;
        }
        case NODE_BINARY_EXPRESSION: {
            BinaryExpressionNode* binOpNode = (BinaryExpressionNode*)node;
            if (binOpNode->left) {
                visualize_ast_recursive(binOpNode->left, fp);
                fprintf(fp, "  node%p -> node%p [label=\"left\"];\n", (void*)node, (void*)binOpNode->left);
            }
            if (binOpNode->right) {
                visualize_ast_recursive(binOpNode->right, fp);
                fprintf(fp, "  node%p -> node%p [label=\"right\"];\n", (void*)node, (void*)binOpNode->right);
            }
            break;
        }
        case NODE_FUNCTION_CALL: {
            FunctionCallNode* fcNode = (FunctionCallNode*)node;
            if (fcNode->identifier) {
                visualize_ast_recursive(fcNode->identifier, fp);
                fprintf(fp, "  node%p -> node%p [label=\"identifier\"];\n", (void*)node, (void*)fcNode->identifier);
            }
            if (fcNode->params) { // List of ExpressionNode
                visualize_ast_recursive(fcNode->params, fp);
                fprintf(fp, "  node%p -> node%p [label=\"arguments\"];\n", (void*)node, (void*)fcNode->params);
            }
            break;
        }
        case NODE_IDENTIFIER:    // Leaf node
        case NODE_NUMBER_LITERAL: // Leaf node
        case NODE_TYPE:          // Leaf node
        case NODE_ERROR:         // Leaf node
            break;
        default: // Should not happen if all types are covered above
            break;
    }

    // Process the 'next' pointer for lists of nodes (e.g., statements, parameters, declarators)
    if (node->next) {
        visualize_ast_recursive(node->next, fp);
        fprintf(fp, "  node%p -> node%p [label=\"next\", style=dotted];\n", (void*)node, (void*)node->next);
    }
}

// Main function to generate the DOT file
void visualize_ast(ASTNode* node, const char* filename) {
    if (!node) return;

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to open file for AST visualization");
        return;
    }

    fprintf(fp, "digraph AST {\n");
    fprintf(fp, "  node [shape=box];\n"); // Optional: style nodes
    visualize_ast_recursive(node, fp);
    fprintf(fp, "}\n");

    fclose(fp);
    printf("AST visualization saved to %s\n", filename);
    printf("Run 'dot -Tpng %s -o ast.png' to generate the image.\n", filename);
}
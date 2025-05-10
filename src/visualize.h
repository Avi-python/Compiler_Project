#ifndef VISUALIZE_H
#define VISUALIZE_H

#include <stdio.h>
#include "expression.h"
#include "parser.tab.h"

// Recursive helper function to traverse the AST and write DOT representation
void visualize_ast_recursive(ASTNode* node, FILE* fp) {
    if (!node) return;

    // Define the current node with a unique ID (using pointer address) and label
    fprintf(fp, "  node%p [label=\"", (void*)node);

    switch (node->type) {
        case NODE_ASSIGNMENT: {
            AssignmentNode* asNode = (AssignmentNode*)node;
            fprintf(fp, "Assign");
            break;
        }
        case NODE_BINARY_OP: {
            BinaryOpNode* binOpNode = (BinaryOpNode*)node;
            fprintf(fp, "Op: %d", binOpNode->op);
            break;
        }
        case NODE_NUMBER: {
            NumberNode* numNode = (NumberNode*)node;
            fprintf(fp, "Num: %d", numNode->value);
            break;
        }
        case NODE_IDENTIFIER: {
            IdentifierNode* idNode = (IdentifierNode*)node;
            fprintf(fp, "ID: %s", idNode->name);
            break;
        }
        default:
            fprintf(fp, "Unknown Node");
            break;
    }
    fprintf(fp, "\"];\n");

    // Process children and create edges
    switch (node->type) {
        case NODE_ASSIGNMENT: {
            AssignmentNode* asNode = (AssignmentNode*)node;
            if (asNode->expression) {
                visualize_ast_recursive(asNode->expression, fp);
                visualize_ast_recursive(asNode->identifier, fp);
                fprintf(fp, "  node%p -> node%p [label=\"expr\"];\n", (void*)node, (void*)asNode->expression);
                fprintf(fp, "  node%p -> node%p [label=\"ident\"];\n", (void*)node, (void*)asNode->identifier);
            }
            break;
        }
        case NODE_BINARY_OP: {
            BinaryOpNode* binOpNode = (BinaryOpNode*)node;
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
        case NODE_NUMBER: // Leaf node
        case NODE_IDENTIFIER: // Leaf node
            break;
    }

    // next
    if (node->next) {
        visualize_ast_recursive(node->next, fp);
        fprintf(fp, "  node%p -> node%p [label=\"next\"];\n", (void*)node, (void*)node->next);
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
    // Optional: Print message indicating success and how to generate image
    // printf("AST visualization saved to %s\n", filename);
    // printf("Run 'dot -Tpng %s -o ast.png' to generate the image.\n", filename);
}

// ... rest of the functions (evaluate, free_ast, main, yyerror) ...

#endif // VISUALIZE_H 
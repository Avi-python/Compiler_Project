%locations
%define parse.error detailed

%{
extern int yylineno;
void yyerror(char *s);
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "visualize.h"
%}

%code requires {
    #include "expression.h" // custom headers should put here
}

%code {
    // custom function declarations with custom types should put here
    // int symbolVal(char symbol); 
    // void updateSymbolVal(char symbol, int val);
    // int evaluate(ASTNode* node);
    void free_ast(ASTNode* node);
}

%union  {
    int ival; 
    char* sval;
    ASTNode* node;
}

%start block
%token <sval> IDENTIFIER
%token <ival> NUMBER
%token PLUS MINUS MUL DIV ASSIGN SEMI
%token L_PAREN R_PAREN L_BRACE R_BRACE
%token INT CHAR RETURN IF ELSE DO WHILE CONST
%type <node> expr term factor block stmt_list stmt assign_stmt decl_stmt

%%

block
    : L_BRACE stmt_list R_BRACE {
        visualize_ast($2, "ast.dot");
        free_ast($2);
        $$ = NULL;
    }

stmt_list
    : stmt_list stmt SEMI { 
        $1->next = $2; 
        $$ = $1; 
    }
    | stmt SEMI { 
        $$ = $1; 
    }

stmt
    : decl_stmt { $$ = $1; }
    | assign_stmt { $$ = $1; }
    | block { $$ = $1; }
    ;

/* TODO : should solve the problem of scope */

decl_stmt   
    : INT IDENTIFIER {
        IdentifierNode* node = malloc(sizeof(IdentifierNode));
        node->base.type = NODE_IDENTIFIER;
        node->name = $2;
        $$ = (ASTNode*)node;
    }
    | CHAR IDENTIFIER {
        IdentifierNode* node = malloc(sizeof(IdentifierNode));
        node->base.type = NODE_IDENTIFIER;
        node->name = $2;
        $$ = (ASTNode*)node;
    }
    ;
assign_stmt
    : IDENTIFIER ASSIGN expr {
        IdentifierNode* ident_node = malloc(sizeof(IdentifierNode));
        ident_node->base.type = NODE_IDENTIFIER;
        ident_node->name = $1;

        AssignmentNode* node = malloc(sizeof(AssignmentNode));
        node->base.type = NODE_ASSIGNMENT;
        node->identifier = ident_node;
        node->expression = $3;
        $$ = (ASTNode*)node;
    }
    ;

expr
    : expr PLUS term          {
        BinaryOpNode* node = malloc(sizeof(BinaryOpNode));
        node->base.type = NODE_BINARY_OP;
        node->op = PLUS;
        node->left = $1;
        node->right = $3;
        $$ = (ASTNode*)node;
    }
    | expr MINUS term          {
        BinaryOpNode* node = malloc(sizeof(BinaryOpNode));
        node->base.type = NODE_BINARY_OP;
        node->op = MINUS;
        node->left = $1;
        node->right = $3;
        $$ = (ASTNode*)node;
    }
    | term                   { $$ = $1; }
    ;

term
    : term MUL factor        {
        BinaryOpNode* node = malloc(sizeof(BinaryOpNode));
        node->base.type = NODE_BINARY_OP;
        node->op = MUL;
        node->left = $1;
        node->right = $3;
        $$ = (ASTNode*)node;
    }
    | term DIV factor        {
        @$.first_column = @1.first_column;
        @$.last_line = @3.last_line;

        // if(!evaluate($3))
        // {
        //     yyerror("division by zero");
        //     $$ = NULL;
        // }
        // else
        // {
        BinaryOpNode* node = malloc(sizeof(BinaryOpNode));
        node->base.type = NODE_BINARY_OP;
        node->op = DIV;
        node->left = $1;
        node->right = $3;
        $$ = (ASTNode*)node;
        // }
    }
    | factor                 { $$ = $1; }
    ;

factor
    : NUMBER {
        NumberNode* node = malloc(sizeof(NumberNode));
        node->base.type = NODE_NUMBER;
        node->value = $1;
        $$ = (ASTNode*)node;
    }
    | IDENTIFIER {
        IdentifierNode* node = malloc(sizeof(IdentifierNode));
        node->base.type = NODE_IDENTIFIER;
        node->name = $1;
        $$ = (ASTNode*)node;
    }
    | L_PAREN expr R_PAREN { $$ = $2; }
    | L_PAREN expr {
        yyerror("missing closing parenthesis");
        $$ = $2;
    }
    ;

%%

/* int computeSymbolIndex(char token)
{
    int idx = -1;
    if(islower(token)) 
    {
        idx = token - 'a' + 26;
    }
    else if(isupper(token))
    {
        idx = token - 'A';
    }
    return idx;
}

int symbolVal(char symbol)
{
    int bucket = computeSymbolIndex(symbol);
    return symbols[bucket];
}

void updateSymbolVal(char symbol, int val)
{
    int bucket = computeSymbolIndex(symbol);
    symbols[bucket] = val;
} */

/* int evaluate(ASTNode* node)
{
    if (!node) return 0;

    switch (node->type) {

        case NODE_ASSIGNMENT: {
            AssignmentNode* asNode = (AssignmentNode*)node;
            char ident = asNode->identifier;
            int val = evaluate(asNode->expression);
            updateSymbolVal(ident, val); 
            break;
        }
        case NODE_NUMBER: {
            NumberNode* numNode = (NumberNode*)node;
            return numNode->value;
        }
        case NODE_BINARY_OP: {
            BinaryOpNode* binOpNode = (BinaryOpNode*)node;
            int leftVal = evaluate(binOpNode->left);
            int rightVal = evaluate(binOpNode->right);
            switch (binOpNode->op) {
                case '+': return leftVal + rightVal;
                case '-': return leftVal - rightVal;
                case '*': return leftVal * rightVal;
                case '/': return leftVal / rightVal;
            }
        }
        case NODE_IDENTIFIER: {
            IdentifierNode* idNode = (IdentifierNode*)node;
            return symbolVal(idNode->name);
        }
        default:
            return 0;
    }
} */

void free_ast(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_ASSIGNMENT: {
            AssignmentNode* assignNode = (AssignmentNode*)node;
            free_ast(assignNode->expression);
            free_ast(assignNode->identifier);
            break;
        }
        case NODE_BINARY_OP: {
            BinaryOpNode* binOpNode = (BinaryOpNode*)node;
            free_ast(binOpNode->left);
            free_ast(binOpNode->right);
            break;
        }
        case NODE_IDENTIFIER: {
            IdentifierNode* idNode = (IdentifierNode*)node;
            free(idNode->name);
            break;
        }
        case NODE_NUMBER:
            break;
    }

    if(node->next != NULL)
    {
        free_ast(node->next);
    }

    free(node);
}

int main(void)
{
    /* int i;
    for(int i = 0; i < 52; i++)
    {
        symbols[i] = 0;
    } */

    return yyparse();
}

void yyerror(char *s) 
{
    extern char* yytext;
    fprintf(stderr, "error: %d:%d (Near token: '%s'): %s\n", 
            yylloc.first_line, yylloc.first_column, yytext, s);
}
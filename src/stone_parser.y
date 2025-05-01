%{
void yyerror(char *s);
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
int symbols[52];
%}

%code requires {
    #include "expression.h" // custom headers should put here
}

%code {
    // custom function declarations with custom types should put here
    int symbolVal(char symbol); 
    void updateSymbolVal(char symbol, int val);
    int evaluate(ASTNode* node);
    void free_ast(ASTNode* node);
}

%union  {
    int num; 
    char id;
    ASTNode* node;
}

%start line
%token print
%token exit_command
%token L_PAREN R_PAREN
%token <num> number
%token <id> identifier
%type <node> line expr term factor assignment

%%

line
    : assignment ';'         {evaluate($1); free_ast($1); $$ = NULL;}
    | exit_command ';'       {exit(EXIT_SUCCESS);}
    | print expr ';'         {printf("Printing %d\n", evaluate($2)); free_ast($2); $$ = NULL;}
    | line assignment ';'    {evaluate($2); free_ast($2); $$ = NULL;}
    | line print expr ';'    {printf("Printing %d\n", evaluate($3)); free_ast($3); $$ = NULL;}
    | line exit_command ';'  {exit(EXIT_SUCCESS);}
    ;

assignment
    : identifier '=' expr    {
        AssignmentNode* node = malloc(sizeof(AssignmentNode));
        node->base.type = NODE_ASSIGNMENT;
        node->identifier = $1;
        node->expression = $3;
        $$ = (ASTNode*)node;
    }
    ;

expr
    : expr '+' term          {
        BinaryOpNode* node = malloc(sizeof(BinaryOpNode));
        node->base.type = NODE_BINARY_OP;
        node->op = '+';
        node->left = $1;
        node->right = $3;
        $$ = (ASTNode*)node;
    }
    | expr '-' term          {
        BinaryOpNode* node = malloc(sizeof(BinaryOpNode));
        node->base.type = NODE_BINARY_OP;
        node->op = '-';
        node->left = $1;
        node->right = $3;
        $$ = (ASTNode*)node;
    }
    | term                   {$$ = $1;}
    ;

term
    : term '*' factor        {
        BinaryOpNode* node = malloc(sizeof(BinaryOpNode));
        node->base.type = NODE_BINARY_OP;
        node->op = '*';
        node->left = $1;
        node->right = $3;
        $$ = (ASTNode*)node;
    }
    | term '/' factor        {
        BinaryOpNode* node = malloc(sizeof(BinaryOpNode));
        node->base.type = NODE_BINARY_OP;
        node->op = '/';
        node->left = $1;
        node->right = $3;
        $$ = (ASTNode*)node;
    }
    | factor                 {$$ = $1;}
    ;

factor
    : number                 {
        NumberNode* node = malloc(sizeof(NumberNode));
        node->base.type = NODE_NUMBER;
        node->value = $1;
        $$ = (ASTNode*)node;
    }
    | identifier             {
        IdentifierNode* node = malloc(sizeof(IdentifierNode));
        node->base.type = NODE_IDENTIFIER;
        node->name = $1;
        $$ = (ASTNode*)node;
    }
    | L_PAREN expr R_PAREN   {$$ = $2;}
    ;

%%

int computeSymbolIndex(char token)
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
}

int evaluate(ASTNode* node)
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
}

void free_ast(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_ASSIGNMENT: {
            AssignmentNode* assignNode = (AssignmentNode*)node;
            free_ast(assignNode->expression);
            break;
        }
        case NODE_BINARY_OP: {
            BinaryOpNode* binOpNode = (BinaryOpNode*)node;
            free_ast(binOpNode->left);
            free_ast(binOpNode->right);
            break;
        }
        case NODE_NUMBER:
        case NODE_IDENTIFIER:
            break;
    }

    free(node);
}

int main(void)
{
    int i;
    for(int i = 0; i < 52; i++)
    {
        symbols[i] = 0;
    }

    return yyparse();
}

void yyerror(char *s) {fprintf(stderr, "%s\n", s);}
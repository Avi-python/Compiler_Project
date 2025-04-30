%{
void yyerror(char *s);
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
int symbols[52];
int symbolVal(char symbol);
void updateSymbolVal(char symbol, int val);
%}

%union  {
    int num; 
    char id;
}

%start line
%token print
%token exit_command
%token L_PAREN R_PAREN
%token <num> number
%token <id> identifier
%type <num> line expr term factor
%type <id> assignment

%%

line
    : assignment ';'         {;}
    | exit_command ';'       {exit(EXIT_SUCCESS);}
    | print expr ';'         {printf("Printing %d\n", $2);}
    | line assignment ';'    {;}
    | line print expr ';'    {printf("Printing %d\n", $3);}
    | line exit_command ';'  {exit(EXIT_SUCCESS);}
    ;

assignment
    : identifier '=' expr    {updateSymbolVal($1, $3);}
    ;

expr
    : expr '+' term          {$$ = $1 + $3;}
    | expr '-' term          {$$ = $1 - $3;}
    | term                   {$$ = $1;}
    ;

term
    : term '*' factor        {$$ = $1 * $3;}
    | term '/' factor        {$$ = $1 / $3;}
    | factor                 {$$ = $1;}
    ;

factor
    : number                 {$$ = $1;}
    | identifier             {$$ = symbolVal($1);}
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
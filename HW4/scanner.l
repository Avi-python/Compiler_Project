%{
#include "tokens.h"
#include <string.h> // Required for strdup if yylval is char* and you copy yytext

extern void yyerror(const char *s); 

int lineno = 1;
int column = 1;

%}

alpha       [a-zA-Z]
ws          [ \t]+
dig         [0-9]
number      {dig}+
identifier  {alpha}({alpha}|{dig}|_)*
character   \'([^'])\'

%%

{ws}            { column += yyleng; }
\n              { lineno++; column = 1; }
"+"             { column += yyleng; return PLUS; }
"-"             { column += yyleng; return MINUS; }
"*"             { column += yyleng; return MUL; }
"/"             { column += yyleng; return DIV; }
"="             { column += yyleng; return ASSIGN; }
"("             { column += yyleng; return LPAREN; }  
")"             { column += yyleng; return RPAREN; }
"{"             { column += yyleng; return LBRACE; }
"}"             { column += yyleng; return RBRACE; }
";"             { column += yyleng; return SEMI; }
{identifier}    { yylval.sval = strdup(yytext); column += yyleng; return IDENTIFIER; } // Assuming yylval.sval is char*
{number}        { yylval.ival = atoi(yytext); column += yyleng; return NUMBER; } // Assuming yylval.ival is int
{character}     { yylval.sval = strdup(yytext); column += yyleng; return CHAR; } // Assuming yylval.sval is char* for the literal string
.               { yyerror("unexpected character"); column += yyleng; /* Still update column */ }

%%

int yywrap(void) {return 1;}
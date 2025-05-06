%{
#include "tokens.h"
#include <string.h> // Required for strdup if yylval is char* and you copy yytext

extern void yyerror(const char *s); 

void clear_buffer();
void push_to_buffer(char* c);

int lineno = 1;
int column = 1;
char current_line_buffer[1024];


%}

alpha       [a-zA-Z]
ws          [ \t]+
dig         [0-9]
number      {dig}+
identifier  {alpha}({alpha}|{dig}|_)*
character   \'([^'])\'

%%

{ws}            { column += yyleng; push_to_buffer(yytext); }
\n              { lineno++; column = 1; clear_buffer(); }
"+"             { column += yyleng; push_to_buffer(yytext); return PLUS; }
"-"             { column += yyleng; push_to_buffer(yytext); return MINUS; }
"*"             { column += yyleng; push_to_buffer(yytext); return MUL; }
"/"             { column += yyleng; push_to_buffer(yytext); return DIV; }
"="             { column += yyleng; push_to_buffer(yytext); return ASSIGN; }
"("             { column += yyleng; push_to_buffer(yytext); return LPAREN; }  
")"             { column += yyleng; push_to_buffer(yytext); return RPAREN; }
"{"             { column += yyleng; push_to_buffer(yytext); return LBRACE; }
"}"             { column += yyleng; push_to_buffer(yytext); return RBRACE; }
";"             { column += yyleng; push_to_buffer(yytext); return SEMI; }
{identifier}    { yylval.sval = strdup(yytext); column += yyleng; push_to_buffer(yytext); return IDENTIFIER; } // Assuming yylval.sval is char*
{number}        { yylval.ival = atoi(yytext); column += yyleng; push_to_buffer(yytext); return NUMBER; } // Assuming yylval.ival is int
{character}     { yylval.sval = strdup(yytext); column += yyleng; push_to_buffer(yytext); return CHAR; } // Assuming yylval.sval is char* for the literal string
.               { column += yyleng; push_to_buffer(yytext); yyerror("unexpected character"); }

%%

int yywrap(void) {return 1;}

void clear_buffer() {
    memset(current_line_buffer, 0, sizeof(current_line_buffer));
}

void push_to_buffer(char* c) {
    if (strlen(current_line_buffer) + strlen(c) < sizeof(current_line_buffer)) {
        strcat(current_line_buffer, c);
    } else {
        yyerror("Buffer overflow");
    }
}

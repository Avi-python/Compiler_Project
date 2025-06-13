%{
#include <string.h> // Required for strdup if yylval is char* and you copy yytext
#include "tokens.h"
#include "error.h"
#include "reswords.h"

extern void yyerror(const char *s); 

void save_error_pos(char* type, char* token);
void free_errors_arr();
void show_and_free_errors();

char* yyfilename;
int lineno = 1;
int column = 1;
int error_count = 0;
Error* errors_arr[1024];

%}

%x COMMENT

alpha       [a-zA-Z]
ws          [ \t]+
dig         [0-9]
number      {dig}+
identifier  {alpha}({alpha}|{dig}|_)*
character   \'([^'])\'

%%

{ws}            { column += yyleng; }
\n              { lineno++; column = 1; }

"//".*          { column += yyleng; /* Ignore single-line comment */ }
"/*"            { column += 2; BEGIN(COMMENT); /* Start multi-line comment */ }

<COMMENT>"*/"      { column += 2; BEGIN(INITIAL); /* End multi-line comment */ }
<COMMENT>\n        { lineno++; column = 1; /* Newline inside comment */ }
<COMMENT>.         { column++; /* Character inside comment, ignored */ }
<COMMENT><<EOF>>   { 
                       save_error_pos("lexical error", "unterminated /* comment"); 
                       yyterminate(); /* Terminate scanning due to unterminated comment at EOF */
                   }

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
"=="            { column += yyleng; return EQ; }
"!="            { column += yyleng; return NE; }
"<"             { column += yyleng; return LT; }
"<="            { column += yyleng; return LE; }
">"             { column += yyleng; return GT; }
">="            { column += yyleng; return GE; }
","             { column += yyleng; return COMMA; }

{identifier}    { 
    column += yyleng; 
    struct resword* res = is_resword(yytext, yyleng);
    if(res != NULL)
    {
        yylval.sval = strdup(yytext);
        return res->type;
    }
    yylval.sval = strdup(yytext); 
    return IDENTIFIER; 
} 

{number}        { yylval.ival = atoi(yytext); column += yyleng; return NUMBER; } // Assuming yylval.ival is int
{character}     { yylval.sval = strdup(yytext); column += yyleng; return CHAR; } // Assuming yylval.sval is char* for the literal string
.               { save_error_pos("unexpected character", yytext); column += yyleng; }

%%

int yywrap(void) { return 1; }

void show_and_free_errors() 
{
    // read file
    FILE *file = fopen(yyfilename, "r");

    int current_line = 0;
    char line[256];
    for (int i = 0; i < error_count; i++) 
    {
        int row = errors_arr[i]->row;
        while(current_line < row) 
        {
            if (fgets(line, sizeof(line), file) == NULL) 
            {
                line[0] = '\0';
                break; // End of file
            }
            current_line++;
        }
        
        line[strcspn(line, "\n")] = '\0'; // Remove newline character

        fprintf(stderr, "%s:%d:%d: error: %s, %s\n", 
                errors_arr[i]->filename, errors_arr[i]->row, errors_arr[i]->col, errors_arr[i]->type, errors_arr[i]->msg);
        fprintf(stderr, "%d |%s\n", errors_arr[i]->row, line);

        for(int j = 0; j < errors_arr[i]->col + 2; j++) 
        {
            fprintf(stderr, " ");
        }
        fprintf(stderr, "^\n");
    }

    free_errors_arr();
}

void save_error_pos(char* type, char* msg) 
{
    if (error_count < 1024) 
    {
        errors_arr[error_count] = (Error*)malloc(sizeof(Error));
        errors_arr[error_count]->row = lineno;
        errors_arr[error_count]->col = column;

        strncpy(errors_arr[error_count]->filename, yyfilename, sizeof(errors_arr[error_count]->filename) - 1);
        errors_arr[error_count]->filename[sizeof(errors_arr[error_count]->filename) - 1] = '\0'; // Ensure null-termination

        strncpy(errors_arr[error_count]->type, type, sizeof(errors_arr[error_count]->type) - 1);
        errors_arr[error_count]->type[sizeof(errors_arr[error_count]->type) - 1] = '\0'; // Ensure null-termination

        strncpy(errors_arr[error_count]->msg, msg, sizeof(errors_arr[error_count]->msg) - 1);
        errors_arr[error_count]->msg[sizeof(errors_arr[error_count]->msg) - 1] = '\0'; // Ensure null-termination

        error_count++;
    }
    else
    {
        fprintf(stderr, "Error: Too many errors, cannot save more.\n");
    }
}

void free_errors_arr()
{
    for (int i = 0; i < error_count; i++) 
    {
        free(errors_arr[i]);
        errors_arr[i] = NULL;
    }

    error_count = 0;
}

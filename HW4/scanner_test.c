#include <stdio.h>
#include <stdlib.h> // For fopen, fclose, exit
#include <string.h> // For strlen, strdup (though strdup is in lexer)

// This should define token constants (IDENTIFIER, NUMBER, etc.)
// and the YYSTYPE union (e.g., union { int ival; char* sval; }).
#include "tokens.h"
#include "error.h" // For Error struct

// Declarations for functions and variables from the lexer (lex.yy.c)
extern int yylex(void);
extern void show_and_free_errors();
extern FILE *yyin;   // Input stream
extern char *yytext; // Matched text
extern int yyleng;   // Length of matched text
extern int lineno;   // Current line number (from your scanner.l)
extern int column;   // Current column number (from your scanner.l)
extern char *yyfilename; // Current filename (from your scanner.l)

// yylval is used by the lexer to return token values.
// It must be defined in the program that uses the lexer.
YYSTYPE yylval;

// void yyerror(const char *s)
// {
//     fprintf(stderr, "%d:%d error: %s\n", lineno, column - yyleng, s);
//     fprintf(stderr, "%d | %s\n", lineno, );
//     for(size_t i = 0; i < column - yyleng; i++) {
//         fprintf(stderr, " ");
//     }
//     fprintf(stderr, "^--\n");
// }


// Helper function to convert token numeric codes to readable names
const char* token_to_string(int token) {
    switch (token) {
        case IDENTIFIER: return "IDENTIFIER";
        case NUMBER:     return "NUMBER";
        case CHAR:       return "CHAR";
        case PLUS:       return "PLUS";
        case MINUS:      return "MINUS";
        case MUL:        return "MUL";
        case DIV:        return "DIV";
        case LPAREN:     return "LPAREN";
        case RPAREN:     return "RPAREN";
        case LBRACE:     return "LBRACE";
        case RBRACE:     return "RBRACE";
        case SEMI:       return "SEMI";
        case ASSIGN:     return "ASSIGN";
        case RETURN:     return "RETURN";
        // Add any other token names defined in tokens.h
        default: {
            static char unknown_token_str[32];
            sprintf(unknown_token_str, "TOKEN<%d>", token);
            return unknown_token_str;
        }
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        yyfilename = argv[1];
        if (!yyin) {
            perror(argv[1]); // Print system error message if fopen fails
            return 1;
        }
    } else {
        printf("No input file specified. Reading from stdin.\n");
        yyin = stdin; // Default to standard input
    }

    int token;
    printf("Scanning input...\n");
    printf("----------------------------------------------------------\n");
    printf("| Line | Col Start | Token Type   | Value / Text         |\n");
    printf("|------|-----------|--------------|----------------------|\n");

    // yylex() returns 0 on end-of-file
    while ((token = yylex()) != 0) {
        // 'column' is updated by scanner.l to be the column *after* the current token.
        // 'yyleng' is the length of the current token.
        // So, the starting column of the current token is 'column - yyleng'.
        int token_start_column = column - yyleng;

        printf("| %4d | %9d | %-12s | ", lineno, token_start_column, token_to_string(token));

        switch (token) {
            case IDENTIFIER:
                printf("%-20s |\n", yylval.sval);
                free(yylval.sval); // Free memory allocated by strdup in lexer
                break;
            case CHAR: // Your lexer also strdup's CHAR literals
                printf("%-20s |\n", yylval.sval);
                free(yylval.sval); // Free memory allocated by strdup in lexer
                break;
            case NUMBER:
                printf("%-20d |\n", yylval.ival);
                break;
            // For tokens that don't use yylval, yytext contains the matched string
            case PLUS:
            case MINUS:
            case MUL:
            case DIV:
            case LPAREN:
            case RPAREN:
            case LBRACE:
            case RBRACE:
            case SEMI:
            case ASSIGN:
                printf("%-20s |\n", yytext);
                break;
            default:
                // This case handles any tokens not explicitly listed above
                printf("Code: %-14d |\n", token);
                break;
        }
    }

    printf("----------------------------------------------------------\n");
    printf("End of input.\n");

    show_and_free_errors();

    if (yyin != stdin) {
        fclose(yyin);
    }

    return 0;
}

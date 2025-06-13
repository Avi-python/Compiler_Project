#ifndef TOKENS_H
#define TOKENS_H

#define EOF             0
#define error           1
#define IDENTIFIER      2
#define NUMBER          3
#define PLUS            4
#define MINUS           5
#define MUL             6
#define DIV             7
#define ASSIGN          8
#define LPAREN          9
#define RPAREN         10
#define LBRACE         11
#define RBRACE         12
#define SEMI           13
#define INT            14
#define CHAR           15
#define RETURN         16
#define IF             17
#define ELSE           18
#define DO             19
#define WHILE          20
#define CONST          21
#define MAIN           22
#define LT             23
#define LE             24
#define GT             25
#define GE             26
#define EQ             27
#define NE             28
#define COMMA          29
#define VOID           30

typedef union {
    int ival;
    char *sval;
} YYSTYPE;

// Extern declaration for yylval, so scanner_test.c and parser.c can use it
// The actual definition (YYSTYPE yylval;) will be in scanner_test.c and your parser.c
extern YYSTYPE yylval; 

#endif // TOKENS_H
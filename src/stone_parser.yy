%require "3.2"
%language "c++"

%code requires {
    #include <string>
    #include <iostream>
    #include "expression.h"
    #include "stone_scanner.hh"
}

%define api.value.type variant

%parse-param {Scanner &lexer}

%header

%code {
    #define yylex lexer.yylex
}

%token <int> INTEGER
%token <std::string> IDENTIFIER
%token <ASTNode*> ASSIGN
%token <std::string> L_PAREN
%token <std::string> R_PAREN
%token <std::string> ADD
%token <std::string> SUB
%token <std::string> MUL
%token <std::string> DIV

%type <ExpressionNode*> assign
%type <ExpressionNode*> expression
%type <ExpressionNode*> term
%type <ExpressionNode*> factor
%type <IntegerNode*> number

%%

assign
    : IDENTIFIER ASSIGN expression ';' { $$ == new AssignNode($1, $3) }
    ;

expression
    : expression ADD term { $$ = new AddNode($1, $3); }
    | expression SUB term { $$ = new SubNode($1, $3); }
    | term
    ;

term
    : term MUL factor { $$ = new MulNode($1, $3); }
    | term DIV factor { $$ = new DivNode($1, $3); }
    | factor
    ;

factor
    : number { $$ = $1; }
    | IDENTIFIER { $$ = new IdNode($1); }
    | L_PAREN expression R_PAREN { $$ = $2; }
    ;

number
    : INTEGER { $$ = new IntegerNode(std::stoi($1)); }
    ;

%%

void yy::parser::error(const std::string &message)
{
    std::cerr << "Error: " << message << std::endl;
}
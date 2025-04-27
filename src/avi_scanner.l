%option c++
%{
#include "avi_scanner.h"
#include <iostream>
%}

digit [0-9]
integer {digit}+
float {digit}+"."{digit}*
identifier [a-zA-Z][a-zA-Z0-9]*
arith_op [+-/%*]
assign "="

%%

{integer} { 
    std::cout << "integer: " << yytext << std::endl; 
    return INTEGER;
}

{float} {
    std::cout << "float: " << yytext << std::endl; 
    return FLOAT;
}

{identifier} { 
    std::cout << "identifier: " << yytext << std::endl; 
    return IDENTIFIER;
}

{arith_op} {
    switch(yytext[0])
    {
        case '+':
            std::cout << "ADD" << std::endl;
            return ADD;
        case '-':
            std::cout << "SUB" << std::endl;
            return SUB;
        case '*':
            std::cout << "MUL" << std::endl;
            return MUL;
        case '/':
            std::cout << "DIV" << std::endl;
            return DIV;
        case '%':
            std::cout << "MOD" << std::endl;
            return MOD;
    }
}

{assign} {
    std::cout << "ASSIGN" << std::endl;
    return ASSIGN;
}

%%

int yyFlexLexer::yywrap()
{
    std::cout << "end of input" << std::endl;
    return 1;
}

#include <fstream>
int main( int argc, char **argv )
{
   std::ifstream input;
   FlexLexer* lexer;

   ++argv, --argc;
   if ( argc > 0 )
   {
       input.open(argv[0]);
       lexer = new yyFlexLexer( &input, &std::cout );
   }
   else
   {
       lexer = new yyFlexLexer();
   }

   while( lexer->yylex() != 0 );

   return 0;
}
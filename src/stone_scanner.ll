%option c++ noyywrap
%option yyclass="Scanner"

%{
    #include "stone_scanner.hh"
    #include "stone_parser.tab.hh"
    
    #undef  YY_DECL
    #define YY_DECL int Scanner::yylex(yy::parser::semantic_type *yylval)
%}

%%

[[:space:]] ;

"+" { return yy::parser::token::ADD; }
"-" { return yy::parser::token::SUB; }
"*" { return yy::parser::token::MUL; }
"/" { return yy::parser::token::DIV; }
"=" { return yy::parser::token::ASSIGN; }
"(" { return yy::parser::token::L_PAREN; }
")" { return yy::parser::token::R_PAREN; }

[[:alpha:]]+ { 
    lval->emplace<std::string>(yytext, yyleng); 
    return yy::parser::token::IDENTIFIER; 
}

[[:digit:]]+ { 
    lval->emplace<int>(std::stoi(std::string(yytext, yyleng))); 
    return yy::parser::token::INTEGER; 
}

. { return yytext[0]; }



%%
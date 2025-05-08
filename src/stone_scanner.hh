#pragma once

#include <string>
#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

class Scanner;

#include "stone_parser.tab.hh"


class Scanner : public yyFlexLexer
{
public:
    int yylex(yy::parser::semantic_type *yylval);
};


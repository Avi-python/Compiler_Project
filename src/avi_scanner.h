#pragma once
#include <string>

enum Tag { 
    INTEGER, FLOAT, IDENTIFIER, 
    ADD, SUB, MUL, DIV, MOD,
    LT, LE, EQ, NE, ST, SE,
    ASSIGN,
    END
};

class Token 
{
public:
    Tag tag;
    std::string lexeme;

    Token(Tag t, std::string v)
        : tag(t), lexeme(v) {}
};

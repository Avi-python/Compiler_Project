#include "stone_scanner.hh"
#include "stone_parser.tab.hh"

int main()
{
    Scanner lexer;
    yy::parser parser(lexer);
    return parser();
}
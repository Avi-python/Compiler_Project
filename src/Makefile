.RECIPEPREFIX = >

prog: main.o stone_parser.tab.o lex.yy.o
> g++ $^ -o $@

main.o: main.cc stone_scanner.hh stone_parser.tab.hh
> g++ -c $< -o $@

lex.yy.o: lex.yy.cc stone_scanner.hh stone_parser.tab.hh
> g++ -c $< -o $@

stone_parser.tab.o: stone_parser.tab.cc stone_scanner.hh
> g++ -c $< -o $@

lex.yy.cc: stone_scanner.ll
> flex $<

stone_parser.tab.hh stone_parser.tab.cc stone_parser.output: stone_parser.yy
> bison $<

.PHONY: clean
clean:
> rm -f prog main.o lex.* stone_parser.tab.* stack.hh
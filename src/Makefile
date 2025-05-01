# Variables
BISON = bison
FLEX = flex
CC = gcc
BISON_FILE = stone_parser.y
LEX_FILE = stone_scanner.l
BISON_OUTPUT = stone_parser.tab.c
BISON_HEADER = stone_parser.tab.h
LEX_OUTPUT = lex.yy.c
EXEC = compiler

# Targets
all: $(EXEC)

$(BISON_OUTPUT) $(BISON_HEADER): $(BISON_FILE)
	$(BISON) -d $(BISON_FILE)

$(LEX_OUTPUT): $(LEX_FILE) $(BISON_HEADER)
	$(FLEX) $(LEX_FILE)

$(EXEC): $(BISON_OUTPUT) $(LEX_OUTPUT)
	$(CC) -o $(EXEC) $(BISON_OUTPUT) $(LEX_OUTPUT)

clean:
	rm -f $(BISON_OUTPUT) $(BISON_HEADER) $(LEX_OUTPUT) $(EXEC)

.PHONY: all clean
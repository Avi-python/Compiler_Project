# Compiler and tools
CC = gcc
FLEX = flex
BISON = bison
GPERF = gperf

# Compiler flags
CFLAGS = -Wall -g
LDFLAGS = -lfl -lm

# Target executable
TARGET = compiler

# Source files
LEX_L_FILE = scanner.l
PARSER_Y_FILE = parser.y
RESWORDS_GPERF_FILE = reswords.gperf
SOURCE_FILES = visualize.h expression.h error.h reswords.h

# Generated files
LEX_C_FILE = lex.yy.c
PARSER_C_FILE = parser.tab.c
PARSER_H_FILE = parser.tab.h
RESWORDS_H_FILE = reswords.h

# Default target: build the executable
all: $(TARGET)

# The compiler target depends on all generated and source files
$(TARGET): $(LEX_C_FILE) $(PARSER_C_FILE) $(SOURCE_FILES)
	$(CC) $(CFLAGS) -o $@ $(LEX_C_FILE) $(PARSER_C_FILE) $(LDFLAGS)

# Rule to generate parser C and header files from parser.y
$(PARSER_C_FILE) $(PARSER_H_FILE): $(PARSER_Y_FILE)
	$(BISON) -d -o $(PARSER_C_FILE) $<

# Rule to generate scanner C file from scanner.l
# This depends on the parser header, since the scanner includes it
$(LEX_C_FILE): $(LEX_L_FILE) $(RESWORDS_H_FILE) $(PARSER_H_FILE)
	$(FLEX) -o $@ $<

$(RESWORDS_H_FILE): $(PARSER_H_FILE) $(RESWORDS_GPERF_FILE)
	$(GPERF) -S 1 -C -c -P -t -N is_resword $(RESWORDS_GPERF_FILE) | \
	sed 's/register const char \*s = resword->name + stringpool;/register const char *s = stringpool + (int)(size_t)resword->name;/' > $(RESWORDS_H_FILE)

# Target to clean up generated files
clean:
	rm -f $(TARGET) $(LEX_C_FILE) $(PARSER_C_FILE) $(PARSER_H_FILE) *.o *.dot ast.png

# Declare 'all' and 'clean' as phony targets
.PHONY: all clean
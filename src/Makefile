# Compiler and tools
CC = gcc
FLEX = flex
GPERF = gperf

# Directories
INCLUDE_DIR = ../include

# Compiler flags (add -g for debugging, -Wall for warnings, -I for include directory)
CFLAGS = -Wall -g -I$(INCLUDE_DIR)
# Linker flags
LDFLAGS = -lfl

# Target executable
TARGET = compiler

# Source files
LEX_L_FILE = scanner.l
PARSER_FILE = parser.c
AST_FILE = ast.c
SYMBOL_FILE = symbol.c
DRIVER_FILE = driver.c
HASHTABLE_FILE = hashtable.c
SYMBOL_TABLE_FILE = symbol_table.c
RESWORDS_GPERF_FILE = reswords.gperf

# Generated C file from Flex
LEX_C_FILE = lex.yy.c

# Header files - now in include directory
HEADERS = \
	$(INCLUDE_DIR)/tokens.h \
	$(INCLUDE_DIR)/ast.h \
	$(INCLUDE_DIR)/symbol.h \
	$(INCLUDE_DIR)/symbol_table.h \
	$(INCLUDE_DIR)/hashtable.h \
	$(INCLUDE_DIR)/error.h \
	$(INCLUDE_DIR)/parser.h \
	$(INCLUDE_DIR)/hashtable.h \
	$(INCLUDE_DIR)/symbol_table.h

RESWORDS_H_FILE = reswords.h

# Object files
OBJS = \
	$(SYMBOL_TABLE_FILE:.c=.o) \
	$(HASHTABLE_FILE:.c=.o) \
	$(DRIVER_FILE:.c=.o) \
	$(PARSER_FILE:.c=.o) \
	$(AST_FILE:.c=.o) \
	$(SYMBOL_FILE:.c=.o) \
	lex.yy.o

# Default target: build the executable
all: $(TARGET)

# Rule to link the final executable
$(TARGET): $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

# Rule to compile driver.c
$(DRIVER_FILE:.c=.o): $(DRIVER_FILE) $(HEADERS)
	$(CC) $(CFLAGS) -c $(DRIVER_FILE)

# Rule to compile parser.c
$(PARSER_FILE:.c=.o): $(PARSER_FILE) $(LEX_C_FILE) $(HEADERS)
	$(CC) $(CFLAGS) -c $(PARSER_FILE)

# Rule to compile hashtable.h
$(HASHTABLE_FILE:.c=.o): $(HASHTABLE_FILE) $(HEADERS)
	$(CC) $(CFLAGS) -c $(HASHTABLE_FILE)

# Rule to compile symbol_table.c
$(SYMBOL_TABLE_FILE:.c=.o): $(SYMBOL_TABLE_FILE) $(HEADERS)
	$(CC) $(CFLAGS) -c $(SYMBOL_TABLE_FILE)

# Rule to compile ast.c
$(AST_FILE:.c=.o): $(AST_FILE) $(HEADERS)
	$(CC) $(CFLAGS) -c $(AST_FILE)

# Rule to compile symbol.c
$(SYMBOL_FILE:.c=.o): $(SYMBOL_FILE) $(HEADERS)
	$(CC) $(CFLAGS) -c $(SYMBOL_FILE)

# Rule to compile the generated lexer
lex.yy.o: $(LEX_C_FILE) $(HEADERS)
	$(CC) $(CFLAGS) -c $(LEX_C_FILE)

# Rule to generate lex.yy.c from scanner.l
$(LEX_C_FILE): $(LEX_L_FILE) $(RESWORDS_H_FILE) $(HEADERS) 
	$(FLEX) -o $@ $(LEX_L_FILE)

# Rule to generate reswords.h from gperf
$(RESWORDS_H_FILE): $(RESWORDS_GPERF_FILE)
	$(GPERF) -S 1 -C -c -P -t -N is_resword $(RESWORDS_GPERF_FILE) | \
	sed 's/register const char \*s = resword->name + stringpool;/register const char *s = stringpool + (int)(size_t)resword->name;/' > $(RESWORDS_H_FILE)

# Legacy target for scanner_test (keeping for backward compatibility)
scanner_test: scanner_test.c $(LEX_C_FILE) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ scanner_test.c $(LEX_C_FILE) $(LDFLAGS)

# Target to clean up generated files
clean:
	rm -f $(TARGET) scanner_test $(LEX_C_FILE) $(RESWORDS_H_FILE) *.o parser_logs.txt ast_output.dot ast.png

# Target to rebuild everything from scratch
rebuild: clean all

# Declare phony targets
.PHONY: all clean rebuild scanner_test
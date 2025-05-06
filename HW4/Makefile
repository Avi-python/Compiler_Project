# Compiler and tools
CC = gcc
FLEX = flex

# Compiler flags (add -g for debugging, -Wall for warnings)
CFLAGS = -Wall -g
# Linker flags
LDFLAGS = -lfl

# Target executable
TARGET = scanner_test

# Source files
LEX_L_FILE = scanner.l
TEST_C_FILE = scanner_test.c

# Generated C file from Flex
LEX_C_FILE = lex.yy.c

# Header files - list all headers that, if changed, should trigger a rebuild.
# scanner.l includes tokens.h and resword.h
# scanner_test.c includes tokens.h
HEADERS = tokens.h resword.h

# Default target: build the executable
all: $(TARGET)

# Rule to link the final executable
# Depends on the main C file, the generated lexer C file, and relevant headers.
$(TARGET): $(TEST_C_FILE) $(LEX_C_FILE) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(TEST_C_FILE) $(LEX_C_FILE) $(LDFLAGS)

# Rule to generate lex.yy.c from scanner.l
# Depends on the .l file and any headers it includes.
$(LEX_C_FILE): $(LEX_L_FILE) $(HEADERS)
	$(FLEX) -o $@ $(LEX_L_FILE)

# Target to clean up generated files
clean:
	rm -f $(TARGET) $(LEX_C_FILE) *.o

# Declare 'all' and 'clean' as phony targets (targets that don't represent actual files)
.PHONY: all clean
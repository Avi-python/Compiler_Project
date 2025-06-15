# C-Like Language Compiler Project

This project is a compiler for a C-like language, developed as a college final homework assignment. It features a complete lexical analyzer (scanner), recursive descent parser, Abstract Syntax Tree (AST) construction, and symbol table management.

## Components

*   **Lexical Analyzer (Scanner)**: Implemented in [`src/scanner.l`](/home/avibun/Programming/CPP/Compiler_Project/src/scanner.l) using Flex. It tokenizes the input source code according to the language's grammar. Token definitions can be found in [`include/tokens.h`](/home/avibun/Programming/CPP/Compiler_Project/include/tokens.h).

*   **Recursive Descent Parser**: A complete recursive descent parser implemented in [`src/parser.c`](/home/avibun/Programming/CPP/Compiler_Project/src/parser.c) that builds an Abstract Syntax Tree (AST) from the token stream. The parser follows the grammar defined in [`src/c_avi.ebnf`](/home/avibun/Programming/CPP/Compiler_Project/src/c_avi.ebnf).

*   **Abstract Syntax Tree (AST)**: AST node structures and creation functions are defined in [`include/ast.h`](/home/avibun/Programming/CPP/Compiler_Project/include/ast.h) and implemented in [`src/ast.c`](/home/avibun/Programming/CPP/Compiler_Project/src/ast.c). The AST supports visualization through DOT graph generation for debugging purposes.

*   **Symbol Table Management**: Symbol table implementation with hash table backend located in [`include/symbol_table.h`](/home/avibun/Programming/CPP/Compiler_Project/include/symbol_table.h), [`include/hashtable.h`](/home/avibun/Programming/CPP/Compiler_Project/include/hashtable.h), and [`src/symbol.c`](/home/avibun/Programming/CPP/Compiler_Project/src/symbol.c).

*   **Reserved Word Handling**: Uses gperf to efficiently look up reserved words. The gperf input file is [`src/reswords.gperf`](/home/avibun/Programming/CPP/Compiler_Project/src/reswords.gperf), and the generated C code is in `src/reswords.h`.

*   **Error Handling**: Comprehensive error reporting for lexical and syntax errors. Error structures are defined in [`include/error.h`](/home/avibun/Programming/CPP/Compiler_Project/include/error.h) with detailed error recovery mechanisms in the parser.

*   **Language Grammar**: The complete EBNF grammar is defined in [`src/c_avi.ebnf`](/home/avibun/Programming/CPP/Compiler_Project/src/c_avi.ebnf). Additional grammar sketches are available in other EBNF files for reference.

## Building the Compiler

The compiler can be built using the Makefile provided in the `src` directory.

1.  Navigate to the `src` directory:
    ```sh
    cd src
    ```

2.  Build the main compiler:
    ```sh
    make
    ```
    This creates the `compiler` executable which includes the lexer, parser, and AST generator.

3.  Alternatively, build just the scanner test:
    ```sh
    make scanner_test
    ```

4.  Clean build artifacts:
    ```sh
    make clean
    ```

## Running the Compiler

Once built, you can run the compiler with a source file as an argument:

```sh
./compiler path/to/your/sourcefile.c
```

For example, to test with the provided [`src/source.c`](/home/avibun/Programming/CPP/Compiler_Project/src/source.c):

```sh
# From the src directory
./compiler source.c
```

The compiler will:
- Parse the input file and generate an AST
- Output parsing results and any errors to the console
- Generate `ast_output.dot` for AST visualization
- Create `parser_logs.txt` with detailed parsing information

### Visualizing the AST

To generate a visual representation of the AST:

```sh
# Generate PNG image from DOT file (requires Graphviz)
dot -Tpng ast_output.dot -o ast.png
```

### Running Scanner Only

To test just the lexical analyzer:

```sh
./scanner_test source.c
```

## Supported Language Features

The compiler supports a C-like language with the following features:

- **Data Types**: `int`, `char`, `void`
- **Variable Declarations**: Global and local variable declarations with optional initialization
- **Function Definitions**: Functions with parameters and return values
- **Control Structures**: `if`/`else` statements, `while` loops
- **Expressions**: Arithmetic (`+`, `-`, `*`, `/`) and relational operators (`<`, `<=`, `>`, `>=`, `==`, `!=`)
- **Function Calls**: Function calls with arguments
- **Assignment Statements**: Variable assignment
- **Return Statements**: Function return statements

## Project Structure

*   `README.md`: This file.
*   `CMakeLists.txt`: CMake build system file (currently empty).
*   `.gitignore`: Specifies intentionally untracked files that Git should ignore.
*   `include/`: Header files for all components:
    *   `tokens.h`: Token type definitions
    *   `ast.h`: AST node structures and function declarations
    *   `symbol.h`, `symbol_table.h`, `hashtable.h`: Symbol table management
    *   `error.h`: Error handling structures
*   `src/`: Contains all source code:
    *   `Makefile`: Build system for the compiler
    *   `scanner.l`: Flex input file for the lexical analyzer
    *   `parser.c`: Recursive descent parser implementation
    *   `ast.c`: AST creation and manipulation functions
    *   `symbol.c`: Symbol table implementation
    *   `scanner_test.c`: Test program for the scanner
    *   `c_avi.ebnf`: Complete EBNF grammar specification
    *   `source.c`: Example source file for testing
    *   `reswords.gperf`: gperf input for reserved words
*   `test/`: Test files with various error cases:
    *   `error_1.c`, `error_2.c`, `error_3.c`: Test cases for error handling
*   `build/`: Directory for build outputs (CMake, currently unused by Makefile)

## Development Status

- ✅ **Lexical Analysis**: Complete with comprehensive error reporting
- ✅ **Syntax Analysis**: Recursive descent parser with error recovery
- ✅ **AST Construction**: Full AST generation with visualization support
- ✅ **Symbol Table**: Hash table-based symbol management
- 🚧 **Semantic Analysis**: Planned for future development
- 🚧 **Code Generation**: Planned for future development

## Testing

The project includes several test files in the `test/` directory to verify error handling capabilities. You can test the compiler with these files to see how it handles various syntax errors and edge cases.

```sh
# Test error handling
./compiler test/error_1.c
./compiler test/error_2.c
./compiler test/error_3.c
```
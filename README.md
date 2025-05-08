# C-Like Language Compiler Project

This project is a compiler for a C-like language, developed as a college final homework assignment. It currently features a lexical analyzer (scanner) built with Flex and a system for handling reserved words using gperf.

## Components
`
*   **Lexical Analyzer (Scanner)**: Implemented in [`src/scanner.l`](/home/avibun/Programming/CPP/Compiler_Project/src/scanner.l) using Flex. It tokenizes the input source code according to the language's grammar. Token definitions can be found in [`src/tokens.h`](/home/avibun/Programming/CPP/Compiler_Project/src/tokens.h).
*   **Reserved Word Handling**: Uses gperf to efficiently look up reserved words. The gperf input file is [`src/reswords.gperf`](/home/avibun/Programming/CPP/Compiler_Project/src/reswords.gperf), and the generated C code is in [`src/reswords.h`](/home/avibun/Programming/CPP/Compiler_Project/src/reswords.h).
*   **Error Handling**: Basic error reporting for unrecognized tokens is implemented in the scanner. Error structures are defined in [`src/error.h`](/home/avibun/Programming/CPP/Compiler_Project/src/error.h).
*   **Parser**: A placeholder for the parser exists in [`src/parser.c`](/home/avibun/Programming/CPP/Compiler_Project/src/parser.c).
*   **Language Grammar Sketches**: Several EBNF files ([`src/c_avi.ebnf`](/home/avibun/Programming/CPP/Compiler_Project/src/c_avi.ebnf), [`src/c_chubek.ebnf`](/home/avibun/Programming/CPP/Compiler_Project/src/c_chubek.ebnf), [`src/c_sketch.ebnf`](/home/avibun/Programming/CPP/Compiler_Project/src/c_sketch.ebnf), [`src/plone.ebnf`](/home/avibun/Programming/CPP/Compiler_Project/src/plone.ebnf)) outline different aspects or versions of the language grammar.

## Building the Scanner

The scanner and its test program can be built using the Makefile provided in the `src` directory.

1.  Navigate to the `src` directory:
    ```sh
    cd src
    ```
2.  Run make:
    ```sh
    make scanner_test
    ```
    This will compile `scanner.l` and `scanner_test.c` to create an executable `scanner_test`.

## Running the Scanner Test

Once built, you can run the `scanner_test` executable with a source file as an argument:

```sh
./scanner_test path/to/your/sourcefile.c
```

For example, to test with the provided [`src/source.c`](/home/avibun/Programming/CPP/Compiler_Project/src/source.c):

```sh
# From the src directory
./scanner_test source.c
```

The program will output the list of tokens found in the input file.

## Project Structure

*   `.gitignore`: Specifies intentionally untracked files that Git should ignore.
*   `CMakeLists.txt`: CMake build system file (currently empty).
*   `README.md`: This file.
*   `.vscode/`: VS Code editor configuration files ([`launch.json`](/home/avibun/Programming/CPP/Compiler_Project/.vscode/launch.json), [`tasks.json`](/home/avibun/Programming/CPP/Compiler_Project/.vscode/tasks.json)).
*   `build/`: Directory for build outputs (intended for CMake, currently unused by Makefile).
*   `include/`: Directory for header files (currently empty, headers are in `src/`).
*   `src/`: Contains all source code, including the lexer, parser, EBNF files, and Makefiles.
    *   `Makefile`: Used to build the scanner.
    *   `scanner.l`: Flex input file for the lexical analyzer.
    *   `scanner_test.c`: Test program for the scanner.
    *   `tokens.h`: Defines token types.
    *   `reswords.gperf`: gperf input for reserved words.
    *   `reswords.h`: Generated C code for reserved words.
    *   `error.h`: Defines error structures.
    *   `parser.c`: Placeholder for the parser.
    *   `*.ebnf`: EBNF grammar files.
    *   `source.c`: An example source file for testing.
```<!-- filepath: /home/avibun/Programming/CPP/Compiler_Project/README.md -->
# C-Like Language Compiler Project

This project is a compiler for a C-like language, developed as a college final homework assignment. It currently features a lexical analyzer (scanner) built with Flex and a system for handling reserved words using gperf.

## Components

*   **Lexical Analyzer (Scanner)**: Implemented in [`src/scanner.l`](/home/avibun/Programming/CPP/Compiler_Project/src/scanner.l) using Flex. It tokenizes the input source code according to the language's grammar. Token definitions can be found in [`src/tokens.h`](/home/avibun/Programming/CPP/Compiler_Project/src/tokens.h).
*   **Reserved Word Handling**: Uses gperf to efficiently look up reserved words. The gperf input file is [`src/reswords.gperf`](/home/avibun/Programming/CPP/Compiler_Project/src/reswords.gperf), and the generated C code is in [`src/reswords.h`](/home/avibun/Programming/CPP/Compiler_Project/src/reswords.h).
*   **Error Handling**: Basic error reporting for unrecognized tokens is implemented in the scanner. Error structures are defined in [`src/error.h`](/home/avibun/Programming/CPP/Compiler_Project/src/error.h).
*   **Parser**: A placeholder for the parser exists in [`src/parser.c`](/home/avibun/Programming/CPP/Compiler_Project/src/parser.c).
*   **Language Grammar Sketches**: Several EBNF files ([`src/c_avi.ebnf`](/home/avibun/Programming/CPP/Compiler_Project/src/c_avi.ebnf), [`src/c_chubek.ebnf`](/home/avibun/Programming/CPP/Compiler_Project/src/c_chubek.ebnf), [`src/c_sketch.ebnf`](/home/avibun/Programming/CPP/Compiler_Project/src/c_sketch.ebnf), [`src/plone.ebnf`](/home/avibun/Programming/CPP/Compiler_Project/src/plone.ebnf)) outline different aspects or versions of the language grammar.

## Building the Scanner

The scanner and its test program can be built using the Makefile provided in the `src` directory.

1.  Navigate to the `src` directory:
    ```sh
    cd src
    ```
2.  Run make:
    ```sh
    make scanner_test
    ```
    This will compile `scanner.l` and `scanner_test.c` to create an executable `scanner_test`.

## Running the Scanner Test

Once built, you can run the `scanner_test` executable with a source file as an argument:

```sh
./scanner_test path/to/your/sourcefile.c
```

For example, to test with the provided [`src/source.c`](/home/avibun/Programming/CPP/Compiler_Project/src/source.c):

```sh
# From the src directory
./scanner_test source.c
```

The program will output the list of tokens found in the input file.

## Project Structure

*   `.gitignore`: Specifies intentionally untracked files that Git should ignore.
*   `CMakeLists.txt`: CMake build system file (currently empty).
*   `README.md`: This file.
*   `.vscode/`: VS Code editor configuration files ([`launch.json`](/home/avibun/Programming/CPP/Compiler_Project/.vscode/launch.json), [`tasks.json`](/home/avibun/Programming/CPP/Compiler_Project/.vscode/tasks.json)).
*   `build/`: Directory for build outputs (intended for CMake, currently unused by Makefile).
*   `include/`: Directory for header files (currently empty, headers are in `src/`).
*   `src/`: Contains all source code, including the lexer, parser, EBNF files, and Makefiles.
    *   `Makefile`: Used to build the scanner.
    *   `scanner.l`: Flex input file for the lexical analyzer.
    *   `scanner_test.c`: Test program for the scanner.
    *   `tokens.h`: Defines token types.
    *   `reswords.gperf`: gperf input for reserved words.
    *   `reswords.h`: Generated C code for reserved words.
    *   `error.h`: Defines error structures.
    *   `parser.c`: Placeholder for the parser.
    *   `*.ebnf`: EBNF grammar files.
    *   `source.c`: An example source file for testing.
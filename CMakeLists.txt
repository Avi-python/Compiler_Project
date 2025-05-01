cmake_minimum_required(VERSION 3.10)
project(CompilerProject C)

# Find Flex and Bison
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Source directory
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Generate parser from Bison
BISON_TARGET(Parser 
    ${SRC_DIR}/stone_parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/stone_parser.tab.c
    COMPILE_FLAGS "-d"
)

# Generate scanner from Flex
FLEX_TARGET(Scanner 
    ${SRC_DIR}/stone_scanner.l
    ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.h
)

# Add dependencies
ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

# Include directories
include_directories(${SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# Add executable
add_executable(compiler 
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Scanner_OUTPUTS}
)
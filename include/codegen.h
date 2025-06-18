#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "symbol_table.h"
#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

// Function to initialize LLVM components and the code generator
void codegen_init(const char* output_filename);

// Function to generate LLVM IR from the AST
// Returns 0 on success, non-zero on error
int codegen_generate(ASTNode* ast_root, sym_t* global_symbol_table);

// Function to clean up LLVM components
void codegen_dispose();

// Function to transform LLVM IR to object file
void codegen_to_object();

// Function to print the generated LLVM IR to a file
void codegen_print_ir();

#endif // CODEGEN_H

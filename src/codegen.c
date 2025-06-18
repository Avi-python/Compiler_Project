#include "codegen.h"
#include "ast.h" // For AST node types
#include "symbol_table.h" // For symbol table access if needed
#include "tokens.h" // For token types if needed for literals/ops
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <llvm-c/Analysis.h>

// LLVM Global Variables
static LLVMContextRef llvm_context;
static LLVMModuleRef llvm_module;
static LLVMBuilderRef llvm_builder;
static char* output_ir_filename = NULL;

// Improved scope-aware symbol table for proper block scoping
#define MAX_SCOPE_DEPTH 32
#define MAX_VARIABLES_PER_SCOPE 256

typedef struct {
    char* name;
    LLVMValueRef llvm_value; // AllocaInst for locals/params
    int scope_depth;         // Track which scope this variable belongs to
} ScopedVariable;

typedef struct {
    ScopedVariable variables[MAX_VARIABLES_PER_SCOPE];
    int variable_count;
    int scope_depth;
    LLVMBasicBlockRef entry_point; // Where allocas for this scope should go
} ScopeFrame;

static ScopeFrame scope_stack[MAX_SCOPE_DEPTH];
static int current_scope_depth = -1; // -1 means no active scope
static LLVMValueRef current_function_ref = NULL;

// Scope management functions
static void push_scope(LLVMBasicBlockRef entry_point) 
{
    if (current_scope_depth >= MAX_SCOPE_DEPTH - 1) {
        fprintf(stderr, "CodeGen Error: Maximum scope depth exceeded.\n");
        return;
    }
    
    current_scope_depth++;
    scope_stack[current_scope_depth].variable_count = 0;
    scope_stack[current_scope_depth].scope_depth = current_scope_depth;
    scope_stack[current_scope_depth].entry_point = entry_point;
    
    printf("CodeGen Debug: Pushed scope depth %d\n", current_scope_depth);
}

static void pop_scope() 
{
    if (current_scope_depth < 0) {
        fprintf(stderr, "CodeGen Error: Cannot pop scope - no active scope.\n");
        return;
    }
    
    // Free variable names in this scope
    ScopeFrame* current_frame = &scope_stack[current_scope_depth];
    for (int i = 0; i < current_frame->variable_count; i++) {
        free(current_frame->variables[i].name);
        current_frame->variables[i].name = NULL;
    }
    
    printf("CodeGen Debug: Popped scope depth %d (had %d variables)\n", 
           current_scope_depth, current_frame->variable_count);
    
    current_scope_depth--;
}

static void clear_all_scopes() 
{
    while (current_scope_depth >= 0) {
        pop_scope();
    }
}

static void add_scoped_variable(const char* name, LLVMValueRef val) 
{
    if (current_scope_depth < 0) {
        fprintf(stderr, "CodeGen Error: Cannot add variable - no active scope.\n");
        return;
    }
    
    ScopeFrame* current_frame = &scope_stack[current_scope_depth];
    if (current_frame->variable_count >= MAX_VARIABLES_PER_SCOPE) {
        fprintf(stderr, "CodeGen Error: Too many variables in current scope.\n");
        return;
    }
    
    ScopedVariable* var = &current_frame->variables[current_frame->variable_count];
    var->name = strdup(name);
    var->llvm_value = val;
    var->scope_depth = current_scope_depth;
    current_frame->variable_count++;
    
    printf("CodeGen Debug: Added variable '%s' to scope depth %d\n", name, current_scope_depth);
}

static LLVMValueRef get_scoped_variable(const char* name) 
{
    // Search from current scope back to function scope (but not global)
    for (int depth = current_scope_depth; depth >= 0; depth--) {
        ScopeFrame* frame = &scope_stack[depth];
        for (int i = frame->variable_count - 1; i >= 0; i--) {
            if (strcmp(frame->variables[i].name, name) == 0) {
                printf("CodeGen Debug: Found variable '%s' in scope depth %d\n", name, depth);
                return frame->variables[i].llvm_value;
            }
        }
    }
    
    // Check global variables if not found locally
    LLVMValueRef global_var = LLVMGetNamedGlobal(llvm_module, name);
    if (global_var) {
        printf("CodeGen Debug: Found global variable '%s'\n", name);
        return global_var;
    }
    
    fprintf(stderr, "CodeGen Error: Undefined variable %s\n", name);
    return NULL;
}

// Helper to create an Alloca instruction at the current scope's entry point
static LLVMValueRef create_scoped_alloca(LLVMTypeRef type, const char *name) 
{
    if (!current_function_ref) {
        fprintf(stderr, "CodeGen Error: Cannot create alloca, no current function context.\n");
        return NULL;
    }
    
    if (current_scope_depth < 0) {
        fprintf(stderr, "CodeGen Error: Cannot create alloca, no active scope.\n");
        return NULL;
    }
    
    // For function-level scope (depth 0), use entry block
    // For nested scopes, we'll still use entry block but track scope for variable resolution
    LLVMBasicBlockRef entry_block = LLVMGetEntryBasicBlock(current_function_ref);
    LLVMBuilderRef temp_builder = LLVMCreateBuilderInContext(llvm_context);
    LLVMPositionBuilder(temp_builder, entry_block, LLVMGetFirstInstruction(entry_block));
    
    // Create a unique name for the alloca to avoid conflicts
    char unique_name[256];
    snprintf(unique_name, sizeof(unique_name), "%s.%d", name, current_scope_depth);
    
    LLVMValueRef alloca_inst = LLVMBuildAlloca(temp_builder, type, unique_name);
    LLVMDisposeBuilder(temp_builder);
    
    printf("CodeGen Debug: Created alloca for '%s' (unique name: '%s') in scope depth %d\n", 
           name, unique_name, current_scope_depth);
    
    return alloca_inst;
}


// Forward declarations for static helper functions
static LLVMValueRef generate_expression(ASTNode* expr_node);
static LLVMValueRef generate_statement(ASTNode* stmt_node);
static LLVMValueRef generate_node(ASTNode* node);
static int current_block_needs_terminator();


void codegen_init(const char* output_filename) 
{
    llvm_context = LLVMContextCreate();
    llvm_module = LLVMModuleCreateWithNameInContext("my_compiler_module", llvm_context);
    llvm_builder = LLVMCreateBuilderInContext(llvm_context);

    if (output_filename) 
    {
        output_ir_filename = strdup(output_filename);
    } 
    else 
    {
        output_ir_filename = strdup("output.ll");
    }

    printf("LLVM Initialized. Output will be written to %s\n", output_ir_filename);
}

int codegen_generate(ASTNode* ast_root, sym_t* global_symbol_table) 
{
    if (!ast_root) 
    {
        fprintf(stderr, "AST root is NULL, cannot generate code.\n");
        return 1;
    }

    // Traverse the AST and generate code
    // This is a simplified traversal, you'll need to handle all node types
    ASTNode* current_node = ast_root;
    if (current_node->type == NODE_PROGRAM) 
    {
        current_node = ((ProgramNode*)current_node)->start;
    }

    while (current_node != NULL) 
    {
        generate_node(current_node);
        current_node = current_node->next; // Assuming ProgramNode's children are linked via 'next'
    }
    
    // Verify the module for errors
    char *error = NULL;
    if (LLVMVerifyModule(llvm_module, LLVMPrintMessageAction, &error)) 
    {
        fprintf(stderr, "LLVM module verification failed:\n%s\n", error ? error : "Unknown error");
        if (error) LLVMDisposeMessage(error);
        return 1;
    }
    if (error) LLVMDisposeMessage(error); // dispose even if no error

    printf("LLVM IR generation complete.\n");
    return 0;
}

void codegen_to_object()
{
    LLVMInitializeX86TargetInfo();
    LLVMInitializeX86Target();
    LLVMInitializeX86TargetMC();
    LLVMInitializeX86AsmPrinter();
    LLVMInitializeX86AsmParser();


    // 步驟 2: 獲取目標資訊並建立 TargetMachine
    char* targetTriple = LLVMGetDefaultTargetTriple(); // 獲取本機的目標三元組
    LLVMTargetRef target;
    char* error = NULL;

    // 根據三元組尋找目標
    if (LLVMGetTargetFromTriple(targetTriple, &target, &error)) {
        fprintf(stderr, "Failed to get target from triple: %s\n", error);
        LLVMDisposeMessage(error);
        LLVMDisposeMessage(targetTriple);
        return;
    }

    const char* cpu = "generic";
    const char* features = "";
    LLVMTargetMachineRef machine = LLVMCreateTargetMachine(
        target,
        targetTriple,
        cpu,
        features,
        LLVMCodeGenLevelDefault,
        LLVMRelocDefault,
        LLVMCodeModelDefault
    );


    // Step 2: Determine output filename based on the target OS
#if defined(_WIN32) || defined(_WIN64)
    const char* filename = "output.obj";
#else
    const char* filename = "output.o";
#endif

    char* errorMessage = NULL;

    // Step 3: Emit the object file
    if (LLVMTargetMachineEmitToFile(machine, llvm_module, (char*)filename, LLVMObjectFile, &errorMessage)) 
    {
        fprintf(stderr, "Failed to emit object file: %s\n", errorMessage);
        LLVMDisposeMessage(errorMessage);
        LLVMDisposeTargetMachine(machine);
        LLVMDisposeMessage(targetTriple);
        return;
    }

    printf("Successfully compiled module to %s\n", filename);

    // Step 4: Clean up resources
    LLVMDisposeTargetMachine(machine);
    LLVMDisposeMessage(targetTriple);
}

void codegen_to_assembly()
{
    LLVMInitializeX86TargetInfo();
    LLVMInitializeX86Target();
    LLVMInitializeX86TargetMC();
    LLVMInitializeX86AsmPrinter();
    LLVMInitializeX86AsmParser();

    // Get target information and create TargetMachine
    char* targetTriple = LLVMGetDefaultTargetTriple();
    LLVMTargetRef target;
    char* error = NULL;

    // Find target from triple
    if (LLVMGetTargetFromTriple(targetTriple, &target, &error)) {
        fprintf(stderr, "Failed to get target from triple: %s\n", error);
        LLVMDisposeMessage(error);
        LLVMDisposeMessage(targetTriple);
        return;
    }

    const char* cpu = "generic";
    const char* features = "";
    LLVMTargetMachineRef machine = LLVMCreateTargetMachine(
        target,
        targetTriple,
        cpu,
        features,
        LLVMCodeGenLevelDefault,
        LLVMRelocDefault,
        LLVMCodeModelDefault
    );

    // Determine assembly output filename
    const char* filename = "output.s";

    char* errorMessage = NULL;

    // Emit assembly file
    if (LLVMTargetMachineEmitToFile(machine, llvm_module, (char*)filename, LLVMAssemblyFile, &errorMessage)) 
    {
        fprintf(stderr, "Failed to emit assembly file: %s\n", errorMessage);
        LLVMDisposeMessage(errorMessage);
        LLVMDisposeTargetMachine(machine);
        LLVMDisposeMessage(targetTriple);
        return;
    }

    printf("Successfully compiled module to assembly: %s\n", filename);

    // Clean up resources
    LLVMDisposeTargetMachine(machine);
    LLVMDisposeMessage(targetTriple);
}

void codegen_print_ir() 
{
    if (LLVMPrintModuleToFile(llvm_module, output_ir_filename, NULL)) 
    {
        fprintf(stderr, "Error printing LLVM IR to file %s\n", output_ir_filename);
    } 
    else 
    {
        printf("LLVM IR written to %s\n", output_ir_filename);
    }
}

void codegen_dispose() 
{
    if (llvm_builder) LLVMDisposeBuilder(llvm_builder);
    if (llvm_module) LLVMDisposeModule(llvm_module);
    if (llvm_context) LLVMContextDispose(llvm_context);
    if (output_ir_filename) free(output_ir_filename);

    llvm_builder = NULL;
    llvm_module = NULL;
    llvm_context = NULL;
    output_ir_filename = NULL;
    clear_all_scopes(); // Clear any remaining scopes
    printf("LLVM Disposed.\n");
}

// --- Helper functions to generate LLVM IR for different AST nodes ---

static LLVMTypeRef get_llvm_type(TypeNode* type_node) 
{
    if (!type_node) return LLVMVoidTypeInContext(llvm_context); // Default or error

    switch (type_node->type) 
    {
        case INT:
            return LLVMInt32TypeInContext(llvm_context);
        case CHAR:
            return LLVMInt8TypeInContext(llvm_context);
        case VOID:
            return LLVMVoidTypeInContext(llvm_context);
        default:
            fprintf(stderr, "CodeGen Error: Unknown type specified.\n");
            return LLVMVoidTypeInContext(llvm_context); // Fallback
    }
}

// Helper function to get the element type from a global variable (LLVM-14 compatible)
static LLVMTypeRef get_global_variable_type(LLVMValueRef global_var) 
{
    if (!global_var) return NULL;
    
    // In LLVM-14, we need to use LLVMGetElementType with the global's type
    LLVMTypeRef global_ptr_type = LLVMTypeOf(global_var);
    return LLVMGetElementType(global_ptr_type);
}

// Helper function to get the allocated type from an alloca (LLVM-14 compatible)
static LLVMTypeRef get_alloca_type(LLVMValueRef alloca_inst) 
{
    if (!alloca_inst) return NULL;
    
    // In LLVM-14, we need to use LLVMGetElementType with the alloca's type
    LLVMTypeRef alloca_ptr_type = LLVMTypeOf(alloca_inst);
    return LLVMGetElementType(alloca_ptr_type);
}

// Helper function to get function type from function value (LLVM-14 compatible)
static LLVMTypeRef get_function_type(LLVMValueRef func) 
{
    if (!func) return NULL;
    
    // In LLVM-14, we need to use LLVMGetElementType with the function's type
    LLVMTypeRef func_ptr_type = LLVMTypeOf(func);
    return LLVMGetElementType(func_ptr_type);
}

static LLVMValueRef generate_node(ASTNode* node) 
{
    if (!node) return NULL;

    switch (node->type) 
    {
        case NODE_FUNCTION_DEFINITION: 
        {
            FunctionDefinitionNode* func_def_node = (FunctionDefinitionNode*)node;
            IdentifierNode* func_name_node = (IdentifierNode*)func_def_node->name_identifier;
            TypeNode* return_type_node = (TypeNode*)func_def_node->type;

            clear_all_scopes(); // Prepare for new function scope

            LLVMTypeRef llvm_return_type = get_llvm_type(return_type_node);

            // Count parameters and get their types
            int num_params = 0;
            ASTNode* p_count_node = func_def_node->params;
            while(p_count_node) 
            {
                num_params++;
                p_count_node = p_count_node->next;
            }
            
            LLVMTypeRef* llvm_param_types = malloc(sizeof(LLVMTypeRef) * num_params);
            ASTNode* current_param_ast = func_def_node->params;
            for(int i = 0; i < num_params; ++i) 
            {
                ParameterNode* param_node = (ParameterNode*)current_param_ast;
                llvm_param_types[i] = get_llvm_type((TypeNode*)param_node->type);
                current_param_ast = current_param_ast->next;
            }

            LLVMTypeRef func_type = LLVMFunctionType(llvm_return_type, llvm_param_types, num_params, 0); // 0 for not variadic
            LLVMValueRef func = LLVMAddFunction(llvm_module, func_name_node->symbol->name, func_type);
            current_function_ref = func; // Set current function context

            LLVMBasicBlockRef entry_block = LLVMAppendBasicBlockInContext(llvm_context, func, "entry");
            LLVMPositionBuilderAtEnd(llvm_builder, entry_block);

            // Push function scope (depth 0)
            push_scope(entry_block);

            // Store parameters in allocas and add to local symbol table
            current_param_ast = func_def_node->params;
            for (int i = 0; i < num_params; ++i) 
            {
                ParameterNode* param_node = (ParameterNode*)current_param_ast;
                IdentifierNode* param_name_node = (IdentifierNode*)param_node->name_identifier;
                LLVMValueRef llvm_param = LLVMGetParam(func, i);
                LLVMSetValueName(llvm_param, param_name_node->symbol->name);

                LLVMValueRef param_alloca = create_scoped_alloca(llvm_param_types[i], param_name_node->symbol->name);
                LLVMBuildStore(llvm_builder, llvm_param, param_alloca);
                add_scoped_variable(param_name_node->symbol->name, param_alloca);
                current_param_ast = current_param_ast->next;
            }
            free(llvm_param_types);

            if (func_def_node->body) generate_node(func_def_node->body);

            // Pop function scope
            pop_scope();

            // Add implicit return for void functions if last block doesn't have a terminator
            if (LLVMGetTypeKind(llvm_return_type) == LLVMVoidTypeKind) 
            {
                if (LLVMGetBasicBlockTerminator(LLVMGetLastBasicBlock(func)) == NULL) 
                {
                    LLVMBuildRetVoid(llvm_builder);
                }
            }
            else
            {
                // Ensure non-void functions end with a return. 
                // Check all basic blocks to see if any lack terminators
                int has_unterminated_block = 0;
                LLVMBasicBlockRef current_bb = LLVMGetFirstBasicBlock(func);
                
                while (current_bb != NULL) 
                {
                    if (LLVMGetBasicBlockTerminator(current_bb) == NULL) 
                    {
                        has_unterminated_block = 1;
                        // Position builder at the end of this unterminated block
                        LLVMPositionBuilderAtEnd(llvm_builder, current_bb);
                        
                        // Add a default return value based on the return type
                        LLVMValueRef default_return_val;
                        if (LLVMGetTypeKind(llvm_return_type) == LLVMIntegerTypeKind) 
                        {
                            // Return 0 for integer types
                            default_return_val = LLVMConstInt(llvm_return_type, 0, 0);
                        }
                        else if (LLVMGetTypeKind(llvm_return_type) == LLVMPointerTypeKind) 
                        {
                            // Return null for pointer types
                            default_return_val = LLVMConstNull(llvm_return_type);
                        }
                        else 
                        {
                            // For other types, try to use null
                            default_return_val = LLVMConstNull(llvm_return_type);
                        }
                        
                        LLVMBuildRet(llvm_builder, default_return_val);
                        
                        fprintf(stderr, "CodeGen Warning: Added implicit return to non-void function %s at unterminated basic block.\n", 
                                func_name_node->symbol->name);
                    }
                    current_bb = LLVMGetNextBasicBlock(current_bb);
                }
                
                if (!has_unterminated_block) 
                {
                    printf("CodeGen Info: Non-void function %s has proper return statements in all paths.\n", 
                           func_name_node->symbol->name);
                }
            }
            current_function_ref = NULL; // Clear current function context
            return func;
        }
        case NODE_COMPOUND_STATEMENT: 
        {
            CompoundStatementNode* compound_node = (CompoundStatementNode*)node;
            
            // Push new scope for this compound statement
            // Use current basic block as entry point for allocas
            LLVMBasicBlockRef current_block = LLVMGetInsertBlock(llvm_builder);
            push_scope(current_block);
            
            ASTNode* current_stmt = compound_node->start;
            while (current_stmt) 
            {
                generate_statement(current_stmt);
                current_stmt = current_stmt->next;
            }
            
            // Pop scope when exiting compound statement
            pop_scope();
            return NULL;
        }
        case NODE_RETURN_STATEMENT:
        {
            ReturnStatementNode* ret_node = (ReturnStatementNode*)node;
            if (ret_node->expression) 
            {
                LLVMValueRef ret_val = generate_expression(ret_node->expression);
                if (ret_val) LLVMBuildRet(llvm_builder, ret_val);
            }
            else 
            {
                LLVMBuildRetVoid(llvm_builder);
            }
            return NULL; // Return is a control flow instruction
        }
        case NODE_PRINT_STATEMENT:
        {
            PrintStatementNode* print_node = (PrintStatementNode*)node;
            if (print_node->expression) 
            {
                LLVMValueRef expr_val = generate_expression(print_node->expression);
                if (expr_val) 
                {
                    // Call printf to print the value
                    // First, get or declare printf function
                    LLVMValueRef printf_func = LLVMGetNamedFunction(llvm_module, "printf");
                    if (!printf_func) 
                    {
                        // Declare printf function: int printf(char* format, ...)
                        LLVMTypeRef printf_type = LLVMFunctionType(
                            LLVMInt32TypeInContext(llvm_context), // return type: int
                            (LLVMTypeRef[]){LLVMPointerType(LLVMInt8TypeInContext(llvm_context), 0)}, // char*
                            1, // number of fixed parameters 
                            1  // is variadic
                        );
                        printf_func = LLVMAddFunction(llvm_module, "printf", printf_type);
                    }
                    
                    // Create format string "%d\n" for integers
                    LLVMValueRef format_str = LLVMBuildGlobalStringPtr(llvm_builder, "%d\n", "fmt");
                    
                    // Call printf with format string and value
                    LLVMValueRef printf_args[] = {format_str, expr_val};
                    LLVMBuildCall2(llvm_builder, LLVMGlobalGetValueType(printf_func), printf_func, printf_args, 2, "");
                }
            }
            return NULL;
        }
        case NODE_NUMBER_LITERAL: 
        {
            NumberNode* num_node = (NumberNode*)node;
            return LLVMConstInt(LLVMInt32TypeInContext(llvm_context), num_node->value, 0); // 0 for not sign-extended
        }
        case NODE_LOCAL_VARIABLE_DECLARATION: 
        {
            LocalVariableDeclarationNode* local_var_decl_node = (LocalVariableDeclarationNode*)node;
            TypeNode* type_node = (TypeNode*)local_var_decl_node->type;
            LLVMTypeRef llvm_type = get_llvm_type(type_node);

            ASTNode* current_declarator_ast = local_var_decl_node->start;
            while (current_declarator_ast) 
            {
                VariableDeclaratorNode* decl_node = (VariableDeclaratorNode*)current_declarator_ast;
                IdentifierNode* var_name_node = (IdentifierNode*)decl_node->identifier;

                LLVMValueRef var_alloca = create_scoped_alloca(llvm_type, var_name_node->symbol->name);
                add_scoped_variable(var_name_node->symbol->name, var_alloca);

                if (decl_node->expression) 
                {
                    LLVMValueRef init_val = generate_expression(decl_node->expression);
                    if (init_val) LLVMBuildStore(llvm_builder, init_val, var_alloca);
                }
                current_declarator_ast = current_declarator_ast->next;
            }
            return NULL;
        }
        case NODE_IDENTIFIER: 
        { // Used in an expression (RHS)
            IdentifierNode* id_node = (IdentifierNode*)node;
            LLVMValueRef var_ref = get_scoped_variable(id_node->symbol->name);
            if (var_ref && LLVMGetInstructionOpcode(var_ref) == LLVMAlloca) 
            {
                // Local variable - load from alloca
                return LLVMBuildLoad2(llvm_builder, LLVMGetAllocatedType(var_ref), var_ref, id_node->symbol->name);
            } 

            // Global variable - load from global
            LLVMTypeRef global_type = LLVMGetElementType(LLVMGetNamedGlobal(llvm_module, id_node->symbol->name));
            if(global_type) 
            {
                return LLVMBuildLoad2(llvm_builder, global_type, LLVMGetNamedGlobal(llvm_module, id_node->symbol->name), id_node->symbol->name);
            }

            fprintf(stderr, "CodeGen Error: Identifier %s not found or not loadable.\n", id_node->symbol->name);
            return NULL;
        }
        case NODE_ASSIGNMENT_STATEMENT: 
        {
            AssignmentStatementNode* assign_node = (AssignmentStatementNode*)node;
            IdentifierNode* id_node = (IdentifierNode*)assign_node->identifier;
            
            LLVMValueRef var_ref = get_scoped_variable(id_node->symbol->name);
            if (!var_ref) 
            {
                fprintf(stderr, "CodeGen Error: Cannot assign to undeclared variable %s\n", id_node->symbol->name);
                return NULL;
            }

            LLVMValueRef expr_val = generate_expression(assign_node->expression);
            if (expr_val) LLVMBuildStore(llvm_builder, expr_val, var_ref);
            return expr_val; // Assignment can be an expression in C
        }
        case NODE_BINARY_EXPRESSION: 
        {
            BinaryExpressionNode* bin_expr_node = (BinaryExpressionNode*)node;
            LLVMValueRef lhs = generate_expression(bin_expr_node->left);
            LLVMValueRef rhs = generate_expression(bin_expr_node->right);

            if (!lhs || !rhs) return NULL;

            // Assuming bin_expr_node->op maps to token types like T_PLUS, T_MINUS, T_EQ etc.
            // These need to match your actual token definitions.
            // Example token values (replace with your actual values from tokens.h)

            switch (bin_expr_node->op) {
                case PLUS: return LLVMBuildAdd(llvm_builder, lhs, rhs, "addtmp");
                case MINUS: return LLVMBuildSub(llvm_builder, lhs, rhs, "subtmp");
                case MUL: return LLVMBuildMul(llvm_builder, lhs, rhs, "multmp");
                case DIV: return LLVMBuildSDiv(llvm_builder, lhs, rhs, "divtmp"); // Assuming signed division
                // Comparison operators
                case EQ: return LLVMBuildICmp(llvm_builder, LLVMIntEQ, lhs, rhs, "eqtmp");
                case NE: return LLVMBuildICmp(llvm_builder, LLVMIntNE, lhs, rhs, "neqtmp");
                case LT: return LLVMBuildICmp(llvm_builder, LLVMIntSLT, lhs, rhs, "lttmp"); // Signed Less Than
                case LE: return LLVMBuildICmp(llvm_builder, LLVMIntSLE, lhs, rhs, "ltetmp");
                case GT: return LLVMBuildICmp(llvm_builder, LLVMIntSGT, lhs, rhs, "gttmp");
                case GE: return LLVMBuildICmp(llvm_builder, LLVMIntSGE, lhs, rhs, "gtetmp");
                default:
                    fprintf(stderr, "CodeGen Error: Unknown binary operator: %d\n", bin_expr_node->op);
                    return NULL;
            }
        }
        case NODE_IF_STATEMENT: 
        {
            IfStatementNode* if_node = (IfStatementNode*)node;
            LLVMValueRef condition_val = generate_expression(if_node->condition);
            if (!condition_val) return NULL;

            // Convert condition to a boolean (i1) if it's not already
            LLVMTypeRef cond_type = LLVMTypeOf(condition_val);
            if (LLVMGetTypeKind(cond_type) != LLVMIntegerTypeKind || LLVMGetIntTypeWidth(cond_type) != 1) {
                // Compare with 0 to get i1 result
                condition_val = LLVMBuildICmp(llvm_builder, LLVMIntNE, condition_val, 
                    LLVMConstInt(cond_type, 0, 0), "ifcond");
            }

            LLVMBasicBlockRef then_block = LLVMAppendBasicBlockInContext(llvm_context, current_function_ref, "then");
            LLVMBasicBlockRef else_block = if_node->else_body ? LLVMAppendBasicBlockInContext(llvm_context, current_function_ref, "else") : NULL;
            LLVMBasicBlockRef merge_block = LLVMAppendBasicBlockInContext(llvm_context, current_function_ref, "ifcont");

            if (else_block) LLVMBuildCondBr(llvm_builder, condition_val, then_block, else_block);
            else            LLVMBuildCondBr(llvm_builder, condition_val, then_block, merge_block);

            // Emit then block
            LLVMPositionBuilderAtEnd(llvm_builder, then_block);
            generate_node(if_node->body);
            if (current_block_needs_terminator()) 
            { // Ensure block is terminated
                LLVMBuildBr(llvm_builder, merge_block);
            }
            

            // Emit else block if it exists
            if (else_block) 
            {
                LLVMPositionBuilderAtEnd(llvm_builder, else_block);
                generate_node(if_node->else_body);
                if (current_block_needs_terminator())
                { // Ensure block is terminated
                    LLVMBuildBr(llvm_builder, merge_block);
                }
            }
            
            LLVMPositionBuilderAtEnd(llvm_builder, merge_block);
            return NULL; // If statements don't produce a value themselves
        }
        case NODE_WHILE_STATEMENT: 
        {
            WhileStatementNode* while_node = (WhileStatementNode*)node;

            LLVMBasicBlockRef cond_block = LLVMAppendBasicBlockInContext(llvm_context, current_function_ref, "loopcond");
            LLVMBasicBlockRef body_block = LLVMAppendBasicBlockInContext(llvm_context, current_function_ref, "loopbody");
            LLVMBasicBlockRef after_block = LLVMAppendBasicBlockInContext(llvm_context, current_function_ref, "loopafter");

            LLVMBuildBr(llvm_builder, cond_block); // Jump to condition check

            // Condition block
            LLVMPositionBuilderAtEnd(llvm_builder, cond_block);
            LLVMValueRef cond_val = generate_expression(while_node->condition);
            if (!cond_val) return NULL; 
            
            // Convert condition to boolean if needed (same as if statement)
            LLVMTypeRef cond_type = LLVMTypeOf(cond_val);
            if (LLVMGetTypeKind(cond_type) != LLVMIntegerTypeKind || LLVMGetIntTypeWidth(cond_type) != 1) {
                cond_val = LLVMBuildICmp(llvm_builder, LLVMIntNE, cond_val, 
                    LLVMConstInt(cond_type, 0, 0), "whilecond");
            }
            LLVMBuildCondBr(llvm_builder, cond_val, body_block, after_block);

            // Body block
            LLVMPositionBuilderAtEnd(llvm_builder, body_block);
            generate_node(while_node->body);
            if (current_block_needs_terminator()) 
            { // Ensure block is terminated
                 LLVMBuildBr(llvm_builder, cond_block); // Jump back to condition
            }

            LLVMPositionBuilderAtEnd(llvm_builder, after_block);
            return NULL;
        }
        case NODE_FUNCTION_CALL: 
        {
            FunctionCallNode* call_node = (FunctionCallNode*)node;
            IdentifierNode* func_name_node = (IdentifierNode*)call_node->identifier;

            LLVMValueRef func_to_call = LLVMGetNamedFunction(llvm_module, func_name_node->symbol->name);
            if (!func_to_call) 
            {
                fprintf(stderr, "CodeGen Error: Calling undefined function %s\n", func_name_node->symbol->name);
                return NULL;
            }

            // Count arguments
            int arg_count = 0;
            ASTNode* arg_ast_count = call_node->params;
            while(arg_ast_count) 
            {
                arg_count++;
                arg_ast_count = arg_ast_count->next; // Assuming params are a linked list of expression nodes
            }

            LLVMValueRef* args = malloc(sizeof(LLVMValueRef) * arg_count);
            ASTNode* current_arg_ast = call_node->params;
            for (int i = 0; i < arg_count; ++i) 
            {
                args[i] = generate_expression(current_arg_ast); // Each param is an expression
                if (!args[i]) {
                    free(args);
                    return NULL; // Error in argument generation
                }
                current_arg_ast = current_arg_ast->next;
            }

            LLVMTypeRef func_type = LLVMGlobalGetValueType(func_to_call);
            LLVMTypeRef func_return_type = LLVMGetReturnType(func_type); // Get function's declared return type

            const char *call_name = "";
            if (LLVMGetTypeKind(func_return_type) != LLVMVoidTypeKind) 
            {
                 call_name = "calltmp"; // Name the result if it's not void
            }

            LLVMValueRef call_val = LLVMBuildCall2(llvm_builder, func_type, func_to_call, args, arg_count, call_name);
            free(args);
            return call_val;
        }
        case NODE_GLOBAL_VARIABLE_DECLARATION: 
        {
            GlobalVariableDeclarationNode* global_var_decl = (GlobalVariableDeclarationNode*)node;
            TypeNode* type_node = (TypeNode*)global_var_decl->type;
            LLVMTypeRef llvm_type = get_llvm_type(type_node);

            ASTNode* current_declarator_ast = global_var_decl->start;
            while(current_declarator_ast) 
            {
                VariableDeclaratorNode* decl_node = (VariableDeclaratorNode*)current_declarator_ast;
                IdentifierNode* var_name_node = (IdentifierNode*)decl_node->identifier;

                LLVMValueRef global_var = LLVMAddGlobal(llvm_module, llvm_type, var_name_node->symbol->name);
                LLVMSetLinkage(global_var, LLVMInternalLinkage); // Or other linkage as appropriate

                if (decl_node->expression) 
                {
                    // TODO
                    // Global variable initializers must be constants.
                    // This requires a more complex constant expression evaluation.
                    // For simplicity, let's assume integer literals for now.
                    if (decl_node->expression->type == NODE_NUMBER_LITERAL) 
                    {
                        NumberNode* num_node = (NumberNode*)decl_node->expression;
                        LLVMSetInitializer(global_var, LLVMConstInt(llvm_type, num_node->value, 0));
                    }
                    else
                    {
                        fprintf(stderr, "CodeGen Warning: Global variable %s initializer is not a constant literal. Not supported in this simplified version.\\n", var_name_node->symbol->name);
                        LLVMSetInitializer(global_var, LLVMConstNull(llvm_type)); // Default to zero/null
                    }
                } 
                else 
                {
                    LLVMSetInitializer(global_var, LLVMConstNull(llvm_type)); // Default to zero/null if no initializer
                }

                current_declarator_ast = current_declarator_ast->next;
            }
            return NULL;
        }
        // Add cases for other AST node types:
        // NODE_VARIABLE_DECLARATOR (handled within local/global var decl),
        // NODE_PARAMETER (handled within function def),
        // NODE_TYPE (handled by get_llvm_type), NODE_ERROR
        default:
            fprintf(stderr, "Codegen: Unhandled AST node type: %d\n", node->type);
            return NULL;
    }
}

static LLVMValueRef generate_statement(ASTNode* stmt_node) 
{
    // Wrapper for generate_node, as most statements will be handled there
    return generate_node(stmt_node);
}

static LLVMValueRef generate_expression(ASTNode* expr_node) 
{
    // Wrapper for generate_node, as most expressions will be handled there
    return generate_node(expr_node);
}

static int current_block_needs_terminator() 
{
    LLVMBasicBlockRef current_block = LLVMGetInsertBlock(llvm_builder);
    return current_block && LLVMGetBasicBlockTerminator(current_block) == NULL;
}
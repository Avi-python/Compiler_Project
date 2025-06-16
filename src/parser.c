#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "ast.h"
#include "symbol_table.h"
#include "parser.h"

// External from lexer
extern FILE *yyin;
extern char* yyfilename;
extern int error_count;
extern int lineno;
extern int yylex();
extern void save_error_pos(char* type, char* token);
extern void show_and_free_errors();

ASTNode* program();
ASTNode* external_declaration();
ASTNode* declarations(ASTNode* type_node, Symbol* id_sym);
ASTNode* function_definition_body(ASTNode* type_node, Symbol* func_sym);
ASTNode* variable_declaration_global_body(ASTNode* type_node, Symbol* first_var_sym);
ASTNode* parameter_list_opt();
ASTNode* parameter_list();
ASTNode* parameter_declaration();
ASTNode* compound_statement();
ASTNode* statement_list();
ASTNode* statement();
ASTNode* assign_or_func_call(Symbol* id_sym);
ASTNode* assignment_statement(Symbol* id_sym);
ASTNode* function_call_statement(Symbol* id_sym);
ASTNode* argument_list_opt();
ASTNode* argument_list();
ASTNode* declare_statement();
ASTNode* init_declarator();
ASTNode* expression();
ASTNode* arithmetic_expression();
ASTNode* arithmetic_prime(ASTNode* left_operand);
ASTNode* relational_prime(ASTNode* left_operand);
ASTNode* term();
ASTNode* term_prime(ASTNode* left_operand);
ASTNode* factor();
ASTNode* epsilon_or_func_call_from_sym(Symbol* sym);
ASTNode* type();
ASTNode* if_statement();
ASTNode* while_statement();
ASTNode* return_statement();

// --- Main Driver --- (This section will be replaced)

// Global variables used by the parser
YYSTYPE yylval;
ASTNode *ast_root = NULL;
sym_t *global_sym_table = NULL;
sym_t *current_sym_table = NULL; // Tracks current scope
int token;
FILE *log_file = NULL;

// Error tracking for parser specific errors
int is_in_follow_set(int* follow_set, int size);
void error_recovery(int* follow_set, int size, const char* production_name);
char* token_type_to_string(int token_type);
void get_next_token();

// Helper function to check if token is in follow set
int is_in_follow_set(int* follow_set, int size) 
{
    for (int i = 0; i < size; i++) 
    {
        if (token == follow_set[i]) 
        {
            return 1;
        }
    }
    return 0;
}

// Error recovery function - skip tokens until we find one in the follow set
void error_recovery(int* follow_set, int size, const char* production_name) 
{
    fprintf(log_file, "Attempting error recovery in %s. Current token: %s. Skipping until one of: [",
            production_name, token_type_to_string(token));

    for(int i=0; i<size; ++i) 
    {
        fprintf(log_file, "%s ", token_type_to_string(follow_set[i]));
    }
    fprintf(log_file, "] is found.\n");

    while (token != 0 && token != EOF && !is_in_follow_set(follow_set, size)) 
    {
        fprintf(log_file, "Skipping token: %s during error recovery for %s\n",
                token_type_to_string(token), production_name);
        get_next_token();
    }
    fprintf(log_file, "Error recovery for %s finished. Current token: %s\n",
            production_name, token_type_to_string(token));
}

char* token_type_to_string(int token_type) 
{
    switch (token_type) {
        case IDENTIFIER: return "IDENTIFIER";
        case NUMBER: return "NUMBER";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case MUL: return "MUL";
        case DIV: return "DIV";
        case ASSIGN: return "ASSIGN";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";
        case SEMI: return "SEMI";
        case INT: return "INT";
        case CHAR: return "CHAR";
        case VOID: return "VOID";
        case RETURN: return "RETURN";
        case IF: return "IF";
        case ELSE: return "ELSE";
        case DO: return "DO";
        case WHILE: return "WHILE";
        case CONST: return "CONST";
        case LE: return "LE";
        case GE: return "GE";
        case EQ: return "EQ";
        case NE: return "NE";
        case LT: return "LT";
        case GT: return "GT";
        case COMMA: return "COMMA";
        case EOF: return "EOF";
        default: 
            return "UNKNOWN";
    }
}

void get_next_token() 
{
    token = yylex(); // Get the next token from the lexer
}

// Match and consume a token
int match(int expected_type) 
{
    fprintf(log_file, "Matching token: %s (expected: %s)\n", 
           token_type_to_string(token), 
           token_type_to_string(expected_type));
    if (token == expected_type) 
    {
        get_next_token();
        return 1;
    }
    return 0;
}

// <Program> ::= <ExternalDeclaration> { <ExternalDeclaration> }
ASTNode* program() 
{
    fprintf(log_file, "Parsing <Program>\n");

    ProgramNode* program_node = create_program_node(NULL);
    ASTNode* current_decl_list_item = NULL;

    while (1) 
    {
        // Check FIRST set of <ExternalDeclaration> (INT, CHAR, VOID)
        if (token != INT && token != CHAR && token != VOID) 
        {
            if (token == EOF) break; // Successfully reached EOF

            int follow_set[] = {INT, CHAR, VOID, EOF};
            char error_msg[200];
            sprintf(error_msg, "Unexpected token %s at start of or between external declarations",
                    token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "program");
            if (token == EOF) break; // EOF after recovery
            if (token != INT && token != CHAR && token != VOID) continue; // Skip if recovery didn't find a start
        }

        ASTNode* decl_node = external_declaration();
        if (decl_node) 
        {
            if (program_node->start == NULL) 
            {
                program_node->start = decl_node;
                current_decl_list_item = decl_node;
            } 
            else 
            {
                current_decl_list_item->next = decl_node;
                current_decl_list_item = decl_node;
            }
        }
        
        // If external_declaration consumed EOF or an error occurred and token is EOF
        if (token == EOF) break;
    }
    return (ASTNode*)program_node;
}

// <ExternalDeclaration> ::= <Type> <Identifier> <Declarations>
ASTNode* external_declaration() 
{
    fprintf(log_file, "Parsing <ExternalDeclaration>\n");
    ASTNode* type_node = type();

    Symbol* id_sym = NULL;
    if (token == IDENTIFIER) 
    {
        // check if the identifier is already declared in the current scope
        if (check_symbol_in_current_scope(current_sym_table, yylval.sval)) 
        {
            char error_msg[200];
            sprintf(error_msg, "Redeclaration of identifier '%s'", yylval.sval);
            save_error_pos("semantic error", error_msg);
        }
        else
        {
            id_sym = insert_symbol(current_sym_table, yylval.sval, token);
            if (!id_sym) {
                perror("Failed to create symbol for identifier");
                free_ast(type_node);
                return (ASTNode*)create_error_node();
            }
        }
        match(IDENTIFIER);
    } 
    else 
    {
        int follow_set[] = {LPAREN, ASSIGN, COMMA, SEMI, INT, CHAR, VOID, EOF}; // Follow of ID or First of Declarations
        char error_msg[200];
        sprintf(error_msg, "Expected identifier after type, got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "external_declaration_id");
        free_ast(type_node);
        // If recovery doesn't find a suitable token for declarations, or if id_sym is crucial.
        return (ASTNode*)create_error_node(); 
    }

    ASTNode* specific_decl_node = declarations(type_node, id_sym);
    return specific_decl_node;
}

// <Declarations> ::= <FunctionDefinition> | <VariableDeclarationGlobal> ;
ASTNode* declarations(ASTNode* type_node, Symbol* id_sym) 
{
    fprintf(log_file, "Parsing <Declarations>\n");
    ASTNode* result_node = NULL;
    if (token == LPAREN) // First of <FunctionDefinition>
    { 
        result_node = function_definition_body(type_node, id_sym);
    } 
    else if (token == ASSIGN || token == COMMA) 
    { 
        result_node = variable_declaration_global_body(type_node, id_sym);
        if (!match(SEMI)) 
        {
            int follow_set[] = {INT, CHAR, VOID, EOF}; // Follow set for <Declarations>
            char error_msg[200];
            sprintf(error_msg, "Expected \';\' after variable declaration, got %s",
                    token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "declarations_semi");
            free_ast(result_node); // Free the result node if it didn't match SEMI
            result_node = (ASTNode*)create_error_node(); // Create an error node
        }
    } 
    else 
    {
        // Unexpected token for declarations
        int follow_set[] = {INT, CHAR, VOID, EOF};
        char error_msg[200];
        sprintf(error_msg, "Unexpected token %s in declarations", token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "declarations_unexpected");
        free_ast(type_node);
        return (ASTNode*)create_error_node();
    }
    return result_node;
}


// <VariableDeclarationGlobal> ::= [ = <Expression> ] { , <Identifier> [ = <Expression> ] }
ASTNode* variable_declaration_global_body(ASTNode* type_node, Symbol* first_var_sym) 
{
    fprintf(log_file, "Parsing <VariableDeclarationGlobalBody>\n");
    
    GlobalVariableDeclarationNode* gvd_node = create_global_variable_declaration_node(type_node, NULL);
    if(!gvd_node)
    {
        perror("Failed to create GlobalVariableDeclarationNode");
        free_ast(type_node);
        return (ASTNode*)create_error_node();
    }

    ASTNode* current_declarator_item = NULL;
    ASTNode* first_id_node = NULL;
    if(!first_var_sym) first_id_node = (ASTNode*)create_error_node();
    else               first_id_node = (ASTNode*)create_identifier_node(first_var_sym);

    ASTNode* first_expr_node = NULL;
    if (token == ASSIGN) 
    {
        match(ASSIGN);
        first_expr_node = expression();
    }
    ASTNode* first_decl_node = (ASTNode*)create_variable_declarator_node(first_id_node, first_expr_node);
    gvd_node->start = first_decl_node;
    current_declarator_item = first_decl_node;

    while (token == COMMA) 
    {
        match(COMMA);
        Symbol* next_var_sym = NULL;
        if (token == IDENTIFIER) 
        {
            if(check_symbol_in_current_scope(current_sym_table, yylval.sval))
            {
                char error_msg[200];
                sprintf(error_msg, "Redeclaration of identifier '%s'", yylval.sval);
                save_error_pos("semantic error", error_msg);
                free_ast((ASTNode*)gvd_node);
                return (ASTNode*)create_error_node();
            }
            next_var_sym = insert_symbol(current_sym_table, yylval.sval, token);
            match(IDENTIFIER);
        } 
        else 
        {
            int follow_set[] = {SEMI, ASSIGN, COMMA}; 
            char error_msg[200];
            sprintf(error_msg, "Expected identifier after \',\', got %s", token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "variable_declaration_global_id");
            free_ast((ASTNode*)gvd_node);
            return (ASTNode*)create_error_node();
        }
        
        ASTNode* next_id_node = (ASTNode*)create_identifier_node(next_var_sym);
        ASTNode* next_expr_node = NULL;
        if (token == ASSIGN) 
        {
            match(ASSIGN);
            next_expr_node = expression();
        }
        ASTNode* next_decl_node = (ASTNode*)create_variable_declarator_node(next_id_node, next_expr_node);
        current_declarator_item->next = next_decl_node;
        current_declarator_item = next_decl_node;
    }
    return (ASTNode*)gvd_node;
}

// <FunctionDefinition> ::= ( <ParameterListOpt> ) <CompoundStatement>
ASTNode* function_definition_body(ASTNode* type_node, Symbol* func_sym) 
{
    fprintf(log_file, "Enter <FunctionDefinitionBody> for '%s'. Outer scope: depth %d, addr %p\n",
            func_sym == NULL ? "NULL" : func_sym->name, current_sym_table->depth, (void*)current_sym_table);

    sym_t* scope_where_func_is_declared = current_sym_table;
    sym_t* function_scope = create_symbol_table(); // Scope for parameters and function locals' first level
    // TODO : error handling

    match(LPAREN);

    function_scope->parent = scope_where_func_is_declared;
    function_scope->depth = scope_where_func_is_declared->depth + 1;
    current_sym_table = function_scope; // Enter function's own scope

    fprintf(log_file, "Entered function scope for '%s'. New depth: %d, addr: %p, parent: %p\n",
            func_sym == NULL ? "NULL" : func_sym->name, current_sym_table->depth, (void*)current_sym_table, (void*)current_sym_table->parent);

    ASTNode* params_list_node = parameter_list_opt();

    if (!match(RPAREN)) 
    {
        int follow_set[] = {LBRACE, INT, CHAR, VOID, EOF}; // Follow for FuncDef or start of CompoundStmt
        char error_msg[200];
        sprintf(error_msg, "Expected \')\' after parameter list, got %s", token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "function_definition_rparen");
        // If RPAREN is missing, proceed if recovery finds LBRACE, otherwise error.
        if (token != LBRACE) {
            free_all_symbol_tables(function_scope);
            free_ast(type_node); free_sym(func_sym); free_ast(params_list_node);
            return (ASTNode*)create_error_node();
        }
    }

    ASTNode* body_node = compound_statement();
    ASTNode* func_id_node;
    if(func_sym) func_id_node = (ASTNode*)create_identifier_node(func_sym);
    else         func_id_node = (ASTNode*)create_error_node();

    FunctionDefinitionNode* fdn_node = create_function_definition_node(type_node, func_id_node, params_list_node, body_node);

    add_child_symbol_table(scope_where_func_is_declared, function_scope); 
    current_sym_table = scope_where_func_is_declared;
    fprintf(log_file, "Exit <FunctionDefinitionBody> for '%s'. Restored scope: depth %d, addr %p\\n",
            func_sym == NULL ? "NULL" : func_sym->name, current_sym_table->depth, (void*)current_sym_table);

    return (ASTNode*)fdn_node;
}

// <ParameterListOpt> ::= <ParameterList> | epsilon
ASTNode* parameter_list_opt() 
{
    fprintf(log_file, "Parsing <ParameterListOpt>\\n");
    // FIRST of ParameterList is FIRST of ParameterDeclaration, which is FIRST of Type
    if (token == INT || token == CHAR || token == VOID) 
    {
        return parameter_list();
    }
    return NULL; 
}

// <ParameterList> ::= <ParameterDeclaration> { , <ParameterDeclaration> }
ASTNode* parameter_list() 
{
    fprintf(log_file, "Parsing <ParameterList>\n");
    ASTNode* head_param_node = NULL;
    ASTNode* current_param_node = NULL;

    ASTNode* param_decl_node = parameter_declaration();
    head_param_node = param_decl_node;
    current_param_node = param_decl_node;

    while (token == COMMA) 
    {
        match(COMMA);
        param_decl_node = parameter_declaration();
        current_param_node->next = param_decl_node;
        current_param_node = param_decl_node;
    }
    return head_param_node;
}

// <ParameterDeclaration> ::= <Type> <Identifier>
ASTNode* parameter_declaration() 
{
    fprintf(log_file, "Parsing <ParameterDeclaration>. Current scope: depth %d, addr %p\n",
            current_sym_table->depth, (void*)current_sym_table);
    ASTNode* type_node = type();

    Symbol* param_sym = NULL;
    if (token == IDENTIFIER) 
    {
        if(check_symbol_in_current_scope(current_sym_table, yylval.sval))
        {
            char error_msg[200];
            sprintf(error_msg, "Redeclaration of identifier '%s'", yylval.sval);
            save_error_pos("semantic error", error_msg);
            free_ast((ASTNode*)type_node);
            return (ASTNode*)create_error_node();
        }
        param_sym = insert_symbol(current_sym_table, yylval.sval, token);
        match(IDENTIFIER);
    } 
    else 
    {
        int follow_set[] = {COMMA, RPAREN}; 
        char error_msg[200];
        sprintf(error_msg, "Expected identifier after type in parameter declaration, got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "parameter_declaration_id");
        free_ast(type_node);
        return (ASTNode*)create_error_node();
    }
    
    ASTNode* param_id_node = (ASTNode*)create_identifier_node(param_sym);
    return (ASTNode*)create_parameter_node(type_node, param_id_node);
}


// <Block> ::= int main ( ) <CompoundStatement>
// This is no longer the top-level program structure.
// It might be useful if we want to enforce a 'main' function specifically,
// but the current EBNF treats all functions similarly.
// For now, I will comment it out. If a specific 'main' handling is needed,
// it would be a semantic check or a modified grammar rule.
/*
void block() {
    printf("Parsing <Block>\\n");
    match(INT);
    match(MAIN); // MAIN token would be needed if we distinguish main
    match(LPAREN);
    match(RPAREN);
    compound_statement();
}
*/
// <CompoundStatement> ::= { <StatementList> }
// Note: This function parses the content *within* the braces.
// The braces themselves are matched by the caller if it's the top-level compound statement of a block,
// or by statement_parser if it's a compound statement used as a statement.
// For simplicity, this function will match its own braces.
ASTNode* compound_statement() 
{
    fprintf(log_file, "Parsing <CompoundStatement>. Current token: %s\n", token_type_to_string(token));

    sym_t* outer_scope = current_sym_table;
    sym_t* new_block_scope = create_symbol_table(); // TODO : error handling

    new_block_scope->parent = outer_scope;
    new_block_scope->depth = outer_scope->depth + 1;
    current_sym_table = new_block_scope;

    fprintf(log_file, "Entered new block scope. New depth: %d, addr: %p, parent: %p\\n",
            current_sym_table->depth, (void*)current_sym_table, (void*)current_sym_table->parent);

    if(!match(LBRACE))
    {
        int follow_set[] = {EOF, IDENTIFIER, LBRACE, INT, CHAR, VOID, IF, WHILE, RBRACE, ELSE, RETURN};
        char error_msg[200];
        sprintf(error_msg, "Expected \'{\' to start compound statement, got %s", token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "compound_statement_missing_lbrace");
        if(token != LBRACE)
        {
            current_sym_table = outer_scope;
            free_all_symbol_tables(new_block_scope); 
            return (ASTNode*)create_error_node(); 
        }
        match(LBRACE); // If recovery found LBRACE, consume it.
    }

    ASTNode* stmt_list_node = statement_list(); // Can be NULL if empty list
    
    if(!match(RBRACE))
    {
        int follow_set[] = {EOF, IDENTIFIER, LBRACE, INT, CHAR, VOID, IF, WHILE, RBRACE, ELSE, RETURN};
        char error_msg[200];
        sprintf(error_msg, "Expected \'}\' at end of compound statement, got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "compound_statement_missing_rbrace");
        // If RBRACE is still not found, the compound statement is malformed.
        // stmt_list_node might be valid, but the structure is broken.
        // Depending on strictness, could return error or proceed.
        // For now, let's assume recovery positions for next statement.
        // If we want to be strict, and RBRACE is not found after recovery:
        if(token != RBRACE) 
        { // Check if match(RBRACE) would fail after recovery
            current_sym_table = outer_scope;
            free_all_symbol_tables(new_block_scope); 
            free_ast(stmt_list_node);
             return (ASTNode*)create_error_node();
        }
        // If recovery found RBRACE, it will be matched by the earlier match(RBRACE) call if it was re-attempted,
        // or the logic needs adjustment. The current match() is outside this if.
        // The original code just proceeds. Let's stick to that for now.
    }

    CompoundStatementNode* cs_node = create_compound_statement_node(stmt_list_node);

    add_child_symbol_table(outer_scope, new_block_scope);
    current_sym_table = outer_scope;
    fprintf(log_file, "Exit <CompoundStatement>. Restored scope: depth %d, addr %p. Token: %s\\n",
            current_sym_table->depth, (void*)current_sym_table, token_type_to_string(token));
    return (ASTNode*)cs_node;
}


// <StatementList> ::= { <Statement> }
ASTNode* statement_list() 
{
    fprintf(log_file, "Parsing <StatementList>. Current token: %s\n", token_type_to_string(token));
    ASTNode* head_stmt_node = NULL;
    ASTNode* current_stmt_node = NULL;

    while(token != RBRACE && token != EOF) 
    {
        int initial_error_count = error_count;
        int initial_token = token;
        int initial_lineno = lineno;

        ASTNode* stmt_node = statement();

        if (stmt_node) 
        { 
            if (head_stmt_node == NULL) 
            {
                head_stmt_node = stmt_node;
                current_stmt_node = stmt_node;
            } 
            else 
            {
                current_stmt_node->next = stmt_node;
                current_stmt_node = stmt_node;
            }
            // Ensure current_stmt_node points to the actual last node if stmt_node was a list (it shouldn't be)
            while(current_stmt_node && current_stmt_node->next) current_stmt_node = current_stmt_node->next;
        }
        // If stmt_node is NULL (epsilon), do nothing to the list.

        // Failsafe for infinite loop
        if (token == initial_token &&
            lineno == initial_lineno && 
            error_count == initial_error_count &&
            token != RBRACE && token != EOF) 
        {
            fprintf(log_file, "Internal Parser Error: statement() did not advance token (%s) or report error in statement_list. Forcing advance.\\n", token_type_to_string(token));
            save_error_pos("internal parser error", "Statement parser stuck, forcing advance.");
            get_next_token(); 
        }
    }
    fprintf(log_file, "Exiting <StatementList>. Next token for compound_statement: %s\n", token_type_to_string(token));
    return head_stmt_node;
}


// <Statement> ::= | <CompoundStatement>
//                | <DeclareStatement> ;
//                | <Identifier> <AssignOrFuncCall> ;
//                | <IfStatement>
//                | <WhileStatement>
//                | <ReturnStatement> ;
//                | epsilon
ASTNode* statement() 
{
    fprintf(log_file, "Parsing <Statement> (current token: %s, line: %d)\n", token_type_to_string(token), lineno);

    int stmt_follow_set[] = {IDENTIFIER, LBRACE, INT, CHAR, VOID, IF, WHILE, RBRACE, ELSE, RETURN};
    int stmt_follow_set_size = sizeof(stmt_follow_set)/sizeof(stmt_follow_set[0]);
    ASTNode* stmt_node = NULL;

    // First of <DeclareStatement>
    if (token == INT || token == CHAR) 
    { 
        stmt_node = declare_statement();
        if(!match(SEMI)) 
        {
            char error_msg[200];
            sprintf(error_msg, "Expected \';\' after declaration statement, got %s", token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(stmt_follow_set, stmt_follow_set_size, "statement_after_declare");
            free_ast(stmt_node);
            return (ASTNode*)create_error_node();
        }
        return stmt_node;
    }

    if (token == LBRACE) // First of <CompoundStatement>
    { 
        return compound_statement();
    }

    if(token == IDENTIFIER) 
    {
        Symbol* id_sym = create_sym(yylval.sval, token);
        match(IDENTIFIER);
        stmt_node = assign_or_func_call(id_sym); // id_sym is consumed by assign_or_func_call
        if(!match(SEMI)) {
            char error_msg[200];
            sprintf(error_msg, "Expected \';\' after assignment or function call, got %s", token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(stmt_follow_set, stmt_follow_set_size, "statement_after_assign_or_call");
            free_ast(stmt_node);
            return (ASTNode*)create_error_node();
        }
        return stmt_node;
    }
    if(token == IF) 
    {
        return if_statement();
    }
    if(token == WHILE) 
    {
        return while_statement();
    }
    if(token == RETURN) 
    {
        stmt_node = return_statement();
        if(!match(SEMI)) 
        {
            char error_msg[200];
            sprintf(error_msg, "Expected \';\' after return statement, got %s", token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(stmt_follow_set, stmt_follow_set_size, "statement_after_return");
            free_ast(stmt_node);
            return (ASTNode*)create_error_node();
        }
        return stmt_node;
    }

    // Special case: handle 'else' without 'if'
    if(token == ELSE) 
    {
        char error_msg[200];
        sprintf(error_msg, "\'else\' without matching \'if\'");
        save_error_pos("syntax error", error_msg);
        get_next_token(); // Consume 'else'

        if (token == LBRACE) 
        { 
            int brace_count = 1;
            get_next_token(); 
            while (token != EOF && brace_count > 0) 
            { 
                if (token == LBRACE) brace_count++;
                else if (token == RBRACE) brace_count--;
                get_next_token();
            }
        }
        return (ASTNode*)create_error_node(); // 'else' without 'if' is an error
    }


    // Epsilon production for <Statement> or start of next valid statement
    if (is_in_follow_set(stmt_follow_set, stmt_follow_set_size)) 
    {
        fprintf(log_file, "Epsilon production for <Statement> or at valid follow token: %s\n", token_type_to_string(token));
        return NULL;
    }

    // If no statement rule matched and not epsilon/follow:
    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in statement. Cannot start a new statement.", token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
    error_recovery(stmt_follow_set, stmt_follow_set_size, "statement_unexpected_token");
    return (ASTNode*)create_error_node();
}

// <AssignmentStatement> ::= = <Expression>
ASTNode* assignment_statement(Symbol* id_sym) 
{
    fprintf(log_file, "Parsing <AssignmentStatement>\n");
    ASTNode* id_node = (ASTNode*)create_identifier_node(id_sym);

    if (!match(ASSIGN)) 
    {
        free_ast(id_node); 
        return (ASTNode*)create_error_node();
    }

    ASTNode* expr_node = expression();
    return (ASTNode*)create_assignment_statement_node(id_node, expr_node);
}

// <AssignOrFuncCall> ::= <AssignmentStatement> | <FunctionCallStatement> 
ASTNode* assign_or_func_call(Symbol* id_sym) 
{
    fprintf(log_file, "Parsing <AssignOrFuncCall> (current token: %s)\n", token_type_to_string(token));
    
    if (token == ASSIGN) return assignment_statement(id_sym);

    if (token == LPAREN) return function_call_statement(id_sym);

    // Error: expected '=' or '(' after identifier in a statement context
    int follow_set[] = {SEMI}; // Follow for AssignOrFuncCall (which is ';')
    char error_msg[200];
    sprintf(error_msg, "Expected \'=\' or \'(\' after identifier, got %s", token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
    error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "assign_or_func_call_operator");
    return (ASTNode*)create_error_node();
}

// <FunctionCallStatement> ::= ( <ArgumentListOpt> ) ; (Identifier already matched, passed as id_sym)
ASTNode* function_call_statement(Symbol* id_sym) 
{
    fprintf(log_file, "Parsing <FunctionCallStatement> (current token: %s)\n", token_type_to_string(token));
    ASTNode* id_node = (ASTNode*)create_identifier_node(id_sym); // Consumes id_sym

    if(!match(LPAREN)) // LPAREN already checked by assign_or_func_call
    { 
        free_ast(id_node);
        return (ASTNode*)create_error_node();
    }
    ASTNode* args_node = argument_list_opt(); // Can be NULL or ErrorNode

    if (!match(RPAREN)) 
    {
        int follow_set[] = {SEMI};
        char error_msg[200];
        sprintf(error_msg, "Expected \')\' after function call arguments, got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "function_call_statement_rparen");
        // If RPAREN missing, proceed if recovery finds SEMI, otherwise error
        if (token != SEMI) { // A more robust check might be needed
            free_ast(id_node); free_ast(args_node);
            return (ASTNode*)create_error_node();
        }
    }

    return (ASTNode*)create_function_call_node(id_node, args_node);
}

// <ArgumentListOpt> ::= <ArgumentList> | epsilon
ASTNode* argument_list_opt() 
{
    fprintf(log_file, "Parsing <ArgumentListOpt> (current token: %s)\n", token_type_to_string(token));
    // FIRST of ArgumentList is FIRST of Expression (ID, NUMBER, LPAREN)
    if (token == IDENTIFIER || token == NUMBER || token == LPAREN) {
        return argument_list();
    }
    // Epsilon case
    return NULL; 
}

// <ArgumentList> ::= <Expression> { , <Expression> }
ASTNode* argument_list() 
{
    fprintf(log_file, "Parsing <ArgumentList> (current token: %s)\n", token_type_to_string(token));
    ASTNode* head_expr_node = NULL;
    ASTNode* current_expr_node = NULL;

    ASTNode* expr_node = expression();
    head_expr_node = expr_node;
    current_expr_node = expr_node;

    while (token == COMMA) 
    {
        match(COMMA);
        expr_node = expression();
        current_expr_node->next = expr_node;
        current_expr_node = expr_node;
    }
    return head_expr_node;
}

// <DeclareStatement> ::= <Type> <InitDeclarator> { , <InitDeclarator> }
ASTNode* declare_statement() 
{
    fprintf(log_file, "Parsing <DeclareStatement> (current token: %s)\n", token_type_to_string(token));
    ASTNode* type_node = type();

    LocalVariableDeclarationNode* lvd_node = create_local_variable_declaration_node(type_node, NULL);
    if (!lvd_node)
    {
        perror("Failed to create LocalVariableDeclarationNode");
        free_ast(type_node);
        return (ASTNode*)create_error_node();
    }
    
    ASTNode* current_declarator_item = NULL;

    ASTNode* decl_node = init_declarator();
    lvd_node->start = decl_node;
    current_declarator_item = decl_node;

    while(token == COMMA) 
    {
        match(COMMA);
        decl_node = init_declarator();
        current_declarator_item->next = decl_node;
        current_declarator_item = decl_node;
    }
    return (ASTNode*)lvd_node;
}

// <InitDeclarator> ::= <Identifier> [ = <Expression> ]
ASTNode* init_declarator() 
{
    fprintf(log_file, "Parsing <InitDeclarator>. Current scope: depth %d, addr %p\n",
            current_sym_table->depth, (void*)current_sym_table);
    Symbol* var_sym = NULL;
    if (token == IDENTIFIER) 
    {
        if (check_symbol_in_current_scope(current_sym_table, yylval.sval)) 
        {
            char error_msg[200];
            sprintf(error_msg, "Redeclaration of identifier '%s'", yylval.sval);
            save_error_pos("semantic error", error_msg);
        }
        else
        {
            var_sym = insert_symbol(current_sym_table, yylval.sval, token);
            if (!var_sym) {
                perror("Failed to create symbol for identifier");
                return (ASTNode*)create_error_node();
            }
        }
        match(IDENTIFIER);
    } 
    else 
    {
        int follow_set[] = {COMMA, SEMI, ASSIGN}; 
        char error_msg[200];
        sprintf(error_msg, "Expected identifier in initializer, got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "init_declarator_id");
        return (ASTNode*)create_error_node();
    }
    
    ASTNode* id_node = NULL;
    if(var_sym) id_node = (ASTNode*)create_identifier_node(var_sym);
    else        id_node = (ASTNode*)create_error_node();

    ASTNode* expr_node = NULL;

    if (token == ASSIGN) 
    {
        match(ASSIGN);
        expr_node = expression();
    } 
    else if (token != COMMA && token != SEMI) 
    { 
        int follow_set[] = {COMMA, SEMI}; 
        char error_msg[200];
        sprintf(error_msg, "Expected \'=\' or \',\' or \';\' after identifier in declarator, got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "init_declarator_follow");
        free_ast(id_node);
        return (ASTNode*)create_error_node();
    }
    return (ASTNode*)create_variable_declarator_node(id_node, expr_node);
}

// <Type> ::= int | char | void
ASTNode* type() 
{
    fprintf(log_file, "Parsing <Type> (current token: %s)\n",  token_type_to_string(token));
    int type_val = -1;
    if (token == INT) 
    {
        type_val = token;
        match(INT);
    } 
    else if (token == CHAR) 
    {
        type_val = token;
        match(CHAR);
    } 
    else if (token == VOID)
    {
        type_val = token;
        match(VOID);
    } 
    else 
    {
        int follow_set[] = {IDENTIFIER}; // Common follow for type is an identifier
        char error_msg[200];
        sprintf(error_msg, "Expected type specifier (int, char, void), got %s", token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "type_specifier");
        return (ASTNode*)create_error_node();
    }
    return (ASTNode*)create_type_node(type_val);
}

// <Expression> ::= <ArithmeticExpression> <RelationalPrime>
ASTNode* expression() 
{
    fprintf(log_file, "Parsing <Expression> (current token: %s)\n", token_type_to_string(token));
    ASTNode* arith_expr_node = arithmetic_expression();
    return relational_prime(arith_expr_node);
}

// <ArithmeticExpression> ::= <Term> <ArithmeticPrime>
ASTNode* arithmetic_expression() 
{
    fprintf(log_file, "Parsing <ArithmeticExpression> (current token: %s)\n", token_type_to_string(token));
    ASTNode* term_node = term();
    return arithmetic_prime(term_node);
}

// <ArithmeticPrime> ::= + <Term> <ArithmeticPrime>
//                     | - <Term> <ArithmeticPrime>
//                     | epsilon
ASTNode* arithmetic_prime(ASTNode* left_operand) 
{
    fprintf(log_file, "Parsing <ArithmeticPrime> (current token: %s)\n", token_type_to_string(token));

    if (token == PLUS || token == MINUS) 
    {
        int op = token;
        match(op);
        ASTNode* term_node = term();
        ASTNode* binary_expr_node = (ASTNode*)create_binary_expression_node(op, left_operand, term_node);
        return arithmetic_prime(binary_expr_node); // Left-associativity through recursion
    } 
    
    // Epsilon production: Check FOLLOW set as in original code for error reporting
    if(token != RPAREN && token != SEMI && token != LE && token != GE && 
        token != LT && token != GT && token != EQ && token != NE && token != COMMA) 
    {
        int follow_set[] = {RPAREN, SEMI, LE, GE, LT, GT, EQ, NE, COMMA};
        if (!is_in_follow_set(follow_set, sizeof(follow_set)/sizeof(follow_set[0])))
        {
            char error_msg[200];
            sprintf(error_msg, "Unexpected token %s in arithmetic expression prime",
                    token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "arithmetic_prime");
            free_ast(left_operand);
            return (ASTNode*)create_error_node();
        }
    }
    return left_operand; // Epsilon case
}

// <RelationalPrime> ::= == <ArithmeticExpression> <RelationalPrime>
//                     | != <ArithmeticExpression> <RelationalPrime>
//                     | < <ArithmeticExpression> <RelationalPrime>
//                     | > <ArithmeticExpression> <RelationalPrime>
//                     | <= <ArithmeticExpression> <RelationalPrime>
//                     | >= <ArithmeticExpression> <RelationalPrime>
//                     | epsilon
ASTNode* relational_prime(ASTNode* left_operand) 
{
    fprintf(log_file, "Parsing <RelationalPrime> (current token: %s)\n", token_type_to_string(token));

    if (token == EQ || token == NE || token == LT || token == GT || token == LE || token == GE) 
    {
        int op = token;
        match(op);
        ASTNode* arith_expr_node = arithmetic_expression();
        ASTNode* binary_expr_node = (ASTNode*)create_binary_expression_node(op, left_operand, arith_expr_node);
        return relational_prime(binary_expr_node); // Left-associativity
    } 
    
    // Epsilon production: Check FOLLOW set for error reporting
    if(token != RPAREN && token != SEMI && token != COMMA) 
    {
        int follow_set[] = {RPAREN, SEMI, COMMA}; 

        if(!is_in_follow_set(follow_set, sizeof(follow_set)/sizeof(follow_set[0])))
        {
            char error_msg[200];
            sprintf(error_msg, "Unexpected token %s in relational expression prime",
                    token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "relational_prime");
            free_ast(left_operand);
            return (ASTNode*)create_error_node();
        }
    }
    return left_operand; // Epsilon case
}

// <Term> ::= <Factor> <TermPrime>
ASTNode* term() 
{
    fprintf(log_file, "Parsing <Term>\n");
    ASTNode* factor_node = factor();
    return term_prime(factor_node);
}

// <TermPrime> ::= * <Factor> <TermPrime> 
//               | / <Factor> <TermPrime>
//               | epsilon
ASTNode* term_prime(ASTNode* left_operand) 
{
    fprintf(log_file, "Parsing <TermPrime> (current token: %s)\n", token_type_to_string(token));
    if (token == MUL || token == DIV) 
    {
        int op = token;
        match(op);
        ASTNode* factor_node = factor();
        ASTNode* binary_expr_node = (ASTNode*)create_binary_expression_node(op, left_operand, factor_node);
        return term_prime(binary_expr_node); // Left-associativity
    } 
    
    // Epsilon production: Check FOLLOW set for error reporting
    if(token != PLUS && token != MINUS && token != RPAREN && token != SEMI && token != LE && token != GE && 
        token != LT && token != GT && token != EQ && token != NE && token != COMMA) 
    {
        int follow_set[] = {PLUS, MINUS, RPAREN, SEMI, LE, GE, LT, GT, EQ, NE, COMMA, RBRACE, EOF};
        if(!is_in_follow_set(follow_set, sizeof(follow_set)/sizeof(follow_set[0])))
        {
            char error_msg[200];
            sprintf(error_msg, "Unexpected token %s in term prime",
                    token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "term_prime");
            free_ast(left_operand);
            return (ASTNode*)create_error_node(); 
        }
    }
    return left_operand; // Epsilon case
}

// <Factor> ::= <Identifier> <EpsilonOrFuncCall> | <Number> | ( <Expression> )
ASTNode* factor() 
{
    fprintf(log_file, "Parsing <Factor>. Current scope: depth %d, addr %p. Token: %s\n",
            current_sym_table->depth, (void*)current_sym_table, token_type_to_string(token));
    ASTNode* node = NULL;
    if (token == IDENTIFIER) 
    {
        Symbol* found_sym = get_symbol(current_sym_table, yylval.sval);
        if (!found_sym) {
            char error_msg[256];
            sprintf(error_msg, "Identifier '%s' not declared in this scope (or parent scopes).", yylval.sval);
            save_error_pos("semantic error", error_msg); // This is a semantic error.
            return (ASTNode*)create_error_node();
        }
        match(IDENTIFIER);
        node = (ASTNode*)epsilon_or_func_call_from_sym(found_sym);
    } 
    else if (token == NUMBER) 
    {
        int val = yylval.ival;
        match(NUMBER);
        node = (ASTNode*)create_number_literal_node(val);
    } 
    else if (token == LPAREN) 
    {
        match(LPAREN);
        node = expression();
        if (!match(RPAREN)) 
        {
            int follow_set[] = {MUL, DIV, PLUS, MINUS, RPAREN, SEMI, LE, GE, LT, GT, EQ, NE, COMMA};
            char error_msg[200];
            sprintf(error_msg, "Expected \')\' after expression in factor, got %s", token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "factor_rparen");
            if (token != RPAREN) 
            {
                 free_ast(node);
                 return (ASTNode*)create_error_node();
            }
            match(RPAREN); // Consume if recovery found it
        }
    } 
    else 
    {
        int follow_set[] = {MUL, DIV, PLUS, MINUS, RPAREN, SEMI, LE, GE, LT, GT, EQ, NE, COMMA, RBRACE, EOF};
        char error_msg[200];
        sprintf(error_msg, "Unexpected token %s in factor. Expected identifier, number, or \'(\'",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "factor_unexpected");
        return (ASTNode*)create_error_node();
    }
    return node;
}

// <EpsilonOrFuncCall> ::= epsilon | <FunctionCallStatement> (modified to take Symbol*)
// Distinguishes between variable usage and function call based on LPAREN
ASTNode* epsilon_or_func_call_from_sym(Symbol* func_sym) 
{
    fprintf(log_file, "Parsing <EpsilonOrFuncCallFromSym> (current token: %s)\n", token_type_to_string(token));
    ASTNode* id_node = (ASTNode*)create_identifier_node(func_sym); // Consumes sym

    if (token == LPAREN) 
    {
        match(LPAREN);
        ASTNode* args_node = argument_list_opt(); // Can be NULL or ErrorNode
        if (!match(RPAREN)) 
        {
            int follow_set[] = {MUL, DIV, PLUS, MINUS, RPAREN, SEMI, LE, GE, LT, GT, EQ, NE, COMMA, RBRACE, EOF};
            char error_msg[200];
            sprintf(error_msg, "Expected \')\' after arguments in factor function call, got %s", token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "epsilon_or_func_call_rparen");
            if (token != RPAREN) 
            {
                free_ast(id_node); free_ast(args_node);
                return (ASTNode*)create_error_node();
            }
            match(RPAREN);
        }
        return (ASTNode*)create_function_call_node(id_node, args_node);
    }
    
    // Epsilon production (it's just an identifier)
    if (token != MUL && token != DIV && token != PLUS && token != MINUS &&
        token != LT && token != LE && token != GT && token != GE &&
        token != EQ && token != NE && token != RPAREN && token != SEMI &&
        token != COMMA) 
    {
        int follow_set[] = {MUL, DIV, PLUS, MINUS, RPAREN, SEMI, LT, LE, GT, GE, EQ, NE, COMMA, RBRACE, EOF};
        char error_msg[200];
        sprintf(error_msg, "Unexpected token %s after identifier in factor/expression",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "epsilon_or_func_call_follow");
        free_ast(id_node);
        return (ASTNode*)create_error_node();
    }
    return id_node;
}

// <IfStatement> ::= if ( <Expression> ) <CompoundStatement> [ else <CompoundStatement> ]
ASTNode* if_statement() 
{
    fprintf(log_file, "Parsing <IfStatement> (current token: %s)\n", token_type_to_string(token));
    match(IF);

    if(!match(LPAREN)) 
    {
        int follow_set[] = {IDENTIFIER, NUMBER, LPAREN, RBRACE, LBRACE}; // First of Expr or LBRACE for body
        char error_msg[200];
        sprintf(error_msg, "Expected \'(\' after \'if\', got %s", token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "if_statement_missing_lparen");
        if (token != IDENTIFIER && token != NUMBER && token != LPAREN && token != LBRACE) return (ASTNode*)create_error_node();
        if (token == LPAREN) match(LPAREN); else return (ASTNode*)create_error_node();
    }
    
    ASTNode* cond_node = expression();

    if(!match(RPAREN)) 
    {
        int follow_set[] = {LBRACE}; // Expect body next
        char error_msg[200];
        sprintf(error_msg, "Expected \')\' after \'if\' condition, got %s", token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "if_statement_missing_rparen_cond");
        if (token != LBRACE) { free_ast(cond_node); return (ASTNode*)create_error_node(); }
         // If recovery finds LBRACE, we assume RPAREN was implicitly there or skipped.
    }
    ASTNode* then_body_node = compound_statement();

    ASTNode* else_body_node = NULL;
    if (token == ELSE) 
    {
        match(ELSE);
        else_body_node = compound_statement();
    }
    return (ASTNode*)create_if_statement_node(cond_node, then_body_node, else_body_node);
}

// <WhileStatement> ::= while ( <Expression> ) <CompoundStatement>
ASTNode* while_statement()
{
    fprintf(log_file, "Parsing <WhileStatement> (current token: %s)\n", token_type_to_string(token));
    match(WHILE);

    if(!match(LPAREN)) 
    {
        int follow_set[] = {IDENTIFIER, NUMBER, LPAREN, RBRACE, LBRACE};
        char error_msg[200];
        sprintf(error_msg, "Expected \'(\' after \'while\', got %s", token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "while_statement_missing_lparen");
        if (token != IDENTIFIER && token != NUMBER && token != LPAREN && token != LBRACE) return (ASTNode*)create_error_node();
        if (token == LPAREN) match(LPAREN); else return (ASTNode*)create_error_node();
    }

    ASTNode* cond_node = expression();

    if(!match(RPAREN)) 
    {
        int follow_set[] = {LBRACE};
        char error_msg[200];
        sprintf(error_msg, "Expected \')\' after expression in \'while\', got %s", token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "while_statement_missing_rparen");
        if (token != LBRACE) { free_ast(cond_node); return (ASTNode*)create_error_node(); }
    }

    ASTNode* body_node = compound_statement();

    return (ASTNode*)create_while_statement_node(cond_node, body_node);
}

ASTNode* return_statement() 
{
    fprintf(log_file, "Parsing <ReturnStatement> (current token: %s)\n", token_type_to_string(token));
    match(RETURN);
    
    ASTNode* expr_node = NULL;

    // Optional expression: if next token is not SEMI, try to parse an expression
    if (token != SEMI && token != RBRACE && token != EOF) 
    {
        expr_node = expression();
    }

    return (ASTNode*)create_return_statement_node(expr_node);
}

// --- Main Driver ---
ParseResult parse_source_file(const char* source_filename)
{
    ParseResult result = {0, NULL, NULL}; // Initialize result

    log_file = fopen("parser_logs.txt", "w");
    if (!log_file) {
        perror("Failed to open log file");
        result.error_count = -1; // Indicate fatal error
        return result;
    }

    if (source_filename != NULL && source_filename[0] != '\0') 
    {
        yyin = fopen(source_filename, "r");
        if (!yyin) 
        {
            perror(source_filename);
            fclose(log_file);
            result.error_count = -1; // Indicate fatal error
            return result;
        }
        yyfilename = (char*)source_filename; // Keep const char* for filename
    } 
    else 
    {
        fprintf(stderr, "No input file specified. Reading from stdin (not fully supported for all features yet).\\n");
        yyin = stdin;
        yyfilename = "stdin";
    }

    // Initialize symbol table
    global_sym_table = create_symbol_table();
    if (!global_sym_table) 
    {
        perror("FATAL: Could not create global symbol table!");
        if (yyin != stdin) fclose(yyin);
        fclose(log_file);
        result.error_count = -1; // Indicate fatal error
        return result;
    }
    global_sym_table->parent = NULL;
    global_sym_table->depth = 0;
    current_sym_table = global_sym_table;
    fprintf(log_file, "Initialized global scope. Depth: %d, Addr: %p\n",
            current_sym_table->depth, (void*)current_sym_table);

    // Reset error count for this parse run (if it's global and shared with lexer)
    // error_count = 0; // Assuming error_count is reset by lexer or should be reset here.
                     // The provided code shows error_count in scanner.l, so it might persist.
                     // For now, let's assume it's reset or handled by the lexer for each new yyparse like call.
                     // If not, it should be reset: error_count = 0; and parser's internal current_error_count = 0;

    get_next_token();
    ast_root = program(); // This will set the global ast_root

    if (token == EOF && error_count == 0) 
    {
        fprintf(log_file, "Parsing reached EOF as expected.\n");
    } 
    else if (token != EOF) 
    {
        char error_msg[200];
        sprintf(error_msg, "Unexpected token %s at end of input instead of EOF",
                token_type_to_string(token));
        // Using parser's save_error_pos if it's the one that increments global error_count
        // or if a unified error reporting is desired.
        // For now, assuming save_error_pos in parser.c updates the global error_count.
        save_error_pos("syntax error", error_msg);
        fprintf(log_file, "Error: %s\\n", error_msg);
    }

    result.error_count = error_count;
    result.ast_root = ast_root;
    result.global_sym_table = global_sym_table;

    // IMPORTANT: AST root and global symbol table are NOT freed here.
    // The caller (driver.c) is responsible for freeing them.

    if (log_file) 
    {
        fclose(log_file);
        log_file = NULL;
    }
    if (yyin != stdin && yyin != NULL) 
    {
        fclose(yyin);
        yyin = NULL;
    }
    
    // Reset globals for potential re-entry if the parser were part of a library
    // though for a compiler main, this is less critical.
    // ast_root = NULL; // Driver has a copy
    // global_sym_table = NULL; // Driver has a copy
    // current_sym_table = NULL;

    return result;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"

// Forward declarations for recursive functions
void program();
void external_declaration();
void declarations();
void function_definition();
void variable_declaration_global();
void parameter_list_opt();
void parameter_list();
void parameter_declaration();
void block();
void compound_statement();
void statement_list();
void statement();
void assign_or_func_call();
void assignment_statement();
void function_call_statement();
void argument_list_opt();
void argument_list();
void declare_statement();
void init_declarator();
void expression();
void expression_prime();
void arithmetic_expression();
void arithmetic_prime();
void relational_prime();
void term();
void term_prime();
void factor();
void epsilon_or_func_call();
void type();
void if_statement();
void while_statement();
void return_statement();

FILE *log_file; // Log file for parser output

extern FILE *yyin;   // Input stream
extern char* yyfilename;
extern int yylex();
extern void save_error_pos(char* type, char* token);
extern int error_count;
extern int lineno;
extern void show_and_free_errors();

YYSTYPE yylval;
int token;

int is_in_follow_set(int* follow_set, int size);
void error_recovery(int* follow_set, int size, const char* production_name);
char* token_type_to_string(int token_type);

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
void error_recovery(int* follow_set, int size, const char* production_name) {
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
void program() 
{
    fprintf(log_file, "Parsing <Program>\n");

    while (1) { // First of <ExternalDeclaration> is First of <Type>
        external_declaration();
        if(token != INT && token != CHAR && token != VOID && token != EOF) 
        {
            int follow_set[] = {INT, CHAR, VOID, EOF}; // Follow set for <Program>
            char error_msg[200];
            sprintf(error_msg, "Unexpected token %s after external declaration",
                    token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "program");
        }
        if(token == EOF) break;
    }

}

// <ExternalDeclaration> ::= <Type> <Identifier> <Declarations>
void external_declaration() 
{
    fprintf(log_file, "Parsing <ExternalDeclaration>\n");
    type();
    if(!match(IDENTIFIER))
    {
        int follow_set[] = {INT, CHAR, VOID, EOF}; // Follow set for <ExternalDeclaration>
        char error_msg[200];
        sprintf(error_msg, "Expected identifier after type, got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, 4, "external_declaration");
        return; // Early return on error
    }
    declarations();
}

// <Declarations> ::= <FunctionDefinition> | <VariableDeclarationGlobal> ;
void declarations() 
{
    fprintf(log_file, "Parsing <Declarations>\n");
    if (token == LPAREN) { // First of <FunctionDefinition>
        function_definition();
        return;
    }
    if (token == ASSIGN || token == COMMA || token == SEMI) { // First of <VariableDeclarationGlobal> or Follow if epsilon
        variable_declaration_global();
        if(!match(SEMI))
        {
            int follow_set[] = {INT, CHAR, VOID, EOF}; // Follow set for <Declarations>
            char error_msg[200];
            sprintf(error_msg, "Expected ';' after variable declaration, got %s",
                    token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(follow_set, 4, "declarations"); 
        }
        return;
    } 
}

// <VariableDeclarationGlobal> ::= [ = <Expression> ] { , <Identifier> [ = <Expression> ] }
void variable_declaration_global() 
{
    fprintf(log_file, "Parsing <VariableDeclarationGlobal>\n");
    if (token == ASSIGN) {
        match(ASSIGN);
        expression();
    }
    while (token == COMMA) {
        match(COMMA);
        if(!match(IDENTIFIER))
        {
            int follow_set[] = {SEMI}; // Follow set for <VariableDeclarationGlobal>
            char error_msg[200];
            sprintf(error_msg, "Expected identifier after ',', got %s",
                    token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(follow_set, 1, "variable_declaration_global");
            return;

        }
        if (token == ASSIGN) 
        {
            match(ASSIGN);
            expression();
        }
    }
}

// <FunctionDefinition> ::= ( <ParameterListOpt> ) <CompoundStatement>
void function_definition() 
{
    fprintf(log_file, "Parsing <FunctionDefinition>\n");
    match(LPAREN);
    parameter_list_opt();
    if(!match(RPAREN))
    {
        int follow_set[] = {INT, CHAR, VOID, EOF}; // Follow set for <FunctionDefinition>
        char error_msg[200];
        sprintf(error_msg, "Expected ')' after parameter list, got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, 4, "function_definition");
        return;
    }
    compound_statement();
}

// <ParameterListOpt> ::= <ParameterList> | epsilon
void parameter_list_opt() 
{
    fprintf(log_file, "Parsing <ParameterListOpt>\n");

    if (token == INT || token == CHAR || token == VOID) {
        parameter_list();
        return;
    }

}

// <ParameterList> ::= <ParameterDeclaration> { , <ParameterDeclaration> }
void parameter_list() 
{
    fprintf(log_file, "Parsing <ParameterList>\n");
    parameter_declaration();
    while (token == COMMA) {
        match(COMMA);
        parameter_declaration();
    }
}

// <ParameterDeclaration> ::= <Type> <Identifier>
void parameter_declaration() 
{
    fprintf(log_file, "Parsing <ParameterDeclaration>\n");
    type();
    if(!match(IDENTIFIER))
    {
        int follow_set[] = {COMMA, RPAREN}; // Follow set for <ParameterDeclaration>
        char error_msg[200];
        sprintf(error_msg, "Expected identifier after type in parameter declaration, got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, 2, "parameter_declaration");
        return;
    }
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
void compound_statement() {
    fprintf(log_file, "Parsing <CompoundStatement>. Current token: %s\n", token_type_to_string(token));
    if(!match(LBRACE))
    {
        int follow_set[] = {EOF, IDENTIFIER, LBRACE, INT, CHAR, VOID, IF, WHILE, RBRACE, ELSE};
        char error_msg[200];
        sprintf(error_msg, "Expected \'{\' to start compound statement, got %s", token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "compound_statement_missing_lbrace");
        // It might be hard to continue meaningfully if LBRACE is missing, but recovery will try.
        if (token != LBRACE) return; // If recovery didn't find LBRACE, abort this compound_statement.
    }
    statement_list();
    if(!match(RBRACE))
    {
        int follow_set[] = {EOF, IDENTIFIER, LBRACE, INT, CHAR, VOID, IF, WHILE, RBRACE, ELSE}; // Follow set for <CompoundStatement>
        char error_msg[200];
        sprintf(error_msg, "Expected \'}\' at end of compound statement, got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "compound_statement_missing_rbrace");
        // No return here, error_recovery will position for the next token.
    }
    fprintf(log_file, "Exiting <CompoundStatement>. Current token: %s\n", token_type_to_string(token));
}


// <StatementList> ::= { <Statement> }
void statement_list() 
{
    fprintf(log_file, "Parsing <StatementList>. Current token: %s\n", token_type_to_string(token));
    while(token != RBRACE && token != EOF) 
    {
        int initial_error_count = error_count;
        int initial_token = token;
        int initial_lineno = yylval.sval ? lineno : -1; // Keep track of line for loop detection

        statement();

        // Failsafe: if statement() didn't advance token and didn't report an error,
        // and we are not at RBRACE or EOF, we might be in an infinite loop.
        if (token == initial_token &&
            (yylval.sval ? lineno : -1) == initial_lineno && // Check if line number also hasn't changed
            error_count == initial_error_count &&
            token != RBRACE && token != EOF) {
            fprintf(log_file, "Internal Parser Error: statement() did not advance token (%s) or report error in statement_list. Forcing advance.\n", token_type_to_string(token));
            save_error_pos("internal parser error", "Statement parser stuck, forcing advance.");
            get_next_token(); // Force advance
        }
    }
    fprintf(log_file, "Exiting <StatementList>. Next token for compound_statement: %s\n", token_type_to_string(token));
}


// <Statement> ::= | <CompoundStatement>
//                | <DeclareStatement> ;
//                | <Identifier> <AssignOrFuncCall> ;
//                | <IfStatement>
//                | <WhileStatement>
//                | epsilon
void statement() 
{
    fprintf(log_file, "Parsing <Statement> (current token: %s, line: %d)\n", token_type_to_string(token), yylval.sval ? lineno : -1);

    int stmt_follow_set[] = {IDENTIFIER, LBRACE, INT, CHAR, VOID, IF, WHILE, RBRACE, ELSE, EOF};
    int stmt_follow_set_size = sizeof(stmt_follow_set)/sizeof(stmt_follow_set[0]);

    if (token == INT || token == CHAR) { // First of <DeclareStatement>
        declare_statement();
        if(!match(SEMI)) {
            char error_msg[200];
            sprintf(error_msg, "Expected \';\' after declaration statement, got %s", token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(stmt_follow_set, stmt_follow_set_size, "statement_after_declare");
        }
        return;
    }
    if (token == LBRACE) { // First of <CompoundStatement>
        compound_statement();
        return;
    }
    if(token == IDENTIFIER) { // First of <Identifier> <AssignOrFuncCall>
        match(IDENTIFIER);
        assign_or_func_call();
        if(!match(SEMI)) {
            char error_msg[200];
            sprintf(error_msg, "Expected \';\' after assignment or function call, got %s", token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(stmt_follow_set, stmt_follow_set_size, "statement_after_assign_or_call");
        }
        return;
    }
    if(token == IF) { // First of <IfStatement>
        if_statement();
        return;
    }
    if(token == WHILE) { // First of <WhileStatement>
        while_statement();
        return;
    }
    if(token == RETURN) {
        return_statement();
        if(!match(SEMI)) {
            char error_msg[200];
            sprintf(error_msg, "Expected \';\' after return statement, got %s", token_type_to_string(token));
            save_error_pos("syntax error", error_msg);
            error_recovery(stmt_follow_set, stmt_follow_set_size, "statement_after_return");
        }
        return;
    }

    // Special case: handle 'else' without 'if'
    if(token == ELSE) {
        char error_msg[200];
        sprintf(error_msg, "\'else\' without matching \'if\'");
        save_error_pos("syntax error", error_msg);
        get_next_token(); // Consume 'else'

        if (token == LBRACE) { // If 'else' is followed by a block, skip the block
            int brace_count = 1;
            get_next_token(); // Consume LBRACE
            while (token != EOF && brace_count > 0) { // Added EOF check
                if (token == LBRACE) brace_count++;
                else if (token == RBRACE) brace_count--;
                get_next_token();
            }
        }
        return;
    }

    // Epsilon production for <Statement> or start of next valid statement
    // If current token is in FOLLOW(<Statement>) or can start a new statement.
    if (is_in_follow_set(stmt_follow_set, stmt_follow_set_size)) {
        // This means an empty statement is valid here, or we are at a token
        // that can validly follow a statement (like RBRACE or start of next statement).
        fprintf(log_file, "Epsilon production for <Statement> or at valid follow token: %s\n", token_type_to_string(token));
        return;  
    }

    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in statement. Cannot start a new statement.", token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
    // Attempt recovery by skipping to a token that can start a new statement or end the current block.
    error_recovery(stmt_follow_set, stmt_follow_set_size, "statement_unexpected_token");
    // After error_recovery, the token should be one from the follow_set, or EOF.
    // The loop in statement_list will then re-evaluate with the new token.
}

// <AssignmentStatement> ::= = <Expression>
void assignment_statement() 
{
    fprintf(log_file, "Parsing <AssignmentStatement>\n");
    match(ASSIGN);
    expression();
}

// <AssignOrFuncCall> ::= = <AssignmentStatement> | <FunctionCallStatement> 
void assign_or_func_call() 
{
    fprintf(log_file, "Parsing <AssignOrFuncCall> (current token: %s)\n", token_type_to_string(token));
    
    // Check for assignment: = <Expression>
    if (token == ASSIGN) {
        assignment_statement();
        return;
    }

    // Check for function call: ( <ArgumentListOpt> )
    if (token == LPAREN) {
        function_call_statement();
        return;
    }
}

// <FunctionCallStatement> ::= <Identifier> ( <ArgumentListOpt> ) ;
void function_call_statement() 
{
    fprintf(log_file, "Parsing <FunctionCallStatement> (current token: %s)\n", token_type_to_string(token));
    match(LPAREN);
    argument_list_opt();
    if(!match(RPAREN))
    {
        int follow_set[] = {MUL, DIV, PLUS, MINUS, RPAREN, SEMI, LT, LE, GT, GE, EQ, NE, COMMA}; // Follow set for <FunctionCallStatement>
        char error_msg[200];
        sprintf(error_msg, "Expected \')\' after function call arguments, got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "function_call_statement");
        return;
    }
}

// <ArgumentListOpt> ::= <ArgumentList> | epsilon
void argument_list_opt() 
{
    fprintf(log_file, "Parsing <ArgumentListOpt> (current token: %s)\n", token_type_to_string(token));

    if (token == IDENTIFIER || token == NUMBER || token == LPAREN) {
        argument_list();
        return;
    }
}

// <ArgumentList> ::= <Expression> { , <Expression> }
void argument_list() 
{
    fprintf(log_file, "Parsing <ArgumentList> (current token: %s)\n", token_type_to_string(token));
    expression();
    while (token == COMMA) 
    {
        match(COMMA);
        expression();
    }
}

// <DeclareStatement> ::= <Type> <InitDeclarator> { , <InitDeclarator> }
void declare_statement() 
{
    fprintf(log_file, "Parsing <DeclareStatment> (current token: %s)\n", token_type_to_string(token));
    type();
    init_declarator();

    while(token == COMMA) 
    {
        match(COMMA);
        init_declarator();
    }
}

// <InitDeclarator> ::= <Identifier> [ = <Expression> ]
void init_declarator() 
{
    fprintf(log_file, "Parsing <InitDeclarator> (current token: %s)\n", token_type_to_string(token));
    if(!match(IDENTIFIER))
    {
        int follow_set[] = {COMMA, SEMI}; // Follow set for <InitDeclarator>
        char error_msg[200];
        sprintf(error_msg, "Expected identifier in initializer, got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "init_declarator");
        return;
    }

    if (token == ASSIGN) 
    {
        match(ASSIGN);
        expression();
        return;
    } 
    if(token == COMMA || token == SEMI) return;

    int follow_set[] = {COMMA, SEMI}; // Follow set for <InitDeclarator>
    char error_msg[200];
    sprintf(error_msg, "Expected \'=\' or \',\' or \';\' after identifier, got %s",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
    error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "init_declarator");
}

// <Type> ::= int | char
void type() 
{
    fprintf(log_file, "Parsing <Type> (current token: %s)\n",  token_type_to_string(token));
    if (token == INT) 
    {
        match(INT);
        return;
    } 
    if (token == CHAR) 
    {
        match(CHAR);
        return;
    } 
    if (token == VOID) // Added VOID
    {
        match(VOID);
        return;
    }
}

// <Expression> ::= <ArithmeticExpression> <RelationalPrime>
void expression() 
{
    fprintf(log_file, "Parsing <Expression> (current token: %s)\n", token_type_to_string(token));
    arithmetic_expression();
    relational_prime();
}

// <ArithmeticExpression> ::= <Term> <ArithmeticPrime>
void arithmetic_expression() 
{
    fprintf(log_file, "Parsing <ArithmeticExpression> (current token: %s)\n", token_type_to_string(token));
    term();
    arithmetic_prime();
}

// <ArithmeticPrime> ::= + <Term> <ArithmeticPrime>
//                     | - <Term> <ArithmeticPrime>
//                     | epsilon
void arithmetic_prime() 
{
    fprintf(log_file, "Parsing <ArithmeticPrime> (current token: %s)\n", token_type_to_string(token));
    if (token == PLUS) {
        match(PLUS);
        term();
        arithmetic_prime();
        return;
    } 
    if (token == MINUS) {
        match(MINUS);
        term();
        arithmetic_prime();
        return;
    }

    // else: Epsilon production. Do nothing.
    if(token == RPAREN || token == SEMI || token == LE || token == GE || 
        token == LT || token == GT || token == EQ || token == NE || token == COMMA) return;

    int follow_set[] = {RPAREN, SEMI, LE, GE, LT, GT, EQ, NE, COMMA}; // Follow set for <ArithmeticPrime>
    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in expression prime",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
    error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "arithmetic_prime");
}

// <RelationalPrime> ::= == <ArithmeticExpression> <RelationalPrime>
//                     | != <ArithmeticExpression> <RelationalPrime>
//                     | < <ArithmeticExpression> <RelationalPrime>
//                     | > <ArithmeticExpression> <RelationalPrime>
//                     | <= <ArithmeticExpression> <RelationalPrime>
//                     | >= <ArithmeticExpression> <RelationalPrime>
//                     | epsilon
void relational_prime() 
{
    fprintf(log_file, "Parsing <RelationalPrime> (current token: %s)\n", token_type_to_string(token));
    if (token == EQ) {
        match(EQ);
        arithmetic_expression();
        relational_prime();
        return;
    } 
    if (token == NE) {
        match(NE);
        arithmetic_expression();
        relational_prime();
        return;
    } 
    if (token == LT) {
        match(LT);
        arithmetic_expression();
        relational_prime();
        return;
    } 
    if (token == GT) {
        match(GT);
        arithmetic_expression();
        relational_prime();
        return;
    } 
    if (token == LE) {
        match(LE);
        arithmetic_expression();
        relational_prime();
        return;
    } 
    if (token == GE) {
        match(GE);
        arithmetic_expression();
        relational_prime();
        return;
    }

    // else: Epsilon production. Do nothing.
    if(token == RPAREN || token == SEMI || token == COMMA) {
        return;
    }

    int follow_set[] = {RPAREN, SEMI, COMMA}; // Follow set for <RelationalPrime>
    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in relational prime",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
    error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "relational_prime");
}

// <Term> ::= <Factor> <TermPrime>
void term() 
{
    fprintf(log_file, "Parsing <Term>\n");
    factor();
    term_prime();
}

// <TermPrime> ::= * <Factor> <TermPrime>  (Corrected from TermPrmie)
//               | / <Factor> <TermPrime>
//               | epsilon
void term_prime() 
{
    fprintf(log_file, "Parsing <TermPrime> (current token: %s)\n", token_type_to_string(token));
    if (token == MUL) {
        match(MUL);
        factor();
        term_prime();
        return;
    }
    if (token == DIV) {
        match(DIV);
        factor();
        term_prime();
        return;
    }

    // else: Epsilon production. Do nothing.
    if(token == PLUS || token == MINUS || token == RPAREN || token == SEMI || token == LE || token == GE || 
        token == LT || token == GT || token == EQ || token == NE || token == COMMA) return;

    int follow_set[] = {PLUS, MINUS, RPAREN, SEMI, LE, GE, LT, GT, EQ, NE, COMMA}; // Follow set for <TermPrime>
    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in term prime",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
    error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "term_prime");
}

// <Factor> ::= <Identifier> <EpsilonOrFuncCall> | <Number> | ( <Expression> )
void factor() 
{
    fprintf(log_file, "Parsing <Factor> (current token: %s)\n", token_type_to_string(token));
    if (token == IDENTIFIER) 
    {
        match(IDENTIFIER);
        epsilon_or_func_call();
        return;
    } 
    if (token == NUMBER) 
    {
        match(NUMBER);
        return;
    } 
    if (token == LPAREN) {
        match(LPAREN);
        expression();
        match(RPAREN);
        return;
    } 

    int follow_set[] = {PLUS, MINUS, MUL, DIV, RPAREN, SEMI, LE, GE, LT, GT, EQ, NE, COMMA}; // Follow set for <Factor>
    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in factor",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
    error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "factor");
}

// <EpsilonOrFuncCall> ::= epsilon | <FunctionCallStatement>
void epsilon_or_func_call() 
{
    fprintf(log_file, "Parsing <EpsilonOrFuncCall> (current token: %s)\n", token_type_to_string(token));
    
    if (token == LPAREN) {
        function_call_statement();
        return;
    }
    
    // Epsilon production - do nothing if not a function call
    // Follow set includes operators, semicolons, parentheses, etc.
    if (token == MUL || token == DIV || token == PLUS || token == MINUS ||
        token == LT || token == LE || token == GT || token == GE ||
        token == EQ || token == NE || token == RPAREN || token == SEMI ||
        token == COMMA) {
        return;
    }

    int follow_set[] = {MUL, DIV, PLUS, MINUS, RPAREN, SEMI, LT, LE, GT, GE, EQ, NE, COMMA}; // Follow set for <EpsilonOrFuncCall>
    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s after identifier in factor",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
    error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "epsilon_or_func_call");
}

// <IfStatement> ::= if ( <Expression> ) <CompoundStatement> [ else <CompoundStatement> ]
void if_statement() 
{
    fprintf(log_file, "Parsing <IfStatement> (current token: %s)\n", token_type_to_string(token));
    match(IF);
    if(!match(LPAREN))
    {
        int follow_set[] = {IDENTIFIER, LBRACE, INT, CHAR, VOID, IF, WHILE, RBRACE}; // Follow set for <IfStatement>
        char error_msg[200];
        sprintf(error_msg, "Expected \'(\' after \'if\', got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "if_statement_missing_lparen");
        return;
    }
    expression();
    if(!match(RPAREN))
    {
        int follow_set[] = {IDENTIFIER, LBRACE, INT, CHAR, VOID, IF, WHILE, RBRACE}; // Follow set for <IfStatement>
        char error_msg[200];
        sprintf(error_msg, "Expected \'(\' after \'if\', got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "if_statement_missing_rparen");
        return;
    }
    compound_statement();
    if (token == ELSE) {
        match(ELSE);
        compound_statement();
    }
}

// <WhileStatement> ::= while ( <Expression> ) <CompoundStatement>
void while_statement()
{
    fprintf(log_file, "Parsing <WhileStatement> (current token: %s)\n", token_type_to_string(token));
    match(WHILE);
    if(!match(LPAREN))
    {
        int follow_set[] = {IDENTIFIER, LBRACE, INT, CHAR, VOID, IF, WHILE, RBRACE}; // Follow set for <WhileStatement>
        char error_msg[200];
        sprintf(error_msg, "Expected \'(\' after \'while\', got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "while_statement_missing_lparen");
        return;
    }
    expression();
    if(!match(RPAREN))
    {
        int follow_set[] = {IDENTIFIER, LBRACE, INT, CHAR, VOID, IF, WHILE, RBRACE}; // Follow set for <WhileStatement>
        char error_msg[200];
        sprintf(error_msg, "Expected \')\' after expression in \'while\', got %s",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
        error_recovery(follow_set, sizeof(follow_set)/sizeof(follow_set[0]), "while_statement_missing_rparen");
        return;
    }
    compound_statement();
}

void return_statement() 
{
    fprintf(log_file, "Parsing <ReturnStatement> (current token: %s)\n", token_type_to_string(token));
    match(RETURN);
    
    // Optional expression
    if (token != SEMI && token != LBRACE && token != EOF) { // TODO : TBD
        expression();
    }
}

// --- Main Driver ---
void parse() 
{

    get_next_token();
    program();

    if (token == EOF) 
    {
        printf("Parsing reached EOF as expected.\n");
        return;
    } 
    else 
    {
        char error_msg[200];
        sprintf(error_msg, "Unexpected token %s at end of input instead of EOF",
                token_type_to_string(token));
        save_error_pos("syntax error", error_msg);
    }
}

int main(int argc, char **argv) 
{

    log_file = fopen("parser_logs.txt", "w");

    if(!log_file) 
    {
        perror("Failed to open log file");
        return 1;
    }

    if (argc > 1) 
    {
        yyin = fopen(argv[1], "r");
        yyfilename = argv[1];
        if (!yyin) 
        {
            perror(argv[1]);
            return 1;
        }
    } 
    else 
    {
        printf("No input file specified. Reading from stdin.\n");
        yyin = stdin;
    }

    parse();

    if(error_count > 0) 
    {
        printf("Parsing completed with %d errors.\n", error_count);
        show_and_free_errors();
    } 
    else 
    {
        printf("Parsing completed successfully.\n");
    }

    return 0;
}
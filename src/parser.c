#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"

// Forward declarations for recursive functions
void program();
void external_declaration(); // New
void declarations(); // New
void function_definition(); // New
void variable_declaration_global(); // New
void parameter_list_opt(); // New
void parameter_list(); // New
void parameter_declaration(); // New
void block();
void compound_statement();
void statement_list();
void statement();
void assignment_statement();
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
void type();
void if_statement();
void while_statement();

extern FILE *yyin;   // Input stream
extern char* yyfilename;
extern int yylex();
extern void save_error_pos(char* type, char* token);
extern int error_count;
extern void show_and_free_errors();

YYSTYPE yylval;
int token;

char* token_type_to_string(int token_type) {
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
        case MAIN: return "MAIN";
        case LE: return "LE";
        case GE: return "GE";
        case EQ: return "EQ";
        case NE: return "NE";
        case LT: return "LT";
        case GT: return "GT";
        case COMMA: return "COMMA";
        case 0: return "EOF";
        default: 
            return "UNKNOWN";
    }
}

void get_next_token() {
    token = yylex(); // Get the next token from the lexer
}

// Match and consume a token
void match(int expected_type) {
    printf("Matching token: %s (expected: %s)\n", 
           token_type_to_string(token), 
           token_type_to_string(expected_type));
    if (token == expected_type) 
    {
        get_next_token();
        return;
    }
    char error_msg[200];
    sprintf(error_msg, "Expected token %s but got %s",
            token_type_to_string(expected_type),
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
}

// <Program> ::= <ExternalDeclaration> { <ExternalDeclaration> }
void program() {
    printf("Parsing <Program>\n");
    // Loop to handle one or more external declarations
    while (token == INT || token == CHAR || token == VOID) { // First of <ExternalDeclaration> is First of <Type>
        external_declaration();
    }
    // After all external declarations, we expect EOF
    if (token != 0) { // 0 is typically EOF
        save_error_pos("syntax error", "Expected EOF after program");
    }
}

// <ExternalDeclaration> ::= <Type> <Identifier> <Declarations>
void external_declaration() {
    printf("Parsing <ExternalDeclaration>\n");
    type();
    match(IDENTIFIER);
    declarations();
}

// <Declarations> ::= <FunctionDefinition> | <VariableDeclarationGlobal> ;
void declarations() {
    printf("Parsing <Declarations>\n");
    if (token == LPAREN) { // First of <FunctionDefinition>
        function_definition();
        return;
    }
    if (token == ASSIGN || token == COMMA || token == SEMI) { // First of <VariableDeclarationGlobal> or Follow if epsilon
        variable_declaration_global();
        match(SEMI);
        return;
    } 

    // TODO : error handling
}

// <VariableDeclarationGlobal> ::= [ = <Expression> ] { , <Identifier> [ = <Expression> ] }
void variable_declaration_global() {
    printf("Parsing <VariableDeclarationGlobal>\n");
    if (token == ASSIGN) {
        match(ASSIGN);
        expression();
    }
    while (token == COMMA) {
        match(COMMA);
        match(IDENTIFIER);
        if (token == ASSIGN) {
            match(ASSIGN);
            expression();
        }
    }
}

// <FunctionDefinition> ::= ( <ParameterListOpt> ) <CompoundStatement>
void function_definition() {
    printf("Parsing <FunctionDefinition>\n");
    match(LPAREN);
    parameter_list_opt();
    match(RPAREN);
    compound_statement();
}

// <ParameterListOpt> ::= <ParameterList> | epsilon
void parameter_list_opt() {
    printf("Parsing <ParameterListOpt>\n");

    if (token == INT || token == CHAR || token == VOID) {
        parameter_list();
        return;
    }
    // Epsilon production. Follow is RPAREN, already handled by caller or next match.
    if(token == RPAREN) return;

    // TODO : error handling
}

// <ParameterList> ::= <ParameterDeclaration> { , <ParameterDeclaration> }
void parameter_list() {
    printf("Parsing <ParameterList>\n");
    parameter_declaration();
    while (token == COMMA) {
        match(COMMA);
        parameter_declaration();
    }
}

// <ParameterDeclaration> ::= <Type> <Identifier>
void parameter_declaration() {
    printf("Parsing <ParameterDeclaration>\n");
    type();
    match(IDENTIFIER);
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
    printf("Parsing <CompoundStatement>\n");
    match(LBRACE);
    statement_list();
    match(RBRACE);
}


// <StatementList> ::= <Statement> <StatementList> | epsilon
// This is handled by the loop in compound_statement_parser.
// For a standalone statement_list_parser (if needed elsewhere), it would be:
void statement_list() {
    printf("Parsing <StatementList>\n");
    // First of <Statement>: IDENTIFIER, LBRACE, INT, CHAR, IF, WHILE
    // Follow of <StatementList> is RBRACE (})
    while (token == IDENTIFIER || token == LBRACE || token == INT || token == CHAR || token == IF || token == WHILE) {
        statement();
    }
    // Epsilon is handled if the loop condition is not met and token is RBRACE (Follow of StatementList)
    if (token == RBRACE) return; 

    // If it's not a valid start of a statement and not the end of the list, it's an error.
    // However, error reporting for unexpected tokens in statement_list might be better handled
    // if statement() itself reports an error when it cannot parse anything and doesn't find an epsilon.
    // For now, let's assume statement() handles its own errors or consumes tokens.
}


// <Statement> ::= <AssignmentStatement> ;
//               | <CompoundStatement>
//               | <DeclareStatment> ;
//               | <IfStatement>
//               | <WhileStatement>;
//               | epsilon
void statement() {
    printf("Parsing <Statement> (current token: %s)\n", token_type_to_string(token));
    // Lookahead for specific statement types
    if (token == INT || token == CHAR) { // First of <DeclareStatement>
        declare_statement();
        match(SEMI);
        return;
    } 
    if (token == LBRACE) { // First of <CompoundStatement>
        compound_statement(); // CompoundStatement in EBNF for statement does not have a trailing semicolon
        return;
    }
    if(token == IDENTIFIER) { // First of <AssignmentStatement>
        // Need to distinguish between assignment and function call if we add function calls
        assignment_statement();
        match(SEMI);
        return;
    }
    if(token == IF) { // First of <IfStatement>
        if_statement(); // IfStatement in EBNF does not have a trailing semicolon
        return;
    }
    if(token == WHILE) { // First of <WhileStatement>
        while_statement(); // WhileStatement in EBNF does not have a trailing semicolon
        return;
    }
    
    // Epsilon production for <Statement>
    // Follow(<Statement>) = { IDENTIFIER, LBRACE, INT, CHAR, IF, WHILE, RBRACE }
    // If current token is in Follow(<Statement>), it means an empty statement (epsilon) is valid.
    if (token == RBRACE || // End of a compound statement
        token == IDENTIFIER || // Start of next statement
        token == LBRACE ||     // Start of next compound statement
        token == INT || token == CHAR || // Start of next declaration statement
        token == IF || token == WHILE) { // Start of next control flow statement
        return;
    }

    // If no rule matches and it's not a valid epsilon case based on Follow set.
    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in statement",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
    // Potentially advance token to attempt recovery, or let caller handle.
    // For now, error is saved, and parsing might halt or behave unpredictably.
}

// <AssignmentStatement> ::= <Identifier> = <Expression>
void assignment_statement() {
    printf("Parsing <AssignmentStatement>\n");
    match(IDENTIFIER);
    match(ASSIGN);
    expression();
}

// <DeclareStatement> ::= <Type> <InitDeclarator> { , <InitDeclarator> }
void declare_statement() {
    printf("Parsing <DeclareStatment>\n");
    type();
    init_declarator();

    while(token == COMMA) {
        match(COMMA);
        init_declarator();
    }
}

// <InitDeclarator> ::= <Identifier> [ = <Expression> ]
void init_declarator() {
    printf("Parsing <InitDeclarator>\n");
    match(IDENTIFIER);
    if (token == ASSIGN) 
    {
        match(ASSIGN);
        expression();
        return;
    } 
    if(token == COMMA || token == SEMI) return;

    char error_msg[200];
    sprintf(error_msg, "Expected '=' or ',' or ';' after identifier, got %s",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
}

// <Type> ::= int | char
void type() {
    printf("Parsing <Type>\n");
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

    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in type declaration",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
}

// <Expression> ::= <ArithmeticExpression> <RelationalPrime>
void expression() {
    printf("Parsing <Expression>\n");
    arithmetic_expression();
    relational_prime();
}

// <ArithmeticExpression> ::= <Term> <ArithmeticPrime>
void arithmetic_expression() {
    printf("Parsing <ArithmeticExpression>\n");
    term();
    arithmetic_prime();
}

// <ArithmeticPrime> ::= + <Term> <ArithmeticPrime>
//                     | - <Term> <ArithmeticPrime>
//                     | epsilon
void arithmetic_prime() {
    printf("Parsing <ArithmeticPrime> (current token: %s)\n", token_type_to_string(token));
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

    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in expression prime",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
}

// <RelationalPrime> ::= == <ArithmeticExpression> <RelationalPrime>
//                     | != <ArithmeticExpression> <RelationalPrime>
//                     | < <ArithmeticExpression> <RelationalPrime>
//                     | > <ArithmeticExpression> <RelationalPrime>
//                     | <= <ArithmeticExpression> <RelationalPrime>
//                     | >= <ArithmeticExpression> <RelationalPrime>
//                     | epsilon
void relational_prime() {
    printf("Parsing <RelationalPrime> (current token: %s)\n", token_type_to_string(token));
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

    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in relational prime",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
}

// <Term> ::= <Factor> <TermPrime>
void term() {
    printf("Parsing <Term>\n");
    factor();
    term_prime(); // Corrected from TermPrmie
}

// <TermPrime> ::= * <Factor> <TermPrime>  (Corrected from TermPrmie)
//               | / <Factor> <TermPrime>
//               | epsilon
void term_prime() {
    printf("Parsing <TermPrime> (current token: %s)\n", token_type_to_string(token));
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

    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in term prime",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
}

// <Factor> ::= <Identifier> | <Number> | ( <Expression> )
void factor() {
    printf("Parsing <Factor> (current token: %s)\n", token_type_to_string(token));
    if (token == IDENTIFIER) 
    {
        match(IDENTIFIER);
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

    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in factor",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
}

// <IfStatement> ::= if ( <Expression> ) <CompoundStatement> [ else <CompoundStatement> ]
void if_statement() 
{
    printf("Parsing <IfStatement> (current token: %s)\n", token_type_to_string(token));
    match(IF);
    match(LPAREN);
    expression();
    match(RPAREN);
    compound_statement();
    if (token == ELSE) {
        match(ELSE);
        compound_statement();
    }
}

// <WhileStatement> ::= while ( <Expression> ) <CompoundStatement>
void while_statement()
{
    printf("Parsing <WhileStatement> (current token: %s)\n", token_type_to_string(token));
    match(WHILE);
    match(LPAREN);
    expression();
    match(RPAREN);
    compound_statement();
}

// --- Main Driver ---
void parse() {

    get_next_token(); // Initialize current_token
    program(); // Start parsing from the <Program> rule

    if (token == 0) 
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

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        yyfilename = argv[1];
        if (!yyin) {
            perror(argv[1]); // Print system error message if fopen fails
            return 1;
        }
    } else {
        printf("No input file specified. Reading from stdin.\n");
        yyin = stdin; // Default to standard input
    }

    parse();

    if(error_count > 0) {
        printf("Parsing completed with %d errors.\n", error_count);
        show_and_free_errors();
    } else {
        printf("Parsing completed successfully.\n");
    }

    return 0;
}
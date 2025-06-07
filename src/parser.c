#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"

// Forward declarations for recursive functions
void program();
void block();
void compound_statement();
void statement_list();
void statement();
void assignment_statement();
void declare_statement();
void expression();
void expression_prime();
void term();
void term_prime();
void factor();
void type();

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
        case RETURN: return "RETURN";
        case IF: return "IF";
        case ELSE: return "ELSE";
        case DO: return "DO";
        case WHILE: return "WHILE";
        case CONST: return "CONST";
        case MAIN: return "MAIN";
        case 0: return "EOF"; // End of file
        default: 
            return "UNKNOWN"; // For any unrecognized token type
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

// <Program> ::= <Block>
void program() {
    block();
    // After a successful program parse, we expect EOF
    if (token != 0) {
        save_error_pos("syntax error", "Expected EOF after program");
    }
}

// <Block> ::= int main ( ) <CompoundStatement>
void block() {
    printf("Parsing <Block>\n");
    match(INT);
    match(MAIN);
    match(LPAREN);
    match(RPAREN);
    compound_statement();
}

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
    if(token == IDENTIFIER || token == INT || token == CHAR || token == LBRACE) 
    {
        statement();
        statement_list(); // Recursive call to handle the next statement
        return;
    } 

    if (token == RBRACE) return; // Epsilon production: do nothing, end of statement list. 

    // If we reach here, it means we have an unexpected token.
    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in statement list",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
}


// <Statement> ::= <AssignmentStatement> ;
//               | <CompoundStatement> ;
//               | <DeclareStatment> ;
//               | epsilon
void statement() {
    printf("Parsing <Statement> (current token: %s)\n", token_type_to_string(token));
    // Lookahead for specific statement types
    if (token == INT || token == CHAR) {
        declare_statement();
        match(SEMI);
        return;
    } 
    if (token == LBRACE) 
    {
        compound_statement();
        return;
    }
    if(token == IDENTIFIER) {
        assignment_statement();
        match(SEMI);
        return;
    }
    // else: Epsilon production for <Statement>. Do nothing, consume no tokens.
    if(token == IDENTIFIER || token == INT || token == CHAR || token == LBRACE || token == RBRACE) {
        return;
    }

    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in statement",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
}

// <AssignmentStatement> ::= <Identifier> = <Expression>
void assignment_statement() {
    printf("Parsing <AssignmentStatement>\n");
    match(IDENTIFIER);
    match(ASSIGN);
    expression();
}

// <DeclareStatment> ::= <Type> <Identifier> [= <Expression>]
void declare_statement() {
    printf("Parsing <DeclareStatment>\n");
    type();
    match(IDENTIFIER);
    if (token == ASSIGN) { // TODO : 這邊對應的 ebnf 應該要修
        match(ASSIGN);
        expression();
    }
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

    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in type declaration",
            token_type_to_string(token));
    save_error_pos("syntax error", error_msg);
}

// <Expression> ::= <Term> <ExpressionPrime>
void expression() {
    printf("Parsing <Expression>\n");
    term();
    expression_prime();
}

// <ExpressionPrime> ::= + <Term> <ExpressionPrime>
//                     | - <Term> <ExpressionPrime>
//                     | epsilon
void expression_prime() {
    printf("Parsing <ExpressionPrime> (current token: %s)\n", token_type_to_string(token));
    if (token == PLUS) {
        match(PLUS);
        term();
        expression_prime();
        return;
    } 
    if (token == MINUS) {
        match(MINUS);
        term();
        expression_prime();
        return;
    }

    if(token == RPAREN || token == SEMI) return;
    // else: Epsilon production. Do nothing.
    char error_msg[200];
    sprintf(error_msg, "Unexpected token %s in expression prime",
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
    if(token == PLUS || token == MINUS || token == RPAREN || token == SEMI) {
        return;
    }

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

// --- Main Driver ---
void parse() {

    get_next_token(); // Initialize current_token
    program(); // Start parsing from the <Program> rule

    if (token == 0) 
    {
        return;
    } 
    else 
    {
        // This case should ideally be caught by program_parser checking for EOF.
        char error_msg[200];
        sprintf(error_msg, "Unexpected token %s at end of input",
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
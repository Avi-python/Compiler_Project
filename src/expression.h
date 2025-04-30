typedef enum {
    NODE_ASSIGNMENT,
    NODE_BINARY_OP,
    NODE_NUMBER,
    NODE_IDENTIFIER
} NodeType;

typedef struct ASTNode {
    NodeType type;
    struct ASTNode* next; // For linked list of nodes, if needed
} ASTNode;

typedef struct {
    ASTNode base;
    char identifier;
    ASTNode* expression;
} AssignmentNode;

typedef struct {
    ASTNode base;
    char op; // '+', '-', '*', '/'
    ASTNode* left;
    ASTNode* right;
} BinaryOpNode;

typedef struct {
    ASTNode base;
    int value;
} NumberNode;

typedef struct {
    ASTNode base;
    char name;
} IdentifierNode;

typedef union {
    AssignmentNode assignment;
    BinaryOpNode binary_op;
    NumberNode number;
    IdentifierNode identifier;
} ASTNodeData;

typedef struct {
    ASTNode base;
    ASTNodeData data;
} ASTNodeWrapper;
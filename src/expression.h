#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>

class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual std::string toString() const = 0;
};

class ExpressionNode : public ASTNode 
{

};

class BinaryOpNode : public ExpressionNode 
{
public:
    BinaryOpNode(ExpressionNode* left, ExpressionNode* right) : left(left), right(right) {}

    ~BinaryOpNode() override {
        delete left;
        delete right;
    }

protected:
    ExpressionNode *left;
    ExpressionNode *right;
};

class AddNode : BinaryOpNode
{
public:
    AddNode(ExpressionNode* left, ExpressionNode* right) : BinaryOpNode(left, right) {}
};

class SubNode : BinaryOpNode
{
public:
    SubNode(ExpressionNode* left, ExpressionNode* right) : BinaryOpNode(left, right) {}
};

class MulNode : BinaryOpNode
{
public:
    MulNode(ExpressionNode* left, ExpressionNode* right) : BinaryOpNode(left, right) {}
};

class DivNode : BinaryOpNode
{
public:
    DivNode(ExpressionNode* left, ExpressionNode* right) : BinaryOpNode(left, right) {}
};

class AssignNode : BinaryOpNode
{
public:
    AssignNode(ExpressionNode* left, ExpressionNode* right) : BinaryOpNode(left, right) {}
};

class IdNode : public ExpressionNode
{
public:
    IdNode(std::string value) : name(value) {}
    std::string name;
};

class IntegerNode : public ExpressionNode 
{
public:
    IntegerNode(int value) : value(value) {}
    int value;
};

#endif
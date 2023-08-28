#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../token/token.hpp"

using llf = long double;
using lld = long long int;

class PyObject; 
class NodeVisitor;

// Define the AST node types
enum class AstNodeType {
    // Root
    Program, 
    
    // Group Statements
    Block,
    
    // Statements
    Print, While, If, Function,
    Return,

    // Expressions
    UnaryOp, BinaryOp, TernaryOp,
    Assign, Call,

    // Literals
    Name, String,
    Int, Float, Boolean, Null
};


class AstNode {

public:

    AstNode(AstNodeType type) : type(type) {}
    
    virtual ~AstNode() {}

    bool is_name_node() { return type == AstNodeType::Name; }

    virtual PyObject* accept(NodeVisitor* visitor) = 0; 

    AstNodeType type;
};


class PrintNode : public AstNode {

public:

    PrintNode(AstNode* args) 
      : AstNode(AstNodeType::Print),  args(args) {}

    AstNode* args;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class BlockNode : public AstNode {

public:

    BlockNode(const std::vector<AstNode*> statements)
      : AstNode(AstNodeType::Block), statements(statements) {}
      
    std::vector<AstNode*> statements;
    
    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class ProgramNode : public AstNode {

public:

    ProgramNode(BlockNode* body) 
      : AstNode(AstNodeType::Program), body(body) {}
    
    BlockNode* body;
    
    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class TernaryOpNode : public AstNode {

public:

    TernaryOpNode(AstNode* condition, AstNode* left, AstNode* right) 
      : AstNode(AstNodeType::TernaryOp), 
        cond(condition), left(left), right(right) {}

    AstNode *cond, *left, *right;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class BinaryOpNode : public AstNode {

public:

    BinaryOpNode(AstNode* left, Token op, AstNode* right)
      : AstNode(AstNodeType::BinaryOp), 
        left(left), op(op), right(right) {}

    Token op;
    AstNode *left, *right;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class UnaryOpNode : public AstNode {

public:
    
    UnaryOpNode(Token op, AstNode* right) 
      : AstNode(AstNodeType::UnaryOp), 
        op(op), right(right) {}

    Token op;
    AstNode* right;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class IntNode : public AstNode {

public:
    
    IntNode(Token value) 
      : AstNode(AstNodeType::Int), value(value) {}

    Token value; // TODO: implement the BigInt class
    
    const std::string& getLexeme() { return value.lexeme; }

    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class FloatNode : public AstNode {

public:
    
    FloatNode(Token value)
      : AstNode(AstNodeType::Float), value(value) {}

    Token value;
    
    const std::string& getLexeme() { return value.lexeme; }

    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class NameNode : public AstNode {

public:
    
    NameNode(Token name) 
      : AstNode(AstNodeType::Name), value(name) {}
    
    Token value;
    
    const std::string& getLexeme() { return value.lexeme; }

    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class StringNode : public AstNode {

public:
    
    StringNode(Token value) 
      : AstNode(AstNodeType::String), value(value) {}
    
    Token value;
    
    const std::string& getLexeme() { return value.lexeme; }

    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class BooleanNode : public AstNode {

public:

    BooleanNode(bool value) 
      : AstNode(AstNodeType::Boolean), value(value) {}
    
    bool value;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class ReturnNode : public AstNode {

public:

    ReturnNode(AstNode* value) 
      : AstNode(AstNodeType::Return), value(value) {}
    
    AstNode* value;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class NullNode : public AstNode {

public:
    
    NullNode(Token none) : AstNode(AstNodeType::Null), none(none) {}
    
    Token none;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class CallNode : public AstNode {

public:
    
    CallNode(AstNode* name, const std::vector<AstNode*>& args) 
      : AstNode(AstNodeType::Call), caller(name), args(args) {}

    AstNode* caller;
    std::vector<AstNode*> args;

    virtual PyObject* accept(NodeVisitor * visitor) override;
};


class AssignNode : public AstNode {
public:
    AssignNode(AstNode* name, AstNode* value) 
      : AstNode(AstNodeType::Assign), name(name), value(value) {}

    AstNode *name, *value;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class WhileNode : public AstNode {
public:

    WhileNode(AstNode* cond, AstNode* body) 
      : AstNode(AstNodeType::While), cond(cond), body(body) {}

    AstNode *cond, *body;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class FunctionNode : public AstNode {

public:

    FunctionNode(Token fname, std::vector<AstNode*> params, AstNode* body)
        : AstNode(AstNodeType::Function), 
          fname(fname), params(params), body(body) {}
   
    const std::string& getName() const { return fname.lexeme; };
    const std::vector<AstNode*>& getParams() const { return params; }
    AstNode* getBody() const { return body; }

    virtual PyObject* accept(NodeVisitor* visitor) override;

private:
    Token fname;
    std::vector<AstNode*> params;
    AstNode* body;
};


class IfNode : public AstNode {

public:

    IfNode(AstNode* cond, AstNode* trueBranch, 
           const std::vector<std::pair<AstNode*, AstNode*>>& elifBranches, 
           AstNode* elseBranch) : AstNode(AstNodeType::If), cond(cond), 
          trueBranch(trueBranch), elifBranches(elifBranches), elseBranch(elseBranch) {}

    AstNode *cond, *trueBranch, *elseBranch;;
    std::vector<std::pair<AstNode*, AstNode*>> elifBranches;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};


class NodeVisitor {

public:

    ~NodeVisitor() = default;

    virtual PyObject* visitProgramNode(ProgramNode* node) = 0;
    virtual PyObject* visitBlockNode(BlockNode* node) = 0;
    virtual PyObject* visitPrintNode(PrintNode* node) = 0;
    virtual PyObject* visitWhileNode(WhileNode* node) = 0;
    virtual PyObject* visitIfNode(IfNode* node) = 0;
    virtual PyObject* visitAssignNode(AssignNode* node) = 0;
    virtual PyObject* visitTernaryOpNode(TernaryOpNode* node) = 0;
    virtual PyObject* visitBinaryOpNode(BinaryOpNode* node) = 0;
    virtual PyObject* visitUnaryOpNode(UnaryOpNode* node) = 0;
    virtual PyObject* visitIntNode(IntNode* node) = 0;
    virtual PyObject* visitFloatNode(FloatNode* node) = 0;
    virtual PyObject* visitNameNode(NameNode* node) = 0;
    virtual PyObject* visitStringNode(StringNode* node) = 0;
    virtual PyObject* visitBooleanNode(BooleanNode* node) = 0;
    virtual PyObject* visitNullNode(NullNode* node) = 0;
    virtual PyObject* visitFunctionNode(FunctionNode* node) = 0;
    virtual PyObject* visitCallNode(CallNode* node) = 0;
    virtual PyObject* visitReturnNode(ReturnNode* node) = 0;
};


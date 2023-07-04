#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

class Value; 
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

    virtual Value* accept(NodeVisitor* visitor) = 0; 

    AstNodeType type;
    
    bool is_name_node() { return type == AstNodeType::Name; }
};

class PrintNode : public AstNode {

public:
    PrintNode(AstNode* args) 
      : AstNode(AstNodeType::Print),  args(args) {}

    AstNode* args;

    virtual Value* accept(NodeVisitor* visitor) override;
};

class BlockNode : public AstNode {
public:
    BlockNode(const vector<AstNode*>& statements)
      : AstNode(AstNodeType::Block), statements(statements) {}
      
    vector<AstNode*> statements;
    
    virtual Value* accept(NodeVisitor* visitor) override;
};

class ProgramNode : public AstNode {
public:
    ProgramNode(BlockNode* body) 
      : AstNode(AstNodeType::Program), body(body) {}
    
    BlockNode* body;
    
    virtual Value* accept(NodeVisitor* visitor) override;
};

class TernaryOpNode : public AstNode {
public:
    TernaryOpNode(AstNode* condition, AstNode* left, AstNode* right) 
      : AstNode(AstNodeType::TernaryOp), cond(condition), left(left), right(right) {}

    AstNode *cond, *left, *right;

    virtual Value* accept(NodeVisitor* visitor) override;
};

class BinaryOpNode : public AstNode {
public:

    BinaryOpNode(AstNode* left, const string& op, AstNode* right)
      : AstNode(AstNodeType::BinaryOp), left(left), op(op), right(right) {}

    string op;
    AstNode *left, *right;

    virtual Value* accept(NodeVisitor* visitor) override;
};

class UnaryOpNode : public AstNode {
public:
    UnaryOpNode(const string& op, AstNode* right) 
      : AstNode(AstNodeType::UnaryOp), op(op), right(right) {}

    string op;
    AstNode* right;

    virtual Value* accept(NodeVisitor* visitor) override;
};

class IntNode : public AstNode {
public:
    IntNode(long long value) 
      : AstNode(AstNodeType::Int), value(value) {}

    long long value; // TODO: implement the BigInt class

    virtual Value* accept(NodeVisitor* visitor) override;
};
class FloatNode : public AstNode {
public:
    FloatNode(double value)
      : AstNode(AstNodeType::Float), value(value) {}

    double value;

    virtual Value* accept(NodeVisitor* visitor) override;
};

class NameNode : public AstNode {
public:
    NameNode(const string& name) 
      : AstNode(AstNodeType::Name), name(name) {}
    
    string name;

    virtual Value* accept(NodeVisitor* visitor) override;
};

class StringNode : public AstNode {
public:
    StringNode(const string& lexeme) 
      : AstNode(AstNodeType::String), lexeme(lexeme) {}
    
    string lexeme;

    virtual Value* accept(NodeVisitor* visitor) override;
};

class BooleanNode : public AstNode {
public:
    BooleanNode(bool value) 
      : AstNode(AstNodeType::Boolean), value(value) {}
    
    bool value;

    virtual Value* accept(NodeVisitor* visitor) override;
};

class ReturnNode : public AstNode {

public:

    ReturnNode(AstNode * value) 
      : AstNode(AstNodeType::Return), value(value) {}
    
    AstNode * value;

    virtual Value* accept(NodeVisitor* visitor) override;
};

class NullNode : public AstNode {
public:
    NullNode() : AstNode(AstNodeType::Null) {}

    virtual Value* accept(NodeVisitor* visitor) override;
};

class CallNode : public AstNode {
public:
    CallNode(AstNode* name, const vector<AstNode*>& args) 
      : AstNode(AstNodeType::Call), name(name), args(args) {}

    AstNode* name;
    vector<AstNode*> args;

    virtual Value* accept(NodeVisitor * visitor) override;
};

class AssignNode : public AstNode {
public:
    AssignNode(AstNode* name, AstNode* value) 
      : AstNode(AstNodeType::Assign), name(name), value(value) {}

    AstNode *name, *value;

    virtual Value* accept(NodeVisitor* visitor) override;
};

class WhileNode : public AstNode {
public:

    WhileNode(AstNode* cond, AstNode* body) 
      : AstNode(AstNodeType::While), cond(cond), body(body) {}

    AstNode *cond, *body;

    virtual Value* accept(NodeVisitor* visitor) override;
};

class FunctionNode : public AstNode {

public:

    FunctionNode(AstNode *name, vector<AstNode*> params, AstNode* body)
    : AstNode(AstNodeType::Function),  name(name) , params(params) , body(body) {}
   
    AstNode* getName() const { return name; };

    vector<AstNode*>  getParams() const { return params; }

    AstNode*  getBody() const { return body; }

    virtual Value* accept(NodeVisitor* visitor) override;

private:

    AstNode* name;
    vector<AstNode*> params;
    AstNode* body;
};



class IfNode : public AstNode {
public:

    IfNode(AstNode* cond, AstNode* trueBranch, 
           const vector<pair<AstNode*, AstNode*>>& elifBranches, 
           AstNode* elseBranch) : AstNode(AstNodeType::If), cond(cond), 
          trueBranch(trueBranch), elifBranches(elifBranches), elseBranch(elseBranch) {}

    AstNode *cond, *trueBranch, *elseBranch;;
    vector<pair<AstNode*, AstNode*> > elifBranches;

    virtual Value* accept(NodeVisitor* visitor) override;
};



class NodeVisitor {

public:

    ~NodeVisitor() = default;

    virtual Value* visitProgramNode(ProgramNode* node) = 0;
    virtual Value* visitBlockNode(BlockNode* node) = 0;
    virtual Value* visitPrintNode(PrintNode* node) = 0;
    virtual Value* visitWhileNode(WhileNode* node) = 0;
    virtual Value* visitIfNode(IfNode* node) =  0;
    virtual Value* visitAssignNode(AssignNode*  node) = 0;
    virtual Value* visitTernaryOpNode(TernaryOpNode* node) = 0;
    virtual Value* visitBinaryOpNode(BinaryOpNode* node) = 0;
    virtual Value* visitUnaryOpNode(UnaryOpNode* node)  = 0;
    virtual Value* visitIntNode(IntNode* node) = 0;
    virtual Value* visitFloatNode(FloatNode* node) = 0;
    virtual Value* visitNameNode(NameNode* node) = 0;
    virtual Value* visitStringNode(StringNode* node) = 0;
    virtual Value* visitBooleanNode(BooleanNode* node)  = 0;
    virtual Value* visitNullNode(NullNode* node) = 0;
    virtual Value* visitFunctionNode(FunctionNode* node) = 0;
    virtual Value* visitCallNode(CallNode* node) = 0;
    virtual Value* visitReturnNode(ReturnNode* node) = 0;

  //  virtual Value * visitCallNode(const CallNode*  expr) = 0;
};


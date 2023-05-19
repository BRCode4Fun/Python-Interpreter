#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

using namespace std;

// Define the AST node types
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

using namespace std;


class Value; 
class NodeVisitor;

// Define the AST node types
enum class AstNodeType {
    // Statements
    Program, Print,

    // Expressions
    BinaryOp, UnaryOp,
    Assign, Call, 

    // Literals
    Number, Boolean, Name, Null
};

class AstNode {

public:

    AstNode(AstNodeType type) : type(type) {}
    
    virtual ~AstNode() {}

    virtual Value * accept(NodeVisitor * visitor) = 0; 

    AstNodeType type;
};

class PrintNode : public AstNode {

public:

    PrintNode(AstNode* args) 
      : AstNode(AstNodeType::Print),  args(args) {}

    virtual Value *accept(NodeVisitor * visitor) override;

    AstNode* args;
};



class ProgramNode : public AstNode {
public:

    ProgramNode(std::vector<AstNode*> statements) 
      : AstNode(AstNodeType::Program) , statements(statements) {}
    virtual ~ProgramNode() {}
    
    virtual Value *accept(NodeVisitor * visitor) override;


    std::vector<AstNode*> statements;
};



class BinaryOpNode : public AstNode {
public:

    BinaryOpNode(AstNode* left, std::string op, AstNode* right) 
      : AstNode(AstNodeType::BinaryOp), left(left), op(op), right(right) {}

    virtual Value *accept(NodeVisitor * visitor) override;

    AstNode* left;
    std::string op;
    AstNode* right;
};

class UnaryOpNode : public AstNode {
public:
    UnaryOpNode(string op, AstNode* right) : AstNode(AstNodeType::UnaryOp), op(op), right(right) {}

    std::string op;
    AstNode* right;

    virtual Value *accept(NodeVisitor * visitor) override;
};

class NumNode : public AstNode {
public:
    NumNode(double value) 
      : AstNode(AstNodeType::Number), value(value) {}

    virtual Value *accept(NodeVisitor * visitor) override;

  
    double value;
};



class NameNode : public AstNode {
public:
    NameNode(const std::string& name) : AstNode(AstNodeType::Name), name(name) {}
    std::string name;

    virtual Value *accept(NodeVisitor * visitor) override;
};

class BooleanNode : public AstNode {
public:
    BooleanNode(bool value) : AstNode(AstNodeType::Boolean), value(value) {}
    bool value;

    virtual Value *accept(NodeVisitor * visitor) override;
};

class NullNode : public AstNode {
public:
    NullNode() : AstNode(AstNodeType::Null) {}

    virtual Value *accept(NodeVisitor * visitor) override;
};

class CallNode : public AstNode {
public:
    CallNode(AstNode* name, std::vector<AstNode*> args) : AstNode(AstNodeType::Call), name(name), args(args) {}

    AstNode* name;
    std::vector<AstNode*> args;

    virtual Value *accept(NodeVisitor * visitor) override;
};

class AssignNode : public AstNode {
public:
    AssignNode(AstNode* name, AstNode* value) : AstNode(AstNodeType::Assign), name(name), value(value) {}

    virtual Value *accept(NodeVisitor * visitor) override;

    AstNode* name;
    AstNode* value;
};


class NodeVisitor {

public:

    ~NodeVisitor() = default;

    virtual Value * visitPrintNode( PrintNode* node) = 0;
    virtual Value * visitProgramNode( ProgramNode* node) = 0;
    virtual Value * visitBinaryOpNode( BinaryOpNode* node) = 0;
    virtual Value * visitNumNode( NumNode* node) = 0;
    virtual Value * visitNameNode( NameNode* node) = 0;
    virtual Value * visitBooleanNode(BooleanNode* node)  = 0;
    virtual Value * visitUnaryOpNode(UnaryOpNode* node)  = 0;
    virtual Value * visitNullNode(NullNode* expr) = 0;
  //  virtual Value * visitCallNode(const CallNode*  expr) = 0;
    virtual Value * visitAssignNode(AssignNode*  node) = 0;
};


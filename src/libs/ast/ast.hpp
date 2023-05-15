#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

using namespace std;

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

    AstNodeType type;
};

class PrintNode : public AstNode {
public:
    PrintNode(AstNode* args) 
      : AstNode(AstNodeType::Print),  args(args) {}

    AstNode* args;
};

class ProgramNode : public AstNode {
public:
    ProgramNode(std::vector<AstNode*> statements) 
      : AstNode(AstNodeType::Program) , statements(statements) {}
    virtual ~ProgramNode() {}

    std::vector<AstNode*> statements;
};

class BinaryOpNode : public AstNode {
public:

    BinaryOpNode(AstNode* left, std::string op, AstNode* right) 
      : AstNode(AstNodeType::BinaryOp), left(left), op(op), right(right) {}

    AstNode* left;
    std::string op;
    AstNode* right;
};

class UnaryOpNode : public AstNode {
public:
    UnaryOpNode(string op, AstNode* right) : AstNode(AstNodeType::UnaryOp), op(op), right(right) {}

    std::string op;
    AstNode* right;
};

class NumNode : public AstNode {
public:
    NumNode(double value) 
      : AstNode(AstNodeType::Number), value(value) {}
      
    double value;
};

class NameNode : public AstNode {
public:
    NameNode(const std::string& name) : AstNode(AstNodeType::Name), name(name) {}
    std::string name;
};

class BooleanNode : public AstNode {
public:
    BooleanNode(bool value) : AstNode(AstNodeType::Boolean), value(value) {}
    bool value;
};

class NullNode : public AstNode {
public:
    NullNode() : AstNode(AstNodeType::Null) {}
};

class CallNode : public AstNode {
public:
    CallNode(AstNode* name, std::vector<AstNode*> args) : AstNode(AstNodeType::Call), name(name), args(args) {}

    AstNode* name;
    std::vector<AstNode*> args;
};

class AssignNode : public AstNode {
public:
    AssignNode(AstNode* name, AstNode* value) : AstNode(AstNodeType::Assign), name(name), value(value) {}

    AstNode* name;
    AstNode* value;
};
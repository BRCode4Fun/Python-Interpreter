#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

using namespace std;

// Define the AST node types
enum class AstNodeType {
    Program,
    Number,
    BinaryOp,
    UnaryOp,
    Name,
    Assign,
    Call, 
    Print
};

class AstNode {
public:
    AstNode(AstNodeType type) : type(type) {}
    virtual ~AstNode() {}

    AstNodeType type;
};

class PrintNode : public AstNode {
public:
    PrintNode(AstNode* args) : AstNode(AstNodeType::Print),  args(args) {}

    //NameNode* name;
    AstNode* args;
};

class ProgramNode : public AstNode {
public:
    ProgramNode(std::vector<AstNode*> statements) : AstNode(AstNodeType::Program) , statements(statements) {}
    virtual ~ProgramNode() {}

    std::vector<AstNode*> statements;
};

class NumNode : public AstNode {
public:
    NumNode(double value) : AstNode(AstNodeType::Number), value(value) {}
    double value;
};

class BinaryOpNode : public AstNode {
public:

    BinaryOpNode(AstNode* left,  std::string  op, AstNode* right) : AstNode(AstNodeType::BinaryOp), left(left), op(op), right(right) {}

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

class NameNode : public AstNode {
public:
    NameNode(const std::string& name) : AstNode(AstNodeType::Name), name(name) {}
    std::string name;
};

class AssignNode : public AstNode {
public:
    AssignNode(NameNode* name, AstNode* value) : AstNode(AstNodeType::Assign), name(name), value(value) {}

    NameNode* name;
    AstNode* value;
};

class CallNode : public AstNode {
public:
    CallNode(AstNode* name, std::vector<AstNode*> args) : AstNode(AstNodeType::Call), name(name), args(args) {}

    AstNode* name;
    std::vector<AstNode*> args;
};

/*
class Interpreter {
public:
    Interpreter() {}

    void execute(const AstNode* node) {
        switch (node->getType()) {
            case AstNodeType::Module:
                executeModule(dynamic_cast<const ModuleNode*>(node));
                break;
            case AstNodeType::FunctionDef:
                executeFunctionDef(dynamic_cast<const FunctionDefNode*>(node));
                break;
            case AstNodeType::Return:
                executeReturn(dynamic_cast<const ReturnNode*>(node));
                break;
            case AstNodeType::Expr:
                executeExpr(dynamic_cast<const ExprNode*>(node));
                break;
            case AstNodeType::Call:
                executeCall(dynamic_cast<const CallNode*>(node));
                break;
            case AstNodeType::Name:
                executeName(dynamic_cast<const NameNode*>(node));
                break;
            case AstNodeType::Num:
                executeNum(dynamic_cast<const NumNode*>(node));
                break;
            default:
                throw runtime_error("Unknown AST node type");
        }
    }

private:
    // Symbol table for storing variable names and values
    unordered_map<string, Value> symbolTable;

    void executeModule(const ModuleNode* node) {
        for (const auto& statement : node->body) {
            execute(statement.get());
        }
    }

    void executeFunctionDef(const FunctionDefNode* node) {
        // Function definitions are not executed, they are just added to the symbol table
        symbolTable[node->getName()] = Value();
    }

    void executeReturn(const ReturnNode* node) {
        // Return statements are not executed, they are just ignored
    }

    void executeExpr(const ExprNode* node) {
        evaluate(node->value.get());
    }

    void executeCall(const CallNode* node) {
        if (node->getName() == "print") {
            for (size_t i = 0; i < node->args.size(); i++) {
                auto value = evaluate(node->args[i].get());
                switch (value.getType()) {
                    case Value::Type::Undefined:
                        cout << "undefined";
                        break;
                    case Value::Type::Number:
                        cout << value.toNumber();
                        break;
                    case Value::Type::Boolean:
                        cout                  << (value.toBoolean() ? "True" : "False");
                    break;
                case Value::Type::String:
                    cout << value.toString();
                    break;
                case Value::Type::List:
                    cout << "[";
                    for (size_t j = 0; j < value.toList().size(); j++) {
                        cout << value.toList()[j].toString();
                        if (j != value.toList().size() - 1) {
                            cout << ", ";
                        }
                    }
                    cout << "]";
                    break;
                default:
                    throw runtime_error("Unknown value type");
            }
            if (i != node->args.size() - 1) {
                cout << " ";
            }
        }
        cout << endl;
    } else {
        throw runtime_error("Unknown function name: " + node->getName());
    }
}

Value evaluate(const AstNode* node) {
    switch (node->getType()) {
        case AstNodeType::Num:
            return evaluateNum(dynamic_cast<const NumNode*>(node));
        case AstNodeType::Name:
            return evaluateName(dynamic_cast<const NameNode*>(node));
        case AstNodeType::Call:
            return evaluateCall(dynamic_cast<const CallNode*>(node));
        default:
            throw runtime_error("Unsupported node type for evaluation");
    }
}



Value evaluateNum(const NumNode* node) {
    return Value(node->getValue());
}

Value evaluateName(const NameNode* node) {
    if (symbolTable.count(node->getName()) == 0) {
        throw runtime_error("Undefined variable: " + node->getName());
    }
    return symbolTable[node->getName()];
}

Value evaluateCall(const CallNode* node) {
    if (node->getName() == "len") {
        auto arg = evaluate(node->args[0].get());
        if (arg.getType() != Value::Type::List) {
            throw runtime_error("len() argument must be a list");
        }
        return Value(arg.toList().size());
    } else {
        throw runtime_error("Unknown function name: " + node->getName());
    }
}

};

*/

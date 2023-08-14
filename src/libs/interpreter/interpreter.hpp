#pragma once

#include <stack>
#include <unordered_map>
#include "../value/value.hpp"
#include "../ast/ast.hpp"
#include "../gc/gc.hpp"
#include "../scope/scope.hpp"

class Interpreter : public NodeVisitor {
   
public:
    Interpreter() {

        GC = new GarbageCollector();

        if(!GC) throw std::runtime_error("Error while allocating garbage collector");

        Global_Environment = new Scope();
        Environment.push(new Scope());
    }
        
    Value* interpret(ProgramNode* node);

    virtual Value* visitProgramNode( ProgramNode* node) override;
    virtual Value* visitBlockNode( BlockNode* node) override;
    virtual Value* visitPrintNode( PrintNode* node) override;
    virtual Value* visitWhileNode(WhileNode* node) override;
    virtual Value* visitIfNode(IfNode* node) override;
    virtual Value* visitAssignNode(AssignNode* node)  override;
    virtual Value* visitTernaryOpNode( TernaryOpNode* node) override;
    virtual Value* visitBinaryOpNode( BinaryOpNode* node) override;
    virtual Value* visitUnaryOpNode(UnaryOpNode* node)  override;
    virtual Value* visitIntNode( IntNode* node)  override;
    virtual Value* visitFloatNode( FloatNode* node)  override;
    virtual Value* visitNameNode( NameNode* node) override;
    virtual Value* visitStringNode( StringNode* node) override;
    virtual Value* visitBooleanNode(BooleanNode* node) override;
    virtual Value* visitNullNode(NullNode* expr) override;
    virtual Value* visitFunctionNode(FunctionNode* node) override;
    virtual Value* visitCallNode(CallNode* node) override;
    virtual Value* visitReturnNode(ReturnNode* node) override;

private:
    std::unordered_map<std::string, Value*> symbolTable;

    GarbageCollector* GC = nullptr; 

    Scope* Global_Environment;
    std::stack<Scope*> Environment;   
};

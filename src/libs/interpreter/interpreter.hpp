#pragma once

#include <stack>
#include "../ast/ast.hpp"
#include "../value/primitives.hpp"
#include "../scope/scope.hpp"
#include "../gc/gc.hpp"

class Interpreter : public NodeVisitor {
   
public:
    Interpreter() {
        __globals__ = new Scope();
        currentEnv.push(__globals__);
    }
        
    PyObject* interpret(ProgramNode* node);

    virtual PyObject* visitProgramNode( ProgramNode* node) override;
    virtual PyObject* visitBlockNode( BlockNode* node) override;
    virtual PyObject* visitPrintNode( PrintNode* node) override;
    virtual PyObject* visitWhileNode(WhileNode* node) override;
    virtual PyObject* visitIfNode(IfNode* node) override;
    virtual PyObject* visitAssignNode(AssignNode* node)  override;
    virtual PyObject* visitTernaryOpNode( TernaryOpNode* node) override;
    virtual PyObject* visitBinaryOpNode( BinaryOpNode* node) override;
    virtual PyObject* visitUnaryOpNode(UnaryOpNode* node)  override;
    virtual PyObject* visitIntNode( IntNode* node)  override;
    virtual PyObject* visitFloatNode( FloatNode* node)  override;
    virtual PyObject* visitNameNode( NameNode* node) override;
    virtual PyObject* visitStringNode( StringNode* node) override;
    virtual PyObject* visitBooleanNode(BooleanNode* node) override;
    virtual PyObject* visitNullNode(NullNode* expr) override;
    virtual PyObject* visitFunctionNode(FunctionNode* node) override;
    virtual PyObject* visitCallNode(CallNode* node) override;
    virtual PyObject* visitReturnNode(ReturnNode* node) override;

private:
    Scope* __globals__;
    std::stack<Scope*> currentEnv;
    GarbageCollector GC;
};

#pragma once

#include "../ast/ast.hpp"
#include "../gc/gc.hpp"

class PyObject;

class Interpreter : public NodeVisitor {
   
public:
    Interpreter();
        
    PyObject* interpret(ProgramNode* node);

    virtual PyObject* visitProgramNode(ProgramNode* node) override { return nullptr; }
    virtual PyObject* visitBlockNode(BlockNode* node) override;
    virtual PyObject* visitPrintNode(PrintNode* node) override;
    virtual PyObject* visitWhileNode(WhileNode* node) override;
    virtual PyObject* visitBreakNode(BreakNode* node) override;
    virtual PyObject* visitContinueNode(ContinueNode* node) override;
    virtual PyObject* visitPassNode(PassNode* node) override;
    virtual PyObject* visitIfNode(IfNode* node) override;
    virtual PyObject* visitAssignNode(AssignNode* node) override;
    virtual PyObject* visitTernaryOpNode(TernaryOpNode* node) override;
    virtual PyObject* visitBinaryOpNode(BinaryOpNode* node) override;
    virtual PyObject* visitUnaryOpNode(UnaryOpNode* node) override;
    virtual PyObject* visitIntNode(IntNode* node) override;
    virtual PyObject* visitFloatNode(FloatNode* node) override;
    virtual PyObject* visitNameNode(NameNode* node) override;
    virtual PyObject* visitStringNode(StringNode* node) override;
    virtual PyObject* visitBooleanNode(BooleanNode* node) override;
    virtual PyObject* visitNullNode(NullNode* expr) override;
    virtual PyObject* visitFunctionNode(FunctionNode* node) override;
    virtual PyObject* visitCallNode(CallNode* node) override;
    virtual PyObject* visitReturnNode(ReturnNode* node) override;
    virtual PyObject* visitClassNode(ClassNode* node) override;
    virtual PyObject* visitPropertyNode(PropertyNode* node) override;

    void pushContext(Scope* frame) {
        contextStack.push_back(frame);
    }
    
    void popContext() { 
        
        if(!contextStack.empty()) {
            //delete contextStack.back();
            contextStack.pop_back();
        } else {
            throw std::runtime_error("Cannot pop context from empty stack");
        }
    }
    
    Scope* currentContext() { 
        
        if(!contextStack.empty()) {
            return contextStack.back();
        } else {
            return nullptr;
        }
    }
    
    void defineOnContext(const std::string& name, PyObject* value) {
        
        if(!contextStack.empty()){
            Scope* lastFrame = contextStack.back();
            lastFrame->define(name, value);
        } else {
            throw std::runtime_error("Cannot define variable outside of context");
        }
    }
    
    PyObject* getFromContext(const std::string& name) {
        
        if(!contextStack.empty()){
            Scope* lastFrame = contextStack.back();
            return lastFrame->get(name);
        } else {
            throw std::runtime_error("Cannot access variable outside of context");
        }
    }

private:
    GarbageCollector GC;
    std::vector<Scope*> contextStack;
};

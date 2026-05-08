#pragma once

#include "../ast/ast.hpp"
#include "../gc/gc.hpp"
#include "../scope/scope.hpp"
#include "../exceptions/python_errors.hpp"

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
            throw RuntimeError("Cannot pop context from empty stack");
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
            throw RuntimeError("Cannot define variable outside of context");
        }
    }
    
    PyObject* getFromContext(const std::string& name) {
        
        if(!contextStack.empty()){
            Scope* lastFrame = contextStack.back();
            return lastFrame->get(name);
        } else {
            throw RuntimeError("Cannot access variable outside of context");
        }
    }

private:
    GarbageCollector GC;
    std::vector<Scope*> contextStack;
    PyTypeObject* object_type = nullptr;
    PyTypeObject* type_type = nullptr;
    PyTypeObject* int_type = nullptr;
    PyTypeObject* float_type = nullptr;
    PyTypeObject* bool_type = nullptr;
    PyTypeObject* str_type = nullptr;
    PyTypeObject* none_type = nullptr;
    PyTypeObject* function_type = nullptr;
    PyTypeObject* builtin_fn_type = nullptr;
    PyTypeObject* method_wrapper_type = nullptr;

    // auxiliar methods
    PyObject* resolve(const std::string& method_name, PyObject* object, 
                std::vector<PyObject*> args = std::vector<PyObject*>());
    PyObject* try_resolve(const std::string& method_name, PyObject* object,
                std::vector<PyObject*> args = std::vector<PyObject*>());
    TokenType assignment_to_binary(TokenType assignment_op);
    PyObject* make_bool_obj(bool value);
    PyObject* make_int_obj(lld value);
    PyObject* make_float_obj(llf value);
    PyObject* make_str_obj(const std::string& value);
    PyObject* negate_truthy_obj(PyObject* value);
    [[noreturn]] void throw_missing_method_error(PyObject* object, const std::string& method_name);
    void validate_call_arity(PyObject* callable, size_t provided_arg_count);

    inline const std::string& getString(PyObject* object);
    inline long long getInteger(PyObject* object);
    inline bool isTruthy(PyObject* object);
    inline bool isNumeric(PyObject* object);
    inline long double toFloat(PyObject* object);
    PyObject* evalBinary(TokenType op, PyObject* lhs, PyObject* rhs);
};

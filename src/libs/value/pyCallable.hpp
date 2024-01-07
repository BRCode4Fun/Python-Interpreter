#pragma once

#include "../interpreter/interpreter.hpp"

class PyCallable : public PyObject {
public:   
    explicit PyCallable(ObjectType type, Scope* context) 
        : PyObject(type), scope(context){}

    virtual size_t arity() = 0;
    virtual PyObject* call(Interpreter*, const std::vector<PyObject*>&) = 0;
    Scope* getContext() { return this->scope; }
    
private:
    Scope* scope;
};

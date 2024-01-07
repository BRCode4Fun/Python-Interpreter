#pragma once

#include "./pyCallable.hpp"
#include "../scope/scope.hpp"
#include "../exceptions/runtime_exceptions.hpp"
#include "./pyNone.hpp"

class PyFunction : public PyCallable {
public:
    explicit PyFunction(
                FunctionNode* declaration, 
                Scope* closure, 
                PyInstance* bound = nullptr
    ) : PyCallable(ObjectType::Func, closure), 
        declaration(declaration), 
        bound(bound) {}
    
    inline bool isFunc() const override { return true; }
    inline bool isTruthy() const override { return true; }
    
    PyFunction* bind(PyInstance* instance) {
        return new PyFunction(this->declaration, this->getContext(), instance);
    }
    
    PyObject* call(Interpreter* interpreter, const std::vector<PyObject*>& args) override {
        
        Scope* closure = this->getContext();
        Scope* fnCallEnv = new Scope(closure);
        interpreter->pushContext(fnCallEnv);
        
        const std::vector<AstNode*>& params = this->declaration->getParams();
        
        unsigned short bounded = (this->bound ? 1 : 0);
        
        size_t expectedArgs = params.size() - bounded;
        
        if (args.size() != expectedArgs) {
            throw std::runtime_error("error on positional args");
        }
        
        size_t argIndex = 0;
        if (this->bound) { // If bound, pass 'self' as the first argument
            NameNode* Self = static_cast<NameNode*>(params[argIndex++]);
            const std::string& argName = Self->getLexeme();
            interpreter->defineOnContext(argName, this->bound);
        }
        
        if(expectedArgs) {
            // Loop through parameters and define them in the context
            for(; argIndex < params.size(); ++argIndex) {
                NameNode* param = static_cast<NameNode*>(params[argIndex]);
                const std::string& argName = param->getLexeme();
                interpreter->defineOnContext(argName, args[argIndex - (bounded)]);
            }
        }
        
        PyObject* retVal = new PyNone();
        try {
            // Visit the body of the function, switching back to the interpreter's context
            declaration->getBody()->accept(interpreter);
        } catch(ReturnException& re) {
            delete retVal;
            retVal = re.value;
        }
        
        interpreter->popContext();
        return retVal;
    }
    
    size_t arity() override {
        return (bound ? 1 : 0) + (*declaration).getParams().size();
    }
    
    void write(std::ostream& out) const override {
        out << "<function \'" << (*declaration).getName() << "\'>";
    }
    
private:
    FunctionNode* declaration = nullptr;
    PyInstance* bound = nullptr;
    
    void deleteData() override {
        delete declaration;
        delete bound;
    }
};

#pragma once

#include "./pyFunction.hpp"
#include "./pyInstance.hpp"

class PyClass : public PyCallable {
public:
    std::string kname;
    
    explicit PyClass(const std::string& kname, Scope* closure) 
        : PyCallable(ObjectType::Klass, closure), kname(kname){}
    
    inline bool isKlass() const override { return true; }
    inline bool isTruthy() const override { return true; }    

    PyObject* call(Interpreter* interpreter, const std::vector<PyObject*>& args) override {
        
        PyInstance* instance = new PyInstance(this);
        
        PyFunction* initTarget = findMethod("__init__");
        
        if(initTarget) {
            initTarget->bind(instance)->call(interpreter, args);
        }
        return instance;
    }
    
    PyFunction* findMethod(const char* methodName) {
        try {
            PyObject* target = this->getContext()->get(methodName);
            PyFunction* method = static_cast<PyFunction*>(target);
            return method;
            
        } catch(std::runtime_error&) {
            return nullptr;
        }
    }
    
    size_t arity() override {
        PyFunction* initTarget = findMethod("__init__");
        return (initTarget ? initTarget->arity() : 0);
    }
    
    void write(std::ostream& out) const override {
        out << "<class \'" << kname << "\'>";
    }

private:
    void deleteData() override {
        delete getContext();
    }
};

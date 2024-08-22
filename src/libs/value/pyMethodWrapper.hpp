#pragma once

#include "./pyFunction.hpp"
#include "./pyStr.hpp"

class PyMethodWrapper : public PyObject {
public:
    explicit PyMethodWrapper(PyFunction* func, PyObject* instance)
        : PyObject(ObjectType::Method), func(func), instance(instance){}
        
    inline bool is_method_wrapper_type() const override { return true; }
    
    PyObject* call(std::vector<PyObject*>& args, Interpreter* interpreter = nullptr) {
        if(instance != nullptr) {
            args.insert(args.begin(), instance);
        }
        return func->call(args, interpreter);
    }
    PyFunction* getFunction() const { return func; }
    PyObject* getInstance() const { return instance; }
    
private:
    PyFunction* func;
    PyObject* instance;
    
    void registerMethods() override {
        
        this->define("__str__", 
            new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
                
                ASSERT_ARG_SIZE(args, 1);
                
                return new PyStr("<method-wrapper object>");
        }));

        this->define("__call__", this);
    }
    
    void deleteData() override {
        //delete func;
        //delete instance;
    }
};

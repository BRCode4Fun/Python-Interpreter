#pragma once

#include <functional>
#include "./pyObject.hpp"

class PyBuiltin : public PyObject {
    
public:
    
    // Constructor for built-in functions (C++ directly)
    explicit PyBuiltin(
        std::function<PyObject*(std::vector<PyObject*>&, Interpreter*)>,
        size_t arity = 0 // self
    );
    
    inline bool is_builtin_fn_type() const override { return true; }
    
    PyObject* call(std::vector<PyObject*>&, Interpreter* = nullptr);
    
private:
    std::string fname;

    // for internal functions
    std::function<PyObject*(std::vector<PyObject*>&, Interpreter*)> functor;

    size_t arity;
    
    void registerMethods() override;
    
    void deleteData() override {}
};

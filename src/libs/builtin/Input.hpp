#pragma once

#include "../value/pyCallable.hpp"
#include <iostream>
#include <string>

class PyStr;

class Input : public PyCallable {
public:
    explicit Input() : PyCallable(ObjectType::Builtin, nullptr){}
    
    size_t arity() override { return 0; }
    
    PyObject* call(Interpreter* interpreter, const std::vector<PyObject*>& args) override {
        
        if(!args.empty()) {
            throw std::runtime_error("input() takes no arguments");        
        }
        std::string inputStr;
        std::getline(std::cin, inputStr);
        
        return new PyStr(inputStr);
    }
    
    void write(std::ostream& out) const override {
        out << "<builtin  function input>";
    }
};

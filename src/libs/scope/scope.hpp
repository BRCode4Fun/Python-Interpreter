#pragma once

#include <unordered_map>
#include "../value/pyObject.hpp"

class Scope {

public:
    
    Scope(Scope* enclosing = nullptr) : enclosing(enclosing){}
    
    ~Scope() {
        for (auto& [key, value] : values) {
            value->decRefCount();
        }
    }
    
    void define(const std::string& name, PyObject* value) {
          
        auto it = values.find(name);
    
        if (it != values.end()) {
            PyObject* temp = it->second;
            temp->decRefCount();
            it->second = value;
            value->incRefCount();
        } else {
            values.emplace(name, value);
            value->incRefCount();
        }
    }

    PyObject* get(const std::string& name) {
        
        auto it = values.find(name);
        
        if(it != values.end()) {
            return it->second;
        
        } else if(enclosing) {
            return enclosing->get(name);
        
        } else {
            throw std::runtime_error("Undeclared variable '" + name + "'.");
        }
    }
    
    Scope* getEnclosing() {
        return this->enclosing;
    }

private:
    std::unordered_map<std::string, PyObject*> values;
    Scope* enclosing;
};


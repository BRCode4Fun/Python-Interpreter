#pragma once

#include <unordered_map>
#include <string>

class PyObject;

class Scope {
    
public:
    Scope(Scope* enclosing = nullptr) : enclosing(enclosing){}
    ~Scope();
    
    bool empty();
    void clear() { values.clear(); }
    
    void define(const std::string& name, PyObject* value);
    PyObject* get(const std::string& name);
    
    Scope* getEnclosing() { return this->enclosing; }

private:
    std::unordered_map<std::string, PyObject*> values;
    Scope* enclosing;
};

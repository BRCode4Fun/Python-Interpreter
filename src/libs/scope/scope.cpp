#include "scope.hpp"

Scope::Scope() : parent(nullptr){}

Scope::Scope(Scope* parent) {
 //   assert(parent != nullptr);
}

void Scope::decRefCount(PyObject* value){

    if(value != nullptr) value->decRc();
}

void Scope::incRefCount(PyObject* value){

    if(value != nullptr) value->incRc();
}

void Scope::define(const std::string& name, PyObject* value) {
    
    auto it = values.find(name);
   
    if (it != values.end()) {
        decRefCount(it->second);
        it->second = value;
        incRefCount(it->second);
    } else {
        values.emplace(name, value);
        incRefCount(value);
    }
}

PyObject* Scope::get(const std::string& name) {

    auto it = values.find(name);
   
    if (it != values.end()) return it->second;

    throw std::runtime_error("Undeclared variable '" + name + "'");
}

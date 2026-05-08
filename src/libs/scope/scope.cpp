#include <stdexcept>
#include "./scope.hpp"
#include "../value/pyObject.hpp"
#include "../exceptions/python_errors.hpp"

Scope::~Scope() {
    for(std::unordered_map<std::string, PyObject*>::value_type& entry : this->values){
        entry.second->decRefCount();
    }
}

bool Scope::empty() {
    return values.empty();
}

void Scope::define(const std::string& name, PyObject* value){
    
    std::unordered_map<std::string, PyObject*>::iterator it = this->values.find(name);
    
    if(it != values.end()){
        PyObject* tp_object = it->second;   
        tp_object->decRefCount();
        it->second = value;
        
    } else {
        values.emplace(name, value);
    }
    value->incRefCount();
}

PyObject* Scope::get(const std::string& name){
    
    std::unordered_map<std::string, PyObject*>::iterator it = this->values.find(name);
    
    if(it != values.end()) {
        return it->second;
    
    } else if(enclosing){
        return enclosing->get(name);
        
    } else {
        throw NameError("Undeclared variable '" + name + "'.");
    }
    return nullptr;
}

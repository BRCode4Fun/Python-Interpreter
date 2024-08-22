#include <stdexcept>

#include "./pyObject.hpp"
#include "./primitives.hpp"
#include "../scope/scope.hpp"

PyObject::PyObject(ObjectType type, void* data, Scope* context)
    : type(type), data(data){
    
    this->scope = context ? context : new Scope();
}

PyObject::~PyObject() {
    deleteData();
}

void PyObject::incRefCount(){ ++this->rc; }

void PyObject::decRefCount() { if(rc > 0) --this->rc; }

int PyObject::getRefCount() { return this->rc; }

Scope* PyObject::getContext() { return this->scope; }

void PyObject::define(const std::string& name, PyObject* object) {

    this->scope->define(name, object);
}

PyObject* PyObject::find(const std::string& name) {
    
    if(this->scope->empty()){
        this->registerMethods();
    }
    
    try {
        return this->scope->get(name);
    
    } catch(std::runtime_error&) {
        
        return nullptr;
    }
}

class PyStr* PyObject::unwrap_str_obj(){
    
    if(this->is_str_type()){
        return dynamic_cast<PyStr*>(this);
    } else {
        throw std::runtime_error("Object is not a string");
    }
}

class PyInt* PyObject::unwrap_int_obj(){
    
    if(this->is_int_type()){
        return dynamic_cast<PyInt*>(this);
    } else {
        throw std::runtime_error("Object is not an integer");
    }
}

class PyFloat* PyObject::unwrap_float_obj(){
    
    if(this->is_float_type()){
        return dynamic_cast<PyFloat*>(this);
    } else {
        throw std::runtime_error("Object is not a float");
    }
}

class PyBool* PyObject::unwrap_bool_obj(){
    
    if(this->is_bool_type()){
        return dynamic_cast<PyBool*>(this);
    } else {
        throw std::runtime_error("Object is not a boolean");
    }
}

class PyFunction* PyObject::unwrap_function_obj(){

    if(this->is_fn_type()){
        return dynamic_cast<PyFunction*>(this);
    } else {
        throw std::runtime_error("Object is not a function");
    }
}

class PyMethodWrapper* PyObject::unwrap_method_obj(){
    
    if(this->is_method_wrapper_type()){
        return dynamic_cast<PyMethodWrapper*>(this);
    } else {
        throw std::runtime_error("Object is not a method");
    }
}

class PyBuiltin* PyObject::unwrap_builtin_fn_obj(){

    if(this->is_builtin_fn_type()){
        return dynamic_cast<PyBuiltin*>(this);
    } else {
        throw std::runtime_error("Object is not a function");
    }
}

class PyClass* PyObject::unwrap_class_obj(){
    
    if(this->is_klass_type()){
        return dynamic_cast<PyClass*>(this);
    } else {
        throw std::runtime_error("Object is not a class");
    }
}

#include "./pyInstance.hpp"
#include "../scope/scope.hpp"
#include "./pyFunction.hpp"
#include "./pyStr.hpp"
#include "./pyBool.hpp"

PyInstance::PyInstance(PyClass* klass) 
    : PyObject(ObjectType::Instance, nullptr, new Scope(klass->getContext())), base(klass) {}

void PyInstance::registerMethods() {
    
    this->define("__class__", this->base);
}


#include "./pyClass.hpp"
#include "./pyFunction.hpp"
#include "../scope/scope.hpp"
#include "./pyStr.hpp"

PyClass::PyClass(const std::string& kname, Scope* closure)
    : PyObject(ObjectType::Klass, nullptr, closure), kname(kname){}

void PyClass::registerMethods() {
    
    this->define("__class__", this);

    this->define("__str__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 1);

            return new PyStr("<class object>");
        })
    );
}
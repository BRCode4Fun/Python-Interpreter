#include "./pyNone.hpp"
#include "./pyStr.hpp"
#include "./pyBool.hpp"
#include "./pyFunction.hpp"

PyNone::PyNone() : PyObject(ObjectType::None) {}

void PyNone::registerMethods() {
    
    this->define("__str__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 1);
            
            return new PyStr("None");
        })
    );

    this->define("__bool__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 1);

            return new PyBool(false);
        })
    );

    this->define("__eq__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];
            return new PyBool(other->is_none_type() ? true : false); // None is only equal to None
        })
    );
}


#include "./pyBool.hpp"
#include "./pyStr.hpp"
#include "./pyFunction.hpp"

PyBool::PyBool(bool v) 
    : PyInt(v ? 1L : 0L) {}

bool PyBool::getBool() const {
    return this->getInt() != 0L;
}

void PyBool::registerMethods() {

    PyInt::registerMethods();

    this->define("__bool__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 1);

            return args[0]; // self
        })
    );

    this->define("__int__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 1);

            PyBool* self = args[0]->unwrap_bool_obj();

            bool value = self->getBool();

            return new PyInt(value ? 1L : 0L);
        })
    );

    this->define("__str__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 1);

            PyBool* self = args[0]->unwrap_bool_obj();

            bool value = self->getBool();

            return new PyStr(value ? "True" : "False");
        })
    );
}

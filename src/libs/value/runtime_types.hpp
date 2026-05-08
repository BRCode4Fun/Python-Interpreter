#pragma once

class Scope;
class Interpreter;
class PyTypeObject;

struct RuntimeTypes {
    PyTypeObject* object_type = nullptr;
    PyTypeObject* type_type = nullptr;
    PyTypeObject* none_type = nullptr;
    PyTypeObject* int_type = nullptr;
    PyTypeObject* float_type = nullptr;
    PyTypeObject* bool_type = nullptr;
    PyTypeObject* str_type = nullptr;
    PyTypeObject* function_type = nullptr;
    PyTypeObject* builtin_fn_type = nullptr;
    PyTypeObject* method_wrapper_type = nullptr;
};

RuntimeTypes initialize_runtime_types(Scope* builtins, Interpreter* interpreter);

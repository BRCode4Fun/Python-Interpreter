#include <iostream>
#include "./runtime_types.hpp"
#include "./py_object.hpp"
#include "./py_type_object.hpp"
#include "../interpreter/interpreter.hpp"
#include "../scope/scope.hpp"
#include "../exceptions/python_exceptions.hpp"

namespace {

PyObject* builtin_issubclass(std::vector<PyObject*>& args, Interpreter* runtime) {
    PyTypeObject* maybe_sub = args[0]->unwrap_type_obj();
    PyTypeObject* maybe_base = args[1]->unwrap_type_obj();
    return PyObject::create_bool(maybe_sub->is_subclass_of(maybe_base));
}

PyObject* builtin_isinstance(std::vector<PyObject*>& args, Interpreter* runtime) {
    PyTypeObject* tp = args[1]->unwrap_type_obj();
    return PyObject::create_bool(tp->is_instance(args[0]));
}

PyObject* builtin_input(std::vector<PyObject*>& args, Interpreter* runtime) {
    std::string buffer;
    std::getline(std::cin, buffer);
    return PyObject::create_string(buffer);
}

PyObject* builtin_repr(std::vector<PyObject*>& args, Interpreter* runtime) {
    PyObject* object = args[0];
    PyObject* method = object->find("__repr__");
    if(method != nullptr && method->is_fn_type()) {
        std::vector<PyObject*> no_args;
        PyObject* bound = method->bind_callable(object, nullptr, "__repr__");
        PyObject* rendered = bound->call_callable(no_args, runtime);
        runtime->track_temp(bound);
        if(rendered != nullptr && rendered->is_str_type()) {
            return rendered;
        }
    }
    throw TypeError("__repr__ must return str");
}

} // namespace

RuntimeTypes initialize_runtime_types(Scope* builtins, Interpreter* interpreter) {
    RuntimeTypes types;

    types.object_type = new PyTypeObject("object", nullptr, {});
    types.type_type = new PyTypeObject("type", nullptr, {types.object_type});
    types.object_type->set_type(types.type_type);
    types.type_type->set_type(types.type_type);

    types.none_type = new PyTypeObject("NoneType", types.type_type, {types.object_type});
    types.int_type = new PyTypeObject("int", types.type_type, {types.object_type});
    types.float_type = new PyTypeObject("float", types.type_type, {types.object_type});
    types.bool_type = new PyTypeObject("bool", types.type_type, {types.int_type});
    types.str_type = new PyTypeObject("str", types.type_type, {types.object_type});
    types.function_type = new PyTypeObject("function", types.type_type, {types.object_type});
    types.builtin_fn_type = new PyTypeObject("builtin_function", types.type_type, {types.object_type});
    types.method_wrapper_type = new PyTypeObject("method_wrapper", types.type_type, {types.object_type});

    PyObject::register_core_types(
        types.none_type,
        types.int_type,
        types.float_type,
        types.bool_type,
        types.str_type,
        types.function_type,
        types.builtin_fn_type,
        types.method_wrapper_type
    );

    types.object_type->ensure_methods_registered();
    types.type_type->ensure_methods_registered();
    types.none_type->ensure_methods_registered();
    types.int_type->ensure_methods_registered();
    types.float_type->ensure_methods_registered();
    types.bool_type->ensure_methods_registered();
    types.str_type->ensure_methods_registered();
    types.function_type->ensure_methods_registered();
    types.builtin_fn_type->ensure_methods_registered();
    types.method_wrapper_type->ensure_methods_registered();

    builtins->define("object", types.object_type);
    builtins->define("type", types.type_type);
    //builtins->define("NoneType", types.none_type);
    builtins->define("int", types.int_type);
    builtins->define("float", types.float_type);
    builtins->define("bool", types.bool_type);
    builtins->define("str", types.str_type);

    builtins->define("issubclass", PyObject::create_native_callable(
        builtin_issubclass, 2, types.builtin_fn_type
    ));

    builtins->define("isinstance", PyObject::create_native_callable(
        builtin_isinstance, 2, types.builtin_fn_type
    ));

    builtins->define("input", PyObject::create_native_callable(
        builtin_input, 0, types.builtin_fn_type
    ));
    builtins->define("repr", PyObject::create_native_callable(
        builtin_repr, 1, types.builtin_fn_type
    ));

    return types;
}

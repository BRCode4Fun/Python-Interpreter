#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <functional>

class Scope;
class Interpreter;
class PyTypeObject;
class FunctionNode;

using llf = long double;
using lld = long long int;

class PyObject {

public:
    PyObject(
        PyTypeObject* type_obj = nullptr,
        void* data = nullptr,
        Scope* context = nullptr
    );
    virtual ~PyObject();
    
    virtual inline bool is_str_type() const { return this->ob_type == str_type_ref; }
    /** Exact runtime value None is the singleton (`none_singleton_ref`), not mere NoneType semantics. */
    virtual inline bool is_none_type() const { return this == none_singleton_ref; }
    virtual inline bool is_fn_type() const { return this->ob_type == function_type_ref; }
    virtual inline bool is_method_wrapper_type() const { return this->ob_type == method_wrapper_type_ref; }
    virtual inline bool is_int_type() const { return this->ob_type == int_type_ref; }
    virtual inline bool is_float_type() const { return this->ob_type == float_type_ref; }
    virtual inline bool is_bool_type() const { return this->ob_type == bool_type_ref; }
    virtual inline bool is_klass_type() const { return this->is_type_type(); }
    virtual inline bool is_instance_type() const { return this->get_type() != nullptr && !this->is_type_type(); }
    virtual inline bool is_builtin_fn_type() const { return this->ob_type == builtin_fn_type_ref; }
    virtual inline bool is_callable_type() const { return this->is_fn_type() || this->is_builtin_fn_type() || this->is_method_wrapper_type(); }
    virtual inline bool is_type_type() const { return false; }

    void incRefCount();
    void decRefCount();
    int getRefCount();
    
    Scope* getContext();
    const Scope* getContext() const;
    void set_type(PyTypeObject* type_obj);
    PyTypeObject* get_type() const;
    
    void define(const std::string&, PyObject*);
    virtual PyObject* find(const std::string&);

    static PyObject* create_none(PyTypeObject* none_type = nullptr);
    static PyObject* create_int(lld value, PyTypeObject* int_type = nullptr);
    static PyObject* create_float(llf value, PyTypeObject* float_type = nullptr);
    static PyObject* create_bool(bool value, PyTypeObject* bool_type = nullptr);
    static PyObject* create_string(const std::string& value, PyTypeObject* str_type = nullptr);

    lld as_int() const;
    llf as_float() const;
    bool as_bool() const;
    const std::string& as_string() const;

    class PyObject* unwrap_str_obj();
    class PyObject* unwrap_int_obj();
    class PyObject* unwrap_float_obj();
    class PyObject* unwrap_bool_obj();
    class PyTypeObject* unwrap_class_obj();
    class PyTypeObject* unwrap_type_obj();

    static PyObject* create_native_callable(
        std::function<PyObject*(std::vector<PyObject*>&, Interpreter*)> functor,
        size_t arity,
        PyTypeObject* callable_type
    );
    static PyObject* create_user_function(
        FunctionNode* declaration,
        Scope* closure,
        PyTypeObject* function_type
    );
    PyObject* bind_callable(
        PyObject* instance,
        PyTypeObject* method_wrapper_type,
        const std::string& wrapper_attribute_name
    );
    PyObject* call_callable(std::vector<PyObject*>& args, Interpreter* interpreter);
    size_t expected_call_arity() const;

    /** Payload declaration when this is a user-defined function object; otherwise nullptr. */
    FunctionNode* callable_user_declaration() const;

    /** When this is a bound method wrapper, fills attribute name and bound receiver; returns false otherwise. */
    bool try_callable_bound_method(std::string* attribute_name, PyObject** bound_owner) const;

    static void register_core_types(
        PyTypeObject* none_type,
        PyTypeObject* int_type,
        PyTypeObject* float_type,
        PyTypeObject* bool_type,
        PyTypeObject* str_type,
        PyTypeObject* function_type,
        PyTypeObject* builtin_fn_type,
        PyTypeObject* method_wrapper_type
    );

protected:
    int rc; // reference counting, for garbage collection

    void* data; // internal data for the object
    virtual void deleteData();
    
    // each object has its own scope and type
    Scope* scope;
    PyTypeObject* ob_type = nullptr; 

    // core types
    static PyTypeObject* none_type_ref;
    static PyTypeObject* int_type_ref;
    static PyTypeObject* float_type_ref;
    static PyTypeObject* bool_type_ref;
    static PyTypeObject* str_type_ref;
    static PyTypeObject* function_type_ref;
    static PyTypeObject* builtin_fn_type_ref;
    static PyTypeObject* method_wrapper_type_ref;

    // singleton objects
    static PyObject* none_singleton_ref;
    static PyObject* true_singleton_ref;
    static PyObject* false_singleton_ref;

    // for function and callable objects
    struct CallableData {
        enum class Kind {
            Native,
            UserDefined,
            BoundMethod
        };

        Kind kind = Kind::Native;
        std::function<PyObject*(std::vector<PyObject*>&, Interpreter*)> native;
        FunctionNode* declaration = nullptr;
        Scope* closure = nullptr;
        PyObject* bound_instance = nullptr;
        std::string wrapper_attribute_name;
        size_t arity = 0;
    };

    virtual void registerMethods(){}
};

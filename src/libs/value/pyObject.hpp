#pragma once

#include <vector>
#include <string>
#include <sstream>

#define ASSERT_ARG_SIZE(args, expected_size) \
    do { \
        if ((args).size() != (expected_size)) { \
            std::ostringstream oss; \
            oss << "expected " << (expected_size) << ", got " << (args).size(); \
            throw std::logic_error(oss.str()); \
        } \
    } while (0)

class Scope;
class Interpreter;

using llf = long double;
using lld = long long int;

class PyObject {

public:
    enum class ObjectType {
        None, 
        Int, Float,
        Boolean, String,
        List, Func, Method,
        Klass, Instance,
        Builtin
    };
    
    PyObject(ObjectType, void* data = nullptr, Scope* context = nullptr);
    ~PyObject();
    
    virtual inline bool is_str_type() const { return false; }
    virtual inline bool is_none_type() const { return false; }
    virtual inline bool is_fn_type() const { return false; }
    virtual inline bool is_method_wrapper_type() const { return false; }
    virtual inline bool is_int_type() const { return false; }
    virtual inline bool is_float_type() const { return false; }
    virtual inline bool is_bool_type() const { return false; }
    virtual inline bool is_klass_type() const { return false; }
    virtual inline bool is_instance_type() const { return false; }
    virtual inline bool is_builtin_fn_type() const { return false; }

    void incRefCount();
    void decRefCount();
    int getRefCount();
    
    Scope* getContext();
    
    void define(const std::string&, PyObject*);
    PyObject* find(const std::string&);

    class PyStr* unwrap_str_obj();
    class PyInt* unwrap_int_obj();
    class PyFloat* unwrap_float_obj();
    class PyBool* unwrap_bool_obj();
    class PyFunction* unwrap_function_obj();
    class PyMethodWrapper* unwrap_method_obj();
    class PyClass* unwrap_class_obj();
    class PyBuiltin* unwrap_builtin_fn_obj();

protected:
    ObjectType type;
    int rc; // reference counting

    void* data;
    virtual void deleteData() {}
    
    Scope* scope;
    virtual void registerMethods(){}
};

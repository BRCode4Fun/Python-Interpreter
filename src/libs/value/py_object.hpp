#pragma once

#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

#include "../scope/scope.hpp"

class Interpreter;
class PyTypeObject;
class FunctionNode;

using llf = long double;
using lld = long long int;

/*
 * Memory model (high level):
 *
 * - PyObject lifetime is manual `new`/`delete` plus an explicit ref-count field (`rc`). Scopes and
 *   the interpreter bump `rc` when a name binds to a value; `Scope::~Scope` and redefinitions
 *   call `dec_ref_count`. Values with `rc == 0` are not automatically freed unless they were
 *   registered with the interpreter's `GarbageCollector` (see `Interpreter::track_temp` /
 *   `maybe_collect`) — there is no cycle-tracing collector.
 *
 * - Each object owns its attribute/namespace table via `std::unique_ptr<Scope> scope` (RAII).
 *   Payload primitives and callables store extra data in `void* data` and free it in `delete_data`.
 *
 * - Callables use `CallableData`: nested functions may `clone_chain` the lexical environment into
 *   `closure_owned`; top-level functions may keep `closure_borrowed` pointing at a live frame that
 *   outlives the function only when that invariant holds at the call site.
 */
class PyObject {

public:
    using NativeCallableFn = PyObject* (*)(std::vector<PyObject*>&, Interpreter*);

    PyObject(PyTypeObject* type_obj = nullptr, void* data = nullptr);
    PyObject(PyTypeObject* type_obj, void* data, std::unique_ptr<Scope> namespace_scope);
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

    void inc_ref_count();
    void dec_ref_count();
    int get_ref_count();
    
    Scope* get_context();
    const Scope* get_context() const;
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
        NativeCallableFn function_ptr,
        size_t arity,
        PyTypeObject* callable_type
    );
    static PyObject* create_user_function(
        FunctionNode* declaration,
        Scope* closure,
        PyTypeObject* function_type,
        /** If true, heap-own a snapshot of the scope chain (nested defs); if false, borrow closure (module-level). */
        bool snapshot_closure_environment
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

    /** Clears static type/singleton pointers after runtime teardown (e.g. interpreter shutdown). */
    static void clear_runtime_statics();

protected:
    /** Interpreter / Scope adjusted refcount; GC may delete when this hits 0 (if the object was tracked). */
    int rc;

    /** Type-specific heap payload (primitives, CallableData, etc.); released in `delete_data`. */
    void* data;
    virtual void delete_data();

    /** Per-object namespace (attributes, class/instance dict); owned for the object's lifetime. */
    std::unique_ptr<Scope> scope;
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

    /** Heap-allocated callable payload; owned by the PyObject via `data` and deleted in `delete_data`. */
    struct CallableData {
        enum class Kind {
            Native,
            UserDefined,
            BoundMethod
        };

        Kind kind = Kind::Native;
        NativeCallableFn native = nullptr;
        FunctionNode* declaration = nullptr;
        /** Snapshot of lexical Scope chain (owns clones); used when the function must outlive its frame. */
        std::unique_ptr<Scope> closure_owned;
        /** Non-owning view of a still-live frame; caller must guarantee lifetime (e.g. module-level). */
        Scope* closure_borrowed = nullptr;
        Scope* closure_env() const {
            return closure_owned ? closure_owned.get() : closure_borrowed;
        }
        PyObject* bound_instance = nullptr;
        std::string wrapper_attribute_name;
        size_t arity = 0;
    };

    virtual void register_methods(){}
};

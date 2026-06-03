#include <cstdint>
#include <stdexcept>
#include "py_object.hpp"
#include "py_type_object.hpp"
#include "../ast/ast.hpp"
#include "../exceptions/runtime_exceptions.hpp"
#include "../exceptions/python_exceptions.hpp"
#include "../interpreter/interpreter.hpp"
#include "../scope/scope.hpp"

/*
 * PyObject allocation: most values are `new PyObject(...)` with `rc` starting at 0. Call sites
 * bind them into a `Scope` (which increments `rc`) or hand them to the GC via `track_temp` so a
 * later sweep can `delete` unreachable objects. Singletons (`None`, booleans) are created once,
 * bumped once, and never collected through the normal path.
 */

PyTypeObject* PyObject::none_type_ref = nullptr;
PyTypeObject* PyObject::int_type_ref = nullptr;
PyTypeObject* PyObject::float_type_ref = nullptr;
PyTypeObject* PyObject::bool_type_ref = nullptr;
PyTypeObject* PyObject::str_type_ref = nullptr;
PyTypeObject* PyObject::function_type_ref = nullptr;
PyTypeObject* PyObject::builtin_fn_type_ref = nullptr;
PyTypeObject* PyObject::method_wrapper_type_ref = nullptr;

PyObject* PyObject::none_singleton_ref = nullptr;
PyObject* PyObject::true_singleton_ref = nullptr;
PyObject* PyObject::false_singleton_ref = nullptr;

PyObject::PyObject(PyTypeObject* type_obj, void* data)
    : rc(0),
      data(data),
      ob_type(type_obj),
      scope(std::make_unique<Scope>()) {}

PyObject::PyObject(PyTypeObject* type_obj, void* data, std::unique_ptr<Scope> namespace_scope)
    : rc(0), data(data), ob_type(type_obj) {
    if(namespace_scope) {
        scope = std::move(namespace_scope);
    } else {
        scope = std::make_unique<Scope>();
    }
}

PyObject::~PyObject() {
    /* Break static singleton/type back-pointers before releasing `data` / implicit `scope` dtor. */
    if(this == none_singleton_ref) {
        none_singleton_ref = nullptr;
    }
    if(this == true_singleton_ref) {
        true_singleton_ref = nullptr;
    }
    if(this == false_singleton_ref) {
        false_singleton_ref = nullptr;
    }
    delete_data();
}

void PyObject::delete_data() {
    if(this->data == nullptr) {
        return;
    }

    /* Each branch matches how `create_*` allocated `data`; must stay in sync with constructors. */
    if(this->is_int_type()) {
        delete static_cast<lld*>(this->data);
    } else if(this->is_float_type()) {
        delete static_cast<llf*>(this->data);
    } else if(this->is_bool_type()) {
        delete static_cast<bool*>(this->data);
    } else if(this->is_str_type()) {
        delete static_cast<std::string*>(this->data);
    } else if(this->is_fn_type() || this->is_builtin_fn_type() || this->is_method_wrapper_type()) {
        CallableData* payload = static_cast<CallableData*>(this->data);
        if(payload != nullptr) {
            /* Drop owned closure tree; borrowed pointer is non-owning — do not delete the frame. */
            payload->closure_owned.reset();
            payload->closure_borrowed = nullptr;
            delete payload;
        }
    }
    this->data = nullptr;
}

/* Refcount is authoritative for "in use" from the Scope/binding perspective; GC uses it at sweep time. */
void PyObject::inc_ref_count(){ ++this->rc; }
void PyObject::dec_ref_count() { if(rc > 0) --this->rc; }
int PyObject::get_ref_count() { return this->rc; }

Scope* PyObject::get_context() { return this->scope.get(); }
const Scope* PyObject::get_context() const { return this->scope.get(); }

void PyObject::set_type(PyTypeObject* type_obj) { this->ob_type = type_obj; }
PyTypeObject* PyObject::get_type() const { return this->ob_type; }

void PyObject::register_core_types(
    PyTypeObject* none_type,
    PyTypeObject* int_type,
    PyTypeObject* float_type,
    PyTypeObject* bool_type,
    PyTypeObject* str_type,
    PyTypeObject* function_type,
    PyTypeObject* builtin_fn_type,
    PyTypeObject* method_wrapper_type
) {
    none_type_ref = none_type;
    int_type_ref = int_type;
    float_type_ref = float_type;
    bool_type_ref = bool_type;
    str_type_ref = str_type;
    function_type_ref = function_type;
    builtin_fn_type_ref = builtin_fn_type;
    method_wrapper_type_ref = method_wrapper_type;

    if(none_singleton_ref != nullptr && none_type_ref != nullptr) {
        none_singleton_ref->set_type(none_type_ref);
    }
    if(true_singleton_ref != nullptr && bool_type_ref != nullptr) {
        true_singleton_ref->set_type(bool_type_ref);
    }
    if(false_singleton_ref != nullptr && bool_type_ref != nullptr) {
        false_singleton_ref->set_type(bool_type_ref);
    }
}

/* Called after VM Scope teardown — global runtime refs must no longer be used. */
void PyObject::clear_runtime_statics() {
    none_type_ref = nullptr;
    int_type_ref = nullptr;
    float_type_ref = nullptr;
    bool_type_ref = nullptr;
    str_type_ref = nullptr;
    function_type_ref = nullptr;
    builtin_fn_type_ref = nullptr;
    method_wrapper_type_ref = nullptr;
    none_singleton_ref = nullptr;
    true_singleton_ref = nullptr;
    false_singleton_ref = nullptr;
}

void PyObject::define(const std::string& name, PyObject* object) {
    this->scope->define(name, object);
}

PyObject* PyObject::find(const std::string& name) {
    if(this->scope->empty()) {
        this->register_methods();
    }

    try {
        return this->scope->get(name);
    } catch(std::runtime_error&) {
        if(this->ob_type != nullptr && this->ob_type != this) {
            return this->ob_type->find(name);
        }
        return nullptr;
    }
}

PyObject* PyObject::create_none(PyTypeObject* none_type) {
    PyTypeObject* resolved = none_type != nullptr ? none_type : none_type_ref;
    if(none_singleton_ref == nullptr) {
        none_singleton_ref = new PyObject(resolved, nullptr);
        none_singleton_ref->inc_ref_count();
    } else if(resolved != nullptr && none_singleton_ref->ob_type != resolved) {
        none_singleton_ref->set_type(resolved);
    }
    return none_singleton_ref;
}

PyObject* PyObject::create_int(lld value, PyTypeObject* int_type) {
    return new PyObject(int_type ? int_type : int_type_ref, new lld(value));
}

PyObject* PyObject::create_float(llf value, PyTypeObject* float_type) {
    return new PyObject(float_type ? float_type : float_type_ref, new llf(value));
}

PyObject* PyObject::create_bool(bool value, PyTypeObject* bool_type) {
    PyObject*& singleton = value ? true_singleton_ref : false_singleton_ref;
    if(singleton == nullptr) {
        singleton = new PyObject(bool_type ? bool_type : bool_type_ref, new bool(value));
        singleton->inc_ref_count();
    } else if(singleton->get_type() == nullptr && (bool_type != nullptr || bool_type_ref != nullptr)) {
        singleton->set_type(bool_type ? bool_type : bool_type_ref);
    }
    return singleton;
}

PyObject* PyObject::create_string(const std::string& value, PyTypeObject* str_type) {
    return new PyObject(str_type ? str_type : str_type_ref, new std::string(value));
}

PyObject* PyObject::create_native_callable(
    NativeCallableFn function_ptr,
    size_t arity,
    PyTypeObject* callable_type
) {
    CallableData* payload = new CallableData();
    payload->kind = CallableData::Kind::Native;
    payload->native = function_ptr;
    payload->arity = arity;
    return new PyObject(callable_type, payload);
}

PyObject* PyObject::create_user_function(
    FunctionNode* declaration,
    Scope* closure,
    PyTypeObject* function_type,
    bool snapshot_closure_environment
) {
    CallableData* payload = new CallableData();
    payload->kind = CallableData::Kind::UserDefined;
    payload->declaration = declaration;
    if(snapshot_closure_environment) {
        /* Own a deep copy so the callable survives after the interpreter pops the defining frame. */
        payload->closure_owned = Scope::clone_chain(closure);
    } else {
        /* Caller keeps the enclosing Scope alive (e.g. module globals) for the borrow to stay valid. */
        payload->closure_borrowed = closure;
    }
    payload->arity = declaration->get_params().size();
    return new PyObject(function_type ? function_type : function_type_ref, payload);
}

PyObject* PyObject::bind_callable(
    PyObject* instance,
    PyTypeObject* method_wrapper_type,
    const std::string& wrapper_attribute_name
) {
    if(!this->is_fn_type()) {
        throw TypeError("Object is not a function");
    }

    CallableData* source = static_cast<CallableData*>(this->data);
    CallableData* bound = new CallableData();
    bound->kind = CallableData::Kind::BoundMethod;
    bound->native = source->native;
    bound->declaration = source->declaration;
    bound->closure_owned.reset();
    bound->closure_borrowed = source->closure_env();
    bound->arity = source->arity;
    bound->bound_instance = instance;
    bound->wrapper_attribute_name = wrapper_attribute_name;
    return new PyObject(method_wrapper_type ? method_wrapper_type : method_wrapper_type_ref, bound);
}

PyObject* PyObject::call_callable(std::vector<PyObject*>& args, Interpreter* interpreter) {
    if(!this->is_callable_type()) {
        throw TypeError("Object is not callable");
    }

    CallableData* payload = static_cast<CallableData*>(this->data);
    if(payload == nullptr) {
        throw RuntimeError("Invalid callable payload");
    }

    /* BoundMethod needs [receiver] + user args. 
     * Build bound_args locally: avoid mutating the caller's vector 
     * (insert(begin,…) shifts everything O(n) and leaves stale extra slots if reused). */
    std::vector<PyObject*> bound_args;
    std::vector<PyObject*>* call_args = &args;
    const bool prepend_receiver =
        payload->kind == CallableData::Kind::BoundMethod && payload->bound_instance != nullptr;
    if(prepend_receiver) {
        bound_args.reserve(args.size() + 1u);
        bound_args.push_back(payload->bound_instance);
        bound_args.insert(bound_args.end(), args.begin(), args.end());
        call_args = &bound_args;
    }

    const bool is_native_call =
        payload->kind == CallableData::Kind::Native
        || (payload->kind == CallableData::Kind::BoundMethod && static_cast<bool>(payload->native));

    if(is_native_call) {
        PyObject* ret =
            payload->native ? payload->native(*call_args, interpreter) : PyObject::create_none();
        if(interpreter != nullptr && ret != nullptr) {
            interpreter->track_temp(ret);
        }
        return ret;
    }

    if(payload->declaration == nullptr) {
        PyObject* ret = PyObject::create_none();
        if(interpreter != nullptr) {
            interpreter->track_temp(ret);
        }
        return ret;
    }

    const std::vector<std::unique_ptr<AstNode>>& params = payload->declaration->get_params();

    interpreter->push_owned(std::make_unique<Scope>(payload->closure_env()));
    for(size_t idx = 0; idx < params.size(); ++idx) {
        const std::string& param_name = params[idx]->unwrap_name_node()->get_lexeme();
        interpreter->define_on_context(param_name, (*call_args)[idx]);
    }

    PyObject* ret_value = nullptr;
    try {
        payload->declaration->get_body()->accept(interpreter);
    } catch(ReturnException& re) {
        ret_value = re.value;
    }
    interpreter->pop_context();
    PyObject* ret = ret_value ? ret_value : PyObject::create_none();
    if(interpreter != nullptr && ret != nullptr) {
        interpreter->track_temp(ret);
    }
    return ret;
}

size_t PyObject::expected_call_arity() const {
    if(!this->is_callable_type()) {
        throw TypeError("Object is not callable");
    }
    const CallableData* payload = static_cast<const CallableData*>(this->data);
    if(payload == nullptr) {
        throw RuntimeError("Invalid callable payload");
    }

    size_t arity = payload->arity;
    if(payload->kind == CallableData::Kind::BoundMethod && arity > 0) {
        arity -= 1;
    }
    return arity;
}

FunctionNode* PyObject::callable_user_declaration() const {
    if(!this->is_fn_type()) {
        return nullptr;
    }
    const CallableData* payload = static_cast<const CallableData*>(this->data);
    if(payload == nullptr || payload->kind != CallableData::Kind::UserDefined) {
        return nullptr;
    }
    return payload->declaration;
}

bool PyObject::try_callable_bound_method(std::string* attribute_name, PyObject** bound_owner) const {
    if(!this->is_method_wrapper_type()) {
        return false;
    }
    const CallableData* payload = static_cast<const CallableData*>(this->data);
    if(payload == nullptr || payload->kind != CallableData::Kind::BoundMethod) {
        return false;
    }
    if(attribute_name != nullptr) {
        *attribute_name = payload->wrapper_attribute_name;
    }
    if(bound_owner != nullptr) {
        *bound_owner = payload->bound_instance;
    }
    return true;
}

lld PyObject::as_int() const {
    if(!this->is_int_type()) {
        throw TypeError("Object is not an integer");
    }
    return *static_cast<lld*>(this->data);
}

llf PyObject::as_float() const {
    if(!this->is_float_type()) {
        throw TypeError("Object is not a float");
    }
    return *static_cast<llf*>(this->data);
}

bool PyObject::as_bool() const {
    if(!this->is_bool_type()) {
        throw TypeError("Object is not a boolean");
    }
    return *static_cast<bool*>(this->data);
}

const std::string& PyObject::as_string() const {
    if(!this->is_str_type()) {
        throw TypeError("Object is not a string");
    }
    return *static_cast<std::string*>(this->data);
}

PyObject* PyObject::unwrap_str_obj(){
    if(this->is_str_type()){
        return this;
    }
    throw TypeError("Object is not a string");
}

PyObject* PyObject::unwrap_int_obj(){
    if(this->is_int_type()){
        return this;
    }
    throw TypeError("Object is not an integer");
}

PyObject* PyObject::unwrap_float_obj(){
    if(this->is_float_type()){
        return this;
    }
    throw TypeError("Object is not a float");
}

PyObject* PyObject::unwrap_bool_obj(){
    if(this->is_bool_type()){
        return this;
    }
    throw TypeError("Object is not a boolean");
}

class PyTypeObject* PyObject::unwrap_type_obj(){
    if(this->is_type_type()){
        return dynamic_cast<PyTypeObject*>(this);
    }
    throw TypeError("Object is not a type");
}

class PyTypeObject* PyObject::unwrap_class_obj(){
    if(this->is_klass_type()){
        return dynamic_cast<PyTypeObject*>(this);
    }
    throw TypeError("Object is not a class");
}

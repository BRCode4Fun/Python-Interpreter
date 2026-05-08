#include <cstdint>
#include <stdexcept>

#include "./pyObject.hpp"
#include "./pyTypeObject.hpp"
#include "../ast/ast.hpp"
#include "../exceptions/runtime_exceptions.hpp"
#include "../exceptions/python_errors.hpp"
#include "../interpreter/interpreter.hpp"
#include "../scope/scope.hpp"

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

PyObject::PyObject(PyTypeObject* type_obj, void* data, Scope* context)
    : rc(0), data(data), ob_type(type_obj) {
    this->scope = context ? context : new Scope();
}

PyObject::~PyObject() {
    deleteData();
}

void PyObject::deleteData() {
    if(this->data == nullptr) {
        return;
    }

    if(this->is_int_type()) {
        delete static_cast<lld*>(this->data);
    } else if(this->is_float_type()) {
        delete static_cast<llf*>(this->data);
    } else if(this->is_bool_type()) {
        delete static_cast<bool*>(this->data);
    } else if(this->is_str_type()) {
        delete static_cast<std::string*>(this->data);
    } else if(this->is_fn_type() || this->is_builtin_fn_type() || this->is_method_wrapper_type()) {
        delete static_cast<CallableData*>(this->data);
    }
    this->data = nullptr;
}

void PyObject::incRefCount(){ ++this->rc; }
void PyObject::decRefCount() { if(rc > 0) --this->rc; }
int PyObject::getRefCount() { return this->rc; }

Scope* PyObject::getContext() { return this->scope; }
const Scope* PyObject::getContext() const { return this->scope; }

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

void PyObject::define(const std::string& name, PyObject* object) {
    this->scope->define(name, object);
}

PyObject* PyObject::find(const std::string& name) {
    if(this->scope->empty()) {
        this->registerMethods();
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
        none_singleton_ref->incRefCount();
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
        singleton->incRefCount();
    } else if(singleton->get_type() == nullptr && (bool_type != nullptr || bool_type_ref != nullptr)) {
        singleton->set_type(bool_type ? bool_type : bool_type_ref);
    }
    return singleton;
}

PyObject* PyObject::create_string(const std::string& value, PyTypeObject* str_type) {
    return new PyObject(str_type ? str_type : str_type_ref, new std::string(value));
}

PyObject* PyObject::create_native_callable(
    std::function<PyObject*(std::vector<PyObject*>&, Interpreter*)> functor,
    size_t arity,
    PyTypeObject* callable_type
) {
    CallableData* payload = new CallableData();
    payload->kind = CallableData::Kind::Native;
    payload->native = functor;
    payload->arity = arity;
    return new PyObject(callable_type, payload);
}

PyObject* PyObject::create_user_function(FunctionNode* declaration, Scope* closure, PyTypeObject* function_type) {
    CallableData* payload = new CallableData();
    payload->kind = CallableData::Kind::UserDefined;
    payload->declaration = declaration;
    payload->closure = closure;
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
    bound->closure = source->closure;
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

    if(payload->kind == CallableData::Kind::BoundMethod && payload->bound_instance != nullptr) {
        args.insert(args.begin(), payload->bound_instance);
    }

    const bool is_native_call =
        payload->kind == CallableData::Kind::Native
        || (payload->kind == CallableData::Kind::BoundMethod && static_cast<bool>(payload->native));

    if(is_native_call) {
        return payload->native ? payload->native(args, interpreter) : PyObject::create_none();
    }

    if(payload->declaration == nullptr) {
        return PyObject::create_none();
    }

    const std::vector<AstNode*>& params = payload->declaration->get_params();

    Scope* fnCallEnv = new Scope(payload->closure);
    interpreter->pushContext(fnCallEnv);
    for(size_t idx = 0; idx < params.size(); ++idx) {
        const std::string& paramName = params[idx]->unwrap_name_node()->get_lexeme();
        interpreter->defineOnContext(paramName, args[idx]);
    }

    PyObject* retValue = nullptr;
    try {
        payload->declaration->get_body()->accept(interpreter);
    } catch(ReturnException& re) {
        retValue = re.value;
    }
    interpreter->popContext();
    return retValue ? retValue : PyObject::create_none();
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

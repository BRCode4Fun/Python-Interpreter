#include <stdexcept>
#include <sstream>
#include <cstdint>
#include "./pyTypeObject.hpp"
#include "../ast/ast.hpp"
#include "../exceptions/python_errors.hpp"
#include "../scope/scope.hpp"

namespace {

std::string object_instance_display(PyObject* self) {
    PyTypeObject* tp = self->get_type();
    std::string type_name = tp != nullptr ? tp->get_name() : "object";
    std::ostringstream oss;
    oss << "<" << type_name << " object at 0x"
        << std::hex << reinterpret_cast<std::uintptr_t>(self)
        << ">";
    return oss.str();
}

std::string type_class_display(PyObject* self) {
    PyTypeObject* type_obj = self->unwrap_type_obj();
    return "<class '" + type_obj->get_name() + "'>";
}

std::string none_display(PyObject* self) {
    (void)self;
    return "None";
}

std::string int_display(PyObject* self) {
    return std::to_string(self->as_int());
}

std::string float_display(PyObject* self) {
    llf number = self->as_float();
    std::ostringstream oss;
    lld intPart = static_cast<lld>(number);
    llf fracPart = number - intPart;
    if(fracPart == 0.0L) {
        oss << intPart << ".0";
    } else {
        oss << number;
    }
    return oss.str();
}

std::string bool_display(PyObject* self) {
    return self->as_bool() ? "True" : "False";
}

std::string str_display(PyObject* self) {
    return self->as_string();
}

std::string str_repr_display(PyObject* self) {
    const std::string value = self->as_string();
    std::string escaped;
    escaped.reserve(value.size() + 2);
    for(char ch : value) {
        if(ch == '\\' || ch == '\'') {
            escaped.push_back('\\');
            escaped.push_back(ch);
            continue;
        }
        if(ch == '\n') {
            escaped += "\\n";
            continue;
        }
        if(ch == '\t') {
            escaped += "\\t";
            continue;
        }
        escaped.push_back(ch);
    }
    return "'" + escaped + "'";
}

std::string function_object_display(PyObject* self) {
    std::string label = "<lambda>";
    FunctionNode* decl = self->callable_user_declaration();
    if(decl != nullptr) {
        label = decl->get_name();
    }
    std::ostringstream oss;
    oss << "<function " << label << " at 0x"
        << std::hex << reinterpret_cast<std::uintptr_t>(self)
        << ">";
    return oss.str();
}

std::string method_wrapper_object_display(PyObject* self) {
    std::string attr_raw;
    PyObject* owner = nullptr;
    if(!self->try_callable_bound_method(&attr_raw, &owner)) {
        throw RuntimeError("Invalid method wrapper payload");
    }
    const std::string attr_display = attr_raw.empty() ? "?" : attr_raw;
    std::string owner_type_name = "?";
    if(owner != nullptr) {
        if(owner->is_type_type()) {
            owner_type_name = static_cast<PyTypeObject*>(owner)->get_name();
        } else {
            PyTypeObject* tp = owner->get_type();
            owner_type_name = tp != nullptr ? tp->get_name() : std::string("object");
        }
    }
    std::ostringstream out;
    out << "<method-wrapper '" << attr_display << "' of " << owner_type_name << " object at 0x"
        << std::hex << reinterpret_cast<std::uintptr_t>(self)
        << ">";
    return out.str();
}

} // namespace

PyTypeObject::PyTypeObject(
    const std::string& name,
    PyTypeObject* metatype,
    const std::vector<PyTypeObject*>& bases,
    Scope* context
) : PyObject(metatype, nullptr, context), type_name(name), bases(bases) {}

PyObject* PyTypeObject::call(std::vector<PyObject*>& args, Interpreter* interpreter) {
    this->ensure_methods_registered();

    PyObject* new_method = this->find("__new__");
    if(new_method != nullptr && new_method->is_fn_type() && interpreter != nullptr) {
        PyObject* bound_new = new_method->bind_callable(this, method_wrapper_type_ref, "__new__");
        std::vector<PyObject*> constructor_args = args;
        PyObject* instance = bound_new->call_callable(constructor_args, interpreter);
        if(instance == nullptr) {
            throw RuntimeError("Type '" + this->type_name + "' returned null from __new__");
        }

        PyObject* init_method = this->find("__init__");
        if(init_method != nullptr && init_method->is_fn_type()) {
            PyObject* bound_init = init_method->bind_callable(instance, method_wrapper_type_ref, "__init__");
            std::vector<PyObject*> init_args = args;
            bound_init->call_callable(init_args, interpreter);
        }
        return instance;
    }

    throw TypeError("Type '" + this->type_name + "' is not callable");
}

void PyTypeObject::ensure_methods_registered() {
    if(this->scope->empty()) {
        this->registerMethods();
    }
}

PyObject* PyTypeObject::instantiate() const {
    PyObject* instance = new PyObject(
        const_cast<PyTypeObject*>(this),
        nullptr,
        new Scope(const_cast<Scope*>(this->getContext()))
    );
    return instance;
}

bool PyTypeObject::is_subclass_of(const PyTypeObject* candidate_base) const {
    if(candidate_base == nullptr) {
        return false;
    }
    if(this == candidate_base) {
        return true;
    }
    for(const PyTypeObject* base : this->bases) {
        if(base != nullptr && base->is_subclass_of(candidate_base)) {
            return true;
        }
    }
    return false;
}

bool PyTypeObject::is_instance(PyObject* obj) const {
    if(obj == nullptr || obj->get_type() == nullptr) {
        return false;
    }
    return obj->get_type()->is_subclass_of(this);
}

PyObject* PyTypeObject::find(const std::string& name) {
    if(this->scope->empty()) {
        this->registerMethods();
    }

    try {
        return this->scope->get(name);
    } catch(const std::runtime_error&) {
        for(PyTypeObject* base : this->bases) {
            if(base == nullptr) {
                continue;
            }
            PyObject* value = base->find(name);
            if(value != nullptr) {
                return value;
            }
        }
    }
    return nullptr;
}

void PyTypeObject::registerMethods() {
    if(this->type_name == "object") {
        this->define("__new__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                PyTypeObject* cls = args[0]->unwrap_type_obj();
                return cls->instantiate();
            }, 1, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_none(none_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(object_instance_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__str__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(object_instance_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__eq__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_bool(args[0] == args[1], bool_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__ne__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_bool(args[0] != args[1], bool_type_ref);
            }, 2, function_type_ref
        ));
        return;
    }

    if(this->type_name == "type") {
        this->define("__new__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                PyTypeObject* type = args[1]->get_type();
                if(type == nullptr) {
                    throw TypeError("object has no runtime type");
                }
                return type;
            }, 2, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_none(none_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(type_class_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__str__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(type_class_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
        return;
    }

    if(this->type_name == "NoneType") {
        this->define("__new__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                throw TypeError("cannot instantiate 'NoneType' directly");
            }, 1, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_none(none_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__str__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(none_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(none_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__bool__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_bool(false, bool_type_ref);
            }, 1, function_type_ref
        ));
        return;
    }

    if(this->type_name == "int") {
        this->define("__new__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                PyObject* source = args[1];
                if(source->is_int_type()) return PyObject::create_int(source->as_int(), int_type_ref);
                if(source->is_bool_type()) return PyObject::create_int(source->as_bool() ? 1LL : 0LL, int_type_ref);
                if(source->is_float_type()) return PyObject::create_int(static_cast<lld>(source->as_float()), int_type_ref);
                throw TypeError("unsupported conversion to int");
            }, 2, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_none(none_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__int__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                if(args[0]->is_int_type()) {
                    return args[0];
                }
                return PyObject::create_int(args[0]->as_int(), int_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__bool__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_bool(args[0]->as_int() != 0LL, bool_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__str__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(int_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(int_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__add__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
                PyObject* other = args[1];
                if(other->is_int_type()) return PyObject::create_int(lhs + other->as_int(), int_type_ref);
                if(other->is_bool_type()) return PyObject::create_int(lhs + (other->as_bool() ? 1LL : 0LL), int_type_ref);
                if(other->is_float_type()) return PyObject::create_float(static_cast<llf>(lhs) + other->as_float(), float_type_ref);
                throw TypeError("unsupported operand types(s) for +.");
            }, 2, function_type_ref
        ));
        this->define("__sub__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
                PyObject* other = args[1];
                if(other->is_int_type()) return PyObject::create_int(lhs - other->as_int(), int_type_ref);
                if(other->is_bool_type()) return PyObject::create_int(lhs - (other->as_bool() ? 1LL : 0LL), int_type_ref);
                if(other->is_float_type()) return PyObject::create_float(static_cast<llf>(lhs) - other->as_float(), float_type_ref);
                throw TypeError("unsupported operand types(s) for -.");
            }, 2, function_type_ref
        ));
        this->define("__mul__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
                PyObject* other = args[1];
                if(other->is_int_type()) return PyObject::create_int(lhs * other->as_int(), int_type_ref);
                if(other->is_bool_type()) return PyObject::create_int(lhs * (other->as_bool() ? 1LL : 0LL), int_type_ref);
                if(other->is_float_type()) return PyObject::create_float(static_cast<llf>(lhs) * other->as_float(), float_type_ref);
                if(other->is_str_type()) {
                    if(lhs <= 0) return PyObject::create_string("", str_type_ref);
                    std::string result;
                    result.reserve(static_cast<size_t>(lhs * static_cast<lld>(other->as_string().size())));
                    for(lld idx = 0; idx < lhs; ++idx) result += other->as_string();
                    return PyObject::create_string(result, str_type_ref);
                }
                throw TypeError("unsupported operand types(s) for *.");
            }, 2, function_type_ref
        ));
        this->define("__truediv__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                llf lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1.0L : 0.0L) : static_cast<llf>(args[0]->as_int());
                PyObject* other = args[1];
                llf rhs = 0.0L;
                if(other->is_int_type()) rhs = static_cast<llf>(other->as_int());
                else if(other->is_bool_type()) rhs = other->as_bool() ? 1.0L : 0.0L;
                else if(other->is_float_type()) rhs = other->as_float();
                else throw TypeError("unsupported operand types(s) for /.");
                if(rhs == 0.0L) throw ZeroDivisionError("division by zero");
                return PyObject::create_float(lhs / rhs, float_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__mod__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
                lld rhs = 0LL;
                if(args[1]->is_int_type()) rhs = args[1]->as_int();
                else if(args[1]->is_bool_type()) rhs = args[1]->as_bool() ? 1LL : 0LL;
                else throw TypeError("unsupported operand type(s) for %");
                if(rhs == 0LL) throw ZeroDivisionError("modulo by zero");
                return PyObject::create_int(lhs % rhs, int_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__and__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
                lld rhs = args[1]->is_bool_type() ? (args[1]->as_bool() ? 1LL : 0LL) : args[1]->as_int();
                return PyObject::create_int(lhs & rhs, int_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__or__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
                lld rhs = args[1]->is_bool_type() ? (args[1]->as_bool() ? 1LL : 0LL) : args[1]->as_int();
                return PyObject::create_int(lhs | rhs, int_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__xor__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
                lld rhs = args[1]->is_bool_type() ? (args[1]->as_bool() ? 1LL : 0LL) : args[1]->as_int();
                return PyObject::create_int(lhs ^ rhs, int_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__lshift__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
                lld rhs = args[1]->is_bool_type() ? (args[1]->as_bool() ? 1LL : 0LL) : args[1]->as_int();
                return PyObject::create_int(lhs << rhs, int_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__rshift__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
                lld rhs = args[1]->is_bool_type() ? (args[1]->as_bool() ? 1LL : 0LL) : args[1]->as_int();
                return PyObject::create_int(lhs >> rhs, int_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__eq__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
                if(args[1]->is_int_type()) return PyObject::create_bool(lhs == args[1]->as_int(), bool_type_ref);
                if(args[1]->is_bool_type()) return PyObject::create_bool(lhs == (args[1]->as_bool() ? 1LL : 0LL), bool_type_ref);
                if(args[1]->is_float_type()) return PyObject::create_bool(static_cast<llf>(lhs) == args[1]->as_float(), bool_type_ref);
                return PyObject::create_bool(false, bool_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__ne__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
                if(args[1]->is_int_type()) return PyObject::create_bool(lhs != args[1]->as_int(), bool_type_ref);
                if(args[1]->is_bool_type()) return PyObject::create_bool(lhs != (args[1]->as_bool() ? 1LL : 0LL), bool_type_ref);
                if(args[1]->is_float_type()) return PyObject::create_bool(static_cast<llf>(lhs) != args[1]->as_float(), bool_type_ref);
                return PyObject::create_bool(true, bool_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__lt__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                llf lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1.0L : 0.0L) : static_cast<llf>(args[0]->as_int());
                if(args[1]->is_int_type()) return PyObject::create_bool(lhs < static_cast<llf>(args[1]->as_int()), bool_type_ref);
                if(args[1]->is_bool_type()) return PyObject::create_bool(lhs < (args[1]->as_bool() ? 1.0L : 0.0L), bool_type_ref);
                if(args[1]->is_float_type()) return PyObject::create_bool(lhs < args[1]->as_float(), bool_type_ref);
                throw TypeError("unsupported comparison");
            }, 2, function_type_ref
        ));
        this->define("__le__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                llf lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1.0L : 0.0L) : static_cast<llf>(args[0]->as_int());
                if(args[1]->is_int_type()) return PyObject::create_bool(lhs <= static_cast<llf>(args[1]->as_int()), bool_type_ref);
                if(args[1]->is_bool_type()) return PyObject::create_bool(lhs <= (args[1]->as_bool() ? 1.0L : 0.0L), bool_type_ref);
                if(args[1]->is_float_type()) return PyObject::create_bool(lhs <= args[1]->as_float(), bool_type_ref);
                throw TypeError("unsupported comparison");
            }, 2, function_type_ref
        ));
        this->define("__gt__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                llf lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1.0L : 0.0L) : static_cast<llf>(args[0]->as_int());
                if(args[1]->is_int_type()) return PyObject::create_bool(lhs > static_cast<llf>(args[1]->as_int()), bool_type_ref);
                if(args[1]->is_bool_type()) return PyObject::create_bool(lhs > (args[1]->as_bool() ? 1.0L : 0.0L), bool_type_ref);
                if(args[1]->is_float_type()) return PyObject::create_bool(lhs > args[1]->as_float(), bool_type_ref);
                throw TypeError("unsupported comparison");
            }, 2, function_type_ref
        ));
        this->define("__ge__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                llf lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1.0L : 0.0L) : static_cast<llf>(args[0]->as_int());
                if(args[1]->is_int_type()) return PyObject::create_bool(lhs >= static_cast<llf>(args[1]->as_int()), bool_type_ref);
                if(args[1]->is_bool_type()) return PyObject::create_bool(lhs >= (args[1]->as_bool() ? 1.0L : 0.0L), bool_type_ref);
                if(args[1]->is_float_type()) return PyObject::create_bool(lhs >= args[1]->as_float(), bool_type_ref);
                throw TypeError("unsupported comparison");
            }, 2, function_type_ref
        ));
        return;
    }

    if(this->type_name == "float") {
        this->define("__new__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                PyObject* source = args[1];
                if(source->is_float_type()) return PyObject::create_float(source->as_float(), float_type_ref);
                if(source->is_int_type()) return PyObject::create_float(static_cast<llf>(source->as_int()), float_type_ref);
                if(source->is_bool_type()) return PyObject::create_float(source->as_bool() ? 1.0L : 0.0L, float_type_ref);
                throw TypeError("unsupported conversion to float");
            }, 2, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_none(none_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__str__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(float_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__float__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                if(args[0]->is_float_type()) {
                    return args[0];
                }
                return PyObject::create_float(args[0]->as_float(), float_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(float_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__add__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                llf lhs = args[0]->as_float();
                if(args[1]->is_float_type()) return PyObject::create_float(lhs + args[1]->as_float(), float_type_ref);
                if(args[1]->is_int_type()) return PyObject::create_float(lhs + static_cast<llf>(args[1]->as_int()), float_type_ref);
                if(args[1]->is_bool_type()) return PyObject::create_float(lhs + (args[1]->as_bool() ? 1.0L : 0.0L), float_type_ref);
                throw TypeError("unsupported operand types(s) for +.");
            }, 2, function_type_ref
        ));
        this->define("__sub__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                llf lhs = args[0]->as_float();
                if(args[1]->is_float_type()) return PyObject::create_float(lhs - args[1]->as_float(), float_type_ref);
                if(args[1]->is_int_type()) return PyObject::create_float(lhs - static_cast<llf>(args[1]->as_int()), float_type_ref);
                if(args[1]->is_bool_type()) return PyObject::create_float(lhs - (args[1]->as_bool() ? 1.0L : 0.0L), float_type_ref);
                throw TypeError("unsupported operand types(s) for -.");
            }, 2, function_type_ref
        ));
        this->define("__mul__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                llf lhs = args[0]->as_float();
                if(args[1]->is_float_type()) return PyObject::create_float(lhs * args[1]->as_float(), float_type_ref);
                if(args[1]->is_int_type()) return PyObject::create_float(lhs * static_cast<llf>(args[1]->as_int()), float_type_ref);
                if(args[1]->is_bool_type()) return PyObject::create_float(lhs * (args[1]->as_bool() ? 1.0L : 0.0L), float_type_ref);
                throw TypeError("unsupported operand types(s) for *.");
            }, 2, function_type_ref
        ));
        this->define("__truediv__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                llf lhs = args[0]->as_float();
                llf rhs = 0.0L;
                if(args[1]->is_float_type()) rhs = args[1]->as_float();
                else if(args[1]->is_int_type()) rhs = static_cast<llf>(args[1]->as_int());
                else if(args[1]->is_bool_type()) rhs = args[1]->as_bool() ? 1.0L : 0.0L;
                else throw TypeError("unsupported operand types(s) for /.");
                if(rhs == 0.0L) throw ZeroDivisionError("division by zero");
                return PyObject::create_float(lhs / rhs, float_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__eq__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                llf lhs = args[0]->as_float();
                if(args[1]->is_float_type()) return PyObject::create_bool(lhs == args[1]->as_float(), bool_type_ref);
                if(args[1]->is_int_type()) return PyObject::create_bool(lhs == static_cast<llf>(args[1]->as_int()), bool_type_ref);
                if(args[1]->is_bool_type()) return PyObject::create_bool(lhs == (args[1]->as_bool() ? 1.0L : 0.0L), bool_type_ref);
                return PyObject::create_bool(false, bool_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__ne__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                llf lhs = args[0]->as_float();
                if(args[1]->is_float_type()) return PyObject::create_bool(lhs != args[1]->as_float(), bool_type_ref);
                if(args[1]->is_int_type()) return PyObject::create_bool(lhs != static_cast<llf>(args[1]->as_int()), bool_type_ref);
                if(args[1]->is_bool_type()) return PyObject::create_bool(lhs != (args[1]->as_bool() ? 1.0L : 0.0L), bool_type_ref);
                return PyObject::create_bool(true, bool_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__lt__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                llf lhs = args[0]->as_float();
                if(args[1]->is_float_type()) return PyObject::create_bool(lhs < args[1]->as_float(), bool_type_ref);
                if(args[1]->is_int_type()) return PyObject::create_bool(lhs < static_cast<llf>(args[1]->as_int()), bool_type_ref);
                if(args[1]->is_bool_type()) return PyObject::create_bool(lhs < (args[1]->as_bool() ? 1.0L : 0.0L), bool_type_ref);
                throw TypeError("unsupported comparison");
            }, 2, function_type_ref
        ));
        this->define("__le__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                llf lhs = args[0]->as_float();
                if(args[1]->is_float_type()) return PyObject::create_bool(lhs <= args[1]->as_float(), bool_type_ref);
                if(args[1]->is_int_type()) return PyObject::create_bool(lhs <= static_cast<llf>(args[1]->as_int()), bool_type_ref);
                if(args[1]->is_bool_type()) return PyObject::create_bool(lhs <= (args[1]->as_bool() ? 1.0L : 0.0L), bool_type_ref);
                throw TypeError("unsupported comparison");
            }, 2, function_type_ref
        ));
        this->define("__gt__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                llf lhs = args[0]->as_float();
                if(args[1]->is_float_type()) return PyObject::create_bool(lhs > args[1]->as_float(), bool_type_ref);
                if(args[1]->is_int_type()) return PyObject::create_bool(lhs > static_cast<llf>(args[1]->as_int()), bool_type_ref);
                if(args[1]->is_bool_type()) return PyObject::create_bool(lhs > (args[1]->as_bool() ? 1.0L : 0.0L), bool_type_ref);
                throw TypeError("unsupported comparison");
            }, 2, function_type_ref
        ));
        this->define("__ge__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                llf lhs = args[0]->as_float();
                if(args[1]->is_float_type()) return PyObject::create_bool(lhs >= args[1]->as_float(), bool_type_ref);
                if(args[1]->is_int_type()) return PyObject::create_bool(lhs >= static_cast<llf>(args[1]->as_int()), bool_type_ref);
                if(args[1]->is_bool_type()) return PyObject::create_bool(lhs >= (args[1]->as_bool() ? 1.0L : 0.0L), bool_type_ref);
                throw TypeError("unsupported comparison");
            }, 2, function_type_ref
        ));
        this->define("__bool__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_bool(args[0]->as_float() != 0.0L, bool_type_ref);
            }, 1, function_type_ref
        ));
        return;
    }

    if(this->type_name == "bool") {
        this->define("__new__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                PyObject* source = args[1];
                if(source->is_none_type()) return PyObject::create_bool(false, bool_type_ref);
                if(source->is_bool_type()) return PyObject::create_bool(source->as_bool(), bool_type_ref);
                if(source->is_int_type()) return PyObject::create_bool(source->as_int() != 0LL, bool_type_ref);
                if(source->is_float_type()) return PyObject::create_bool(source->as_float() != 0.0L, bool_type_ref);
                if(source->is_str_type()) return PyObject::create_bool(!source->as_string().empty(), bool_type_ref);
                PyObject* bool_method = source->find("__bool__");
                if(bool_method != nullptr && bool_method->is_fn_type()) {
                    PyObject* bound = bool_method->bind_callable(source, method_wrapper_type_ref, "__bool__");
                    std::vector<PyObject*> no_args;
                    PyObject* evaluated = bound->call_callable(no_args, runtime);
                    if(evaluated != nullptr && evaluated->is_bool_type()) {
                        return PyObject::create_bool(evaluated->as_bool(), bool_type_ref);
                    }
                }
                PyObject* len_method = source->find("__len__");
                if(len_method != nullptr && len_method->is_fn_type()) {
                    PyObject* bound = len_method->bind_callable(source, method_wrapper_type_ref, "__len__");
                    std::vector<PyObject*> no_args;
                    PyObject* evaluated = bound->call_callable(no_args, runtime);
                    if(evaluated != nullptr && evaluated->is_int_type()) {
                        return PyObject::create_bool(evaluated->as_int() != 0LL, bool_type_ref);
                    }
                }
                return PyObject::create_bool(true, bool_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_none(none_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__bool__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                if(args[0]->is_bool_type()) {
                    return args[0];
                }
                return PyObject::create_bool(args[0]->as_bool(), bool_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__str__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(bool_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(bool_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
        return;
    }

    if(this->type_name == "str") {
        this->define("__new__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                PyObject* source = args[1];
                if(source->is_str_type()) return PyObject::create_string(source->as_string(), str_type_ref);
                if(source->is_none_type()) return PyObject::create_string("None", str_type_ref);
                if(source->is_bool_type()) return PyObject::create_string(source->as_bool() ? "True" : "False", str_type_ref);
                if(source->is_int_type()) return PyObject::create_string(std::to_string(source->as_int()), str_type_ref);
                if(source->is_float_type()) {
                    llf number = source->as_float();
                    std::ostringstream oss;
                    lld intPart = static_cast<lld>(number);
                    llf fracPart = number - intPart;
                    if(fracPart == 0.0L) oss << intPart << ".0";
                    else oss << number;
                    return PyObject::create_string(oss.str(), str_type_ref);
                }
                if(source->is_type_type()) {
                    return PyObject::create_string("<class '" + source->unwrap_type_obj()->get_name() + "'>", str_type_ref);
                }
                PyObject* method = source->find("__str__");
                if(method != nullptr && method->is_fn_type()) {
                    PyObject* bound = method->bind_callable(source, method_wrapper_type_ref, "__str__");
                    std::vector<PyObject*> no_args;
                    PyObject* rendered = bound->call_callable(no_args, runtime);
                    if(rendered != nullptr && rendered->is_str_type()) {
                        return PyObject::create_string(rendered->as_string(), str_type_ref);
                    }
                }
                throw TypeError("Method not found or is not callable");
            }, 2, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_none(none_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__str__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                if(args[0]->is_str_type()) {
                    return args[0];
                }
                return PyObject::create_string(args[0]->as_string(), str_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(str_repr_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__bool__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_bool(!args[0]->as_string().empty(), bool_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__add__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                if(!args[1]->is_str_type()) {
                    throw TypeError("can only concatenate str to str");
                }
                return PyObject::create_string(args[0]->as_string() + args[1]->as_string(), str_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__eq__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                if(!args[1]->is_str_type()) return PyObject::create_bool(false, bool_type_ref);
                return PyObject::create_bool(args[0]->as_string() == args[1]->as_string(), bool_type_ref);
            }, 2, function_type_ref
        ));
        this->define("__ne__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                if(!args[1]->is_str_type()) return PyObject::create_bool(true, bool_type_ref);
                return PyObject::create_bool(args[0]->as_string() != args[1]->as_string(), bool_type_ref);
            }, 2, function_type_ref
        ));
        return;
    }

    if(this->type_name == "function") {
        this->define("__str__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(function_object_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(function_object_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
    }

    if(this->type_name == "method_wrapper") {
        this->define("__str__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(method_wrapper_object_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_string(method_wrapper_object_display(args[0]), str_type_ref);
            }, 1, function_type_ref
        ));
    }

    if(this->type_name == "function" || this->type_name == "builtin_function" || this->type_name == "method_wrapper") {
        this->define("__call__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return args[0];
            }, 1, function_type_ref
        ));
        this->define("__new__", PyObject::create_native_callable(
            [this](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                throw TypeError("cannot instantiate '" + this->type_name + "' directly");
            }, 1, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            [](std::vector<PyObject*>& args, Interpreter* runtime) -> PyObject* {
                return PyObject::create_none(none_type_ref);
            }, 1, function_type_ref
        ));
    }
}


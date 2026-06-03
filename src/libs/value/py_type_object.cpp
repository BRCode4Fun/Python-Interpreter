#include <stdexcept>
#include <sstream>
#include <cstdint>
#include "py_type_object.hpp"
#include "../ast/ast.hpp"
#include "../exceptions/python_exceptions.hpp"
#include "../interpreter/interpreter.hpp"
#include "../scope/scope.hpp"

namespace {

/*
 * Runtime display helpers shared by __str__ and __repr__ implementations.
 */
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
    return "None";
}

std::string int_display(PyObject* self) {
    return std::to_string(self->as_int());
}

std::string float_display(PyObject* self) {
    llf number = self->as_float();
    std::ostringstream oss;
    lld int_part = static_cast<lld>(number);
    llf frac_part = number - int_part;
    if(frac_part == 0.0L) {
        oss << int_part << ".0";
    } else {
        oss << number;
    }
    return oss.str();
}

std::string bool_display(PyObject* self) {
    return self->as_bool() ? "True" : "False";
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

/*
 * Native methods installed on the base object type.
 */
PyObject* object_new_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    PyTypeObject* cls = args[0]->unwrap_type_obj();
    return cls->instantiate();
}

PyObject* object_init_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_none();
}

PyObject* object_repr_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(object_instance_display(args[0]));
}

PyObject* object_display_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(object_instance_display(args[0]));
}

PyObject* object_eq_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_bool(args[0] == args[1]);
}

PyObject* object_ne_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_bool(args[0] != args[1]);
}

/*
 * Native methods installed on the metatype object.
 */
PyObject* type_new_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    PyTypeObject* type = args[1]->get_type();
    if(type == nullptr) {
        throw TypeError("object has no runtime type");
    }
    return type;
}

PyObject* type_init_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_none();
}

PyObject* type_repr_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(type_class_display(args[0]));
}

PyObject* type_display_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(type_class_display(args[0]));
}

/*
 * Native methods installed on NoneType.
 */
PyObject* none_new_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    throw TypeError("cannot instantiate 'NoneType' directly");
}

PyObject* none_init_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_none();
}

PyObject* none_display_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(none_display(args[0]));
}

PyObject* none_repr_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(none_display(args[0]));
}

PyObject* none_bool_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_bool(false);
}

/*
 * Native methods installed on int, including numeric operations and comparisons.
 */
PyObject* int_new_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    PyObject* source = args[1];
    if(source->is_int_type()) return PyObject::create_int(source->as_int());
    if(source->is_bool_type()) return PyObject::create_int(source->as_bool() ? 1LL : 0LL);
    if(source->is_float_type()) return PyObject::create_int(static_cast<lld>(source->as_float()));
    throw TypeError("unsupported conversion to int");
}

PyObject* int_init_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_none();
}

PyObject* int_to_int_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    if(args[0]->is_int_type()) {
        return args[0];
    }
    return PyObject::create_int(args[0]->as_int());
}

PyObject* int_bool_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_bool(args[0]->as_int() != 0LL);
}

PyObject* int_display_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(int_display(args[0]));
}

PyObject* int_repr_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(int_display(args[0]));
}

PyObject* int_add_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
    PyObject* other = args[1];
    if(other->is_int_type()) return PyObject::create_int(lhs + other->as_int());
    if(other->is_bool_type()) return PyObject::create_int(lhs + (other->as_bool() ? 1LL : 0LL));
    if(other->is_float_type()) return PyObject::create_float(static_cast<llf>(lhs) + other->as_float());
    throw TypeError("unsupported operand types(s) for +.");
}

PyObject* int_sub_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
    PyObject* other = args[1];
    if(other->is_int_type()) return PyObject::create_int(lhs - other->as_int());
    if(other->is_bool_type()) return PyObject::create_int(lhs - (other->as_bool() ? 1LL : 0LL));
    if(other->is_float_type()) return PyObject::create_float(static_cast<llf>(lhs) - other->as_float());
    throw TypeError("unsupported operand types(s) for -.");
}

PyObject* int_mul_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
    PyObject* other = args[1];
    if(other->is_int_type()) return PyObject::create_int(lhs * other->as_int());
    if(other->is_bool_type()) return PyObject::create_int(lhs * (other->as_bool() ? 1LL : 0LL));
    if(other->is_float_type()) return PyObject::create_float(static_cast<llf>(lhs) * other->as_float());
    if(other->is_str_type()) {
        if(lhs <= 0) return PyObject::create_string("");
        std::string result;
        result.reserve(static_cast<size_t>(lhs * static_cast<lld>(other->as_string().size())));
        for(lld idx = 0; idx < lhs; ++idx) result += other->as_string();
        return PyObject::create_string(result);
    }
    throw TypeError("unsupported operand types(s) for *.");
}

PyObject* int_truediv_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    llf lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1.0L : 0.0L) : static_cast<llf>(args[0]->as_int());
    PyObject* other = args[1];
    llf rhs = 0.0L;
    if(other->is_int_type()) rhs = static_cast<llf>(other->as_int());
    else if(other->is_bool_type()) rhs = other->as_bool() ? 1.0L : 0.0L;
    else if(other->is_float_type()) rhs = other->as_float();
    else throw TypeError("unsupported operand types(s) for /.");
    if(rhs == 0.0L) throw ZeroDivisionError("division by zero");
    return PyObject::create_float(lhs / rhs);
}

PyObject* int_mod_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
    lld rhs = 0LL;
    if(args[1]->is_int_type()) rhs = args[1]->as_int();
    else if(args[1]->is_bool_type()) rhs = args[1]->as_bool() ? 1LL : 0LL;
    else throw TypeError("unsupported operand type(s) for %");
    if(rhs == 0LL) throw ZeroDivisionError("modulo by zero");
    return PyObject::create_int(lhs % rhs);
}

PyObject* int_and_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
    lld rhs = args[1]->is_bool_type() ? (args[1]->as_bool() ? 1LL : 0LL) : args[1]->as_int();
    return PyObject::create_int(lhs & rhs);
}

PyObject* int_or_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
    lld rhs = args[1]->is_bool_type() ? (args[1]->as_bool() ? 1LL : 0LL) : args[1]->as_int();
    return PyObject::create_int(lhs | rhs);
}

PyObject* int_xor_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
    lld rhs = args[1]->is_bool_type() ? (args[1]->as_bool() ? 1LL : 0LL) : args[1]->as_int();
    return PyObject::create_int(lhs ^ rhs);
}

PyObject* int_lshift_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
    lld rhs = args[1]->is_bool_type() ? (args[1]->as_bool() ? 1LL : 0LL) : args[1]->as_int();
    return PyObject::create_int(lhs << rhs);
}

PyObject* int_rshift_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
    lld rhs = args[1]->is_bool_type() ? (args[1]->as_bool() ? 1LL : 0LL) : args[1]->as_int();
    return PyObject::create_int(lhs >> rhs);
}

PyObject* int_eq_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
    if(args[1]->is_int_type()) return PyObject::create_bool(lhs == args[1]->as_int());
    if(args[1]->is_bool_type()) return PyObject::create_bool(lhs == (args[1]->as_bool() ? 1LL : 0LL));
    if(args[1]->is_float_type()) return PyObject::create_bool(static_cast<llf>(lhs) == args[1]->as_float());
    return PyObject::create_bool(false);
}

PyObject* int_ne_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    lld lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1LL : 0LL) : args[0]->as_int();
    if(args[1]->is_int_type()) return PyObject::create_bool(lhs != args[1]->as_int());
    if(args[1]->is_bool_type()) return PyObject::create_bool(lhs != (args[1]->as_bool() ? 1LL : 0LL));
    if(args[1]->is_float_type()) return PyObject::create_bool(static_cast<llf>(lhs) != args[1]->as_float());
    return PyObject::create_bool(true);
}

PyObject* int_lt_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    llf lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1.0L : 0.0L) : static_cast<llf>(args[0]->as_int());
    if(args[1]->is_int_type()) return PyObject::create_bool(lhs < static_cast<llf>(args[1]->as_int()));
    if(args[1]->is_bool_type()) return PyObject::create_bool(lhs < (args[1]->as_bool() ? 1.0L : 0.0L));
    if(args[1]->is_float_type()) return PyObject::create_bool(lhs < args[1]->as_float());
    throw TypeError("unsupported comparison");
}

PyObject* int_le_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    llf lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1.0L : 0.0L) : static_cast<llf>(args[0]->as_int());
    if(args[1]->is_int_type()) return PyObject::create_bool(lhs <= static_cast<llf>(args[1]->as_int()));
    if(args[1]->is_bool_type()) return PyObject::create_bool(lhs <= (args[1]->as_bool() ? 1.0L : 0.0L));
    if(args[1]->is_float_type()) return PyObject::create_bool(lhs <= args[1]->as_float());
    throw TypeError("unsupported comparison");
}

PyObject* int_gt_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    llf lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1.0L : 0.0L) : static_cast<llf>(args[0]->as_int());
    if(args[1]->is_int_type()) return PyObject::create_bool(lhs > static_cast<llf>(args[1]->as_int()));
    if(args[1]->is_bool_type()) return PyObject::create_bool(lhs > (args[1]->as_bool() ? 1.0L : 0.0L));
    if(args[1]->is_float_type()) return PyObject::create_bool(lhs > args[1]->as_float());
    throw TypeError("unsupported comparison");
}

PyObject* int_ge_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    llf lhs = args[0]->is_bool_type() ? (args[0]->as_bool() ? 1.0L : 0.0L) : static_cast<llf>(args[0]->as_int());
    if(args[1]->is_int_type()) return PyObject::create_bool(lhs >= static_cast<llf>(args[1]->as_int()));
    if(args[1]->is_bool_type()) return PyObject::create_bool(lhs >= (args[1]->as_bool() ? 1.0L : 0.0L));
    if(args[1]->is_float_type()) return PyObject::create_bool(lhs >= args[1]->as_float());
    throw TypeError("unsupported comparison");
}

/*
 * Native methods installed on float, including mixed numeric operations.
 */
PyObject* float_new_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    PyObject* source = args[1];
    if(source->is_float_type()) return PyObject::create_float(source->as_float());
    if(source->is_int_type()) return PyObject::create_float(static_cast<llf>(source->as_int()));
    if(source->is_bool_type()) return PyObject::create_float(source->as_bool() ? 1.0L : 0.0L);
    throw TypeError("unsupported conversion to float");
}

PyObject* float_init_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_none();
}

PyObject* float_display_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(float_display(args[0]));
}

PyObject* float_to_float_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    if(args[0]->is_float_type()) {
        return args[0];
    }
    return PyObject::create_float(args[0]->as_float());
}

PyObject* float_repr_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(float_display(args[0]));
}

PyObject* float_add_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    llf lhs = args[0]->as_float();
    if(args[1]->is_float_type()) return PyObject::create_float(lhs + args[1]->as_float());
    if(args[1]->is_int_type()) return PyObject::create_float(lhs + static_cast<llf>(args[1]->as_int()));
    if(args[1]->is_bool_type()) return PyObject::create_float(lhs + (args[1]->as_bool() ? 1.0L : 0.0L));
    throw TypeError("unsupported operand types(s) for +.");
}

PyObject* float_sub_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    llf lhs = args[0]->as_float();
    if(args[1]->is_float_type()) return PyObject::create_float(lhs - args[1]->as_float());
    if(args[1]->is_int_type()) return PyObject::create_float(lhs - static_cast<llf>(args[1]->as_int()));
    if(args[1]->is_bool_type()) return PyObject::create_float(lhs - (args[1]->as_bool() ? 1.0L : 0.0L));
    throw TypeError("unsupported operand types(s) for -.");
}

PyObject* float_mul_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    llf lhs = args[0]->as_float();
    if(args[1]->is_float_type()) return PyObject::create_float(lhs * args[1]->as_float());
    if(args[1]->is_int_type()) return PyObject::create_float(lhs * static_cast<llf>(args[1]->as_int()));
    if(args[1]->is_bool_type()) return PyObject::create_float(lhs * (args[1]->as_bool() ? 1.0L : 0.0L));
    throw TypeError("unsupported operand types(s) for *.");
}

PyObject* float_truediv_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    llf lhs = args[0]->as_float();
    llf rhs = 0.0L;
    if(args[1]->is_float_type()) rhs = args[1]->as_float();
    else if(args[1]->is_int_type()) rhs = static_cast<llf>(args[1]->as_int());
    else if(args[1]->is_bool_type()) rhs = args[1]->as_bool() ? 1.0L : 0.0L;
    else throw TypeError("unsupported operand types(s) for /.");
    if(rhs == 0.0L) throw ZeroDivisionError("division by zero");
    return PyObject::create_float(lhs / rhs);
}

PyObject* float_eq_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    llf lhs = args[0]->as_float();
    if(args[1]->is_float_type()) return PyObject::create_bool(lhs == args[1]->as_float());
    if(args[1]->is_int_type()) return PyObject::create_bool(lhs == static_cast<llf>(args[1]->as_int()));
    if(args[1]->is_bool_type()) return PyObject::create_bool(lhs == (args[1]->as_bool() ? 1.0L : 0.0L));
    return PyObject::create_bool(false);
}

PyObject* float_ne_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    llf lhs = args[0]->as_float();
    if(args[1]->is_float_type()) return PyObject::create_bool(lhs != args[1]->as_float());
    if(args[1]->is_int_type()) return PyObject::create_bool(lhs != static_cast<llf>(args[1]->as_int()));
    if(args[1]->is_bool_type()) return PyObject::create_bool(lhs != (args[1]->as_bool() ? 1.0L : 0.0L));
    return PyObject::create_bool(true);
}

PyObject* float_lt_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    llf lhs = args[0]->as_float();
    if(args[1]->is_float_type()) return PyObject::create_bool(lhs < args[1]->as_float());
    if(args[1]->is_int_type()) return PyObject::create_bool(lhs < static_cast<llf>(args[1]->as_int()));
    if(args[1]->is_bool_type()) return PyObject::create_bool(lhs < (args[1]->as_bool() ? 1.0L : 0.0L));
    throw TypeError("unsupported comparison");
}

PyObject* float_le_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    llf lhs = args[0]->as_float();
    if(args[1]->is_float_type()) return PyObject::create_bool(lhs <= args[1]->as_float());
    if(args[1]->is_int_type()) return PyObject::create_bool(lhs <= static_cast<llf>(args[1]->as_int()));
    if(args[1]->is_bool_type()) return PyObject::create_bool(lhs <= (args[1]->as_bool() ? 1.0L : 0.0L));
    throw TypeError("unsupported comparison");
}

PyObject* float_gt_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    llf lhs = args[0]->as_float();
    if(args[1]->is_float_type()) return PyObject::create_bool(lhs > args[1]->as_float());
    if(args[1]->is_int_type()) return PyObject::create_bool(lhs > static_cast<llf>(args[1]->as_int()));
    if(args[1]->is_bool_type()) return PyObject::create_bool(lhs > (args[1]->as_bool() ? 1.0L : 0.0L));
    throw TypeError("unsupported comparison");
}

PyObject* float_ge_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    llf lhs = args[0]->as_float();
    if(args[1]->is_float_type()) return PyObject::create_bool(lhs >= args[1]->as_float());
    if(args[1]->is_int_type()) return PyObject::create_bool(lhs >= static_cast<llf>(args[1]->as_int()));
    if(args[1]->is_bool_type()) return PyObject::create_bool(lhs >= (args[1]->as_bool() ? 1.0L : 0.0L));
    throw TypeError("unsupported comparison");
}

PyObject* float_bool_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_bool(args[0]->as_float() != 0.0L);
}

/*
 * Native methods installed on bool, preserving bool as an int subtype.
 */
PyObject* bool_new_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    PyObject* source = args[1];
    if(source->is_none_type()) return PyObject::create_bool(false);
    if(source->is_bool_type()) return PyObject::create_bool(source->as_bool());
    if(source->is_int_type()) return PyObject::create_bool(source->as_int() != 0LL);
    if(source->is_float_type()) return PyObject::create_bool(source->as_float() != 0.0L);
    if(source->is_str_type()) return PyObject::create_bool(!source->as_string().empty());
    PyObject* bool_method = source->find("__bool__");
    if(bool_method != nullptr && bool_method->is_fn_type()) {
        PyObject* bound = bool_method->bind_callable(source, nullptr, "__bool__");
        std::vector<PyObject*> no_args;
        PyObject* evaluated = bound->call_callable(no_args, runtime);
        runtime->track_temp(bound);
        if(evaluated != nullptr && evaluated->is_bool_type()) {
            return PyObject::create_bool(evaluated->as_bool());
        }
    }
    PyObject* len_method = source->find("__len__");
    if(len_method != nullptr && len_method->is_fn_type()) {
        PyObject* bound = len_method->bind_callable(source, nullptr, "__len__");
        std::vector<PyObject*> no_args;
        PyObject* evaluated = bound->call_callable(no_args, runtime);
        runtime->track_temp(bound);
        if(evaluated != nullptr && evaluated->is_int_type()) {
            return PyObject::create_bool(evaluated->as_int() != 0LL);
        }
    }
    return PyObject::create_bool(true);
}

PyObject* bool_init_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_none();
}

PyObject* bool_bool_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    if(args[0]->is_bool_type()) {
        return args[0];
    }
    return PyObject::create_bool(args[0]->as_bool());
}

PyObject* bool_display_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(bool_display(args[0]));
}

PyObject* bool_repr_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(bool_display(args[0]));
}

/*
 * Native methods installed on str, including conversion, truthiness and comparison.
 */
PyObject* str_new_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    PyObject* source = args[1];
    if(source->is_str_type()) return PyObject::create_string(source->as_string());
    if(source->is_none_type()) return PyObject::create_string("None");
    if(source->is_bool_type()) return PyObject::create_string(source->as_bool() ? "True" : "False");
    if(source->is_int_type()) return PyObject::create_string(std::to_string(source->as_int()));
    if(source->is_float_type()) {
        llf number = source->as_float();
        std::ostringstream oss;
        lld int_part = static_cast<lld>(number);
        llf frac_part = number - int_part;
        if(frac_part == 0.0L) oss << int_part << ".0";
        else oss << number;
        return PyObject::create_string(oss.str());
    }
    if(source->is_type_type()) {
        return PyObject::create_string("<class '" + source->unwrap_type_obj()->get_name() + "'>");
    }
    PyObject* method = source->find("__str__");
    if(method != nullptr && method->is_fn_type()) {
        PyObject* bound = method->bind_callable(source, nullptr, "__str__");
        std::vector<PyObject*> no_args;
        PyObject* rendered = bound->call_callable(no_args, runtime);
        runtime->track_temp(bound);
        if(rendered != nullptr && rendered->is_str_type()) {
            return PyObject::create_string(rendered->as_string());
        }
    }
    throw TypeError("Method not found or is not callable");
}

PyObject* str_init_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_none();
}

PyObject* str_display_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    if(args[0]->is_str_type()) {
        return args[0];
    }
    return PyObject::create_string(args[0]->as_string());
}

PyObject* str_repr_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(str_repr_display(args[0]));
}

PyObject* str_bool_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_bool(!args[0]->as_string().empty());
}

PyObject* str_add_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    if(!args[1]->is_str_type()) {
        throw TypeError("can only concatenate str to str");
    }
    return PyObject::create_string(args[0]->as_string() + args[1]->as_string());
}

PyObject* str_eq_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    if(!args[1]->is_str_type()) return PyObject::create_bool(false);
    return PyObject::create_bool(args[0]->as_string() == args[1]->as_string());
}

PyObject* str_ne_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    if(!args[1]->is_str_type()) return PyObject::create_bool(true);
    return PyObject::create_bool(args[0]->as_string() != args[1]->as_string());
}

/*
 * Native methods installed on user-defined function objects.
 */
PyObject* function_display_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(function_object_display(args[0]));
}

PyObject* function_repr_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(function_object_display(args[0]));
}

/*
 * Native methods installed on bound method wrapper objects.
 */
PyObject* method_wrapper_display_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(method_wrapper_object_display(args[0]));
}

PyObject* method_wrapper_repr_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_string(method_wrapper_object_display(args[0]));
}

/*
 * Native methods shared by all callable runtime types.
 */
PyObject* callable_new_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    PyTypeObject* callable_type = args[0]->unwrap_type_obj();
    throw TypeError("cannot instantiate '" + callable_type->get_name() + "' directly");
}

PyObject* callable_call_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return args[0];
}

PyObject* callable_init_object(std::vector<PyObject*>& args, Interpreter* runtime) {
    return PyObject::create_none();
}

} // namespace

/* builtins: context nullptr → internal Scope. Class: class_scope heap-adopted (final bool true on base PyObject). */
PyTypeObject::PyTypeObject(
    const std::string& name,
    PyTypeObject* metatype,
    const std::vector<PyTypeObject*>& bases,
    std::unique_ptr<Scope> context
) : PyObject(metatype, nullptr, std::move(context)), type_name(name), bases(bases) {}

PyObject* PyTypeObject::call(std::vector<PyObject*>& args, Interpreter* interpreter) {
    this->ensure_methods_registered();

    PyObject* new_method = this->find("__new__");
    if(new_method != nullptr && new_method->is_fn_type() && interpreter != nullptr) {
        PyObject* bound_new = new_method->bind_callable(this, method_wrapper_type_ref, "__new__");
        std::vector<PyObject*> constructor_args = args;
        PyObject* instance = bound_new->call_callable(constructor_args, interpreter);
        interpreter->track_temp(bound_new);
        if(instance == nullptr) {
            throw RuntimeError("Type '" + this->type_name + "' returned null from __new__");
        }

        PyObject* init_method = this->find("__init__");
        if(init_method != nullptr && init_method->is_fn_type()) {
            PyObject* bound_init = init_method->bind_callable(instance, method_wrapper_type_ref, "__init__");
            std::vector<PyObject*> init_args = args;
            bound_init->call_callable(init_args, interpreter);
            interpreter->track_temp(bound_init);
        }
        return instance;
    }

    throw TypeError("Type '" + this->type_name + "' is not callable");
}

void PyTypeObject::ensure_methods_registered() {
    if(this->scope->empty()) {
        this->register_methods();
    }
}

PyObject* PyTypeObject::instantiate() const {
    /* New instance Scope tied to type context; PyObject adopts ownership (avoids leak in native __new__). */
    PyObject* instance = new PyObject(
        const_cast<PyTypeObject*>(this),
        nullptr,
        std::make_unique<Scope>(const_cast<Scope*>(this->get_context()))
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
        this->register_methods();
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

void PyTypeObject::register_methods() {
    /*
     * Register methods for the base object type.
     */
    if(this->type_name == "object") {
        this->define("__new__", PyObject::create_native_callable(
            object_new_object, 1, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            object_init_object, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            object_repr_object, 1, function_type_ref
        ));
        this->define("__str__", PyObject::create_native_callable(
            object_display_object, 1, function_type_ref
        ));
        this->define("__eq__", PyObject::create_native_callable(
            object_eq_object, 2, function_type_ref
        ));
        this->define("__ne__", PyObject::create_native_callable(
            object_ne_object, 2, function_type_ref
        ));
        return;
    }

    /*
     * Register methods for the metatype object.
     */
    if(this->type_name == "type") {
        this->define("__new__", PyObject::create_native_callable(
            type_new_object, 2, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            type_init_object, 2, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            type_repr_object, 1, function_type_ref
        ));
        this->define("__str__", PyObject::create_native_callable(
            type_display_object, 1, function_type_ref
        ));
        return;
    }

    /*
     * Register methods for NoneType.
     */
    if(this->type_name == "NoneType") {
        this->define("__new__", PyObject::create_native_callable(
            none_new_object, 1, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            none_init_object, 1, function_type_ref
        ));
        this->define("__str__", PyObject::create_native_callable(
            none_display_object, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            none_repr_object, 1, function_type_ref
        ));
        this->define("__bool__", PyObject::create_native_callable(
            none_bool_object, 1, function_type_ref
        ));
        return;
    }

    /*
     * Register methods for int.
     */
    if(this->type_name == "int") {
        this->define("__new__", PyObject::create_native_callable(
            int_new_object, 2, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            int_init_object, 2, function_type_ref
        ));
        this->define("__int__", PyObject::create_native_callable(
            int_to_int_object, 1, function_type_ref
        ));
        this->define("__bool__", PyObject::create_native_callable(
            int_bool_object, 1, function_type_ref
        ));
        this->define("__str__", PyObject::create_native_callable(
            int_display_object, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            int_repr_object, 1, function_type_ref
        ));
        this->define("__add__", PyObject::create_native_callable(
            int_add_object, 2, function_type_ref
        ));
        this->define("__sub__", PyObject::create_native_callable(
            int_sub_object, 2, function_type_ref
        ));
        this->define("__mul__", PyObject::create_native_callable(
            int_mul_object, 2, function_type_ref
        ));
        this->define("__truediv__", PyObject::create_native_callable(
            int_truediv_object, 2, function_type_ref
        ));
        this->define("__mod__", PyObject::create_native_callable(
            int_mod_object, 2, function_type_ref
        ));
        this->define("__and__", PyObject::create_native_callable(
            int_and_object, 2, function_type_ref
        ));
        this->define("__or__", PyObject::create_native_callable(
            int_or_object, 2, function_type_ref
        ));
        this->define("__xor__", PyObject::create_native_callable(
            int_xor_object, 2, function_type_ref
        ));
        this->define("__lshift__", PyObject::create_native_callable(
            int_lshift_object, 2, function_type_ref
        ));
        this->define("__rshift__", PyObject::create_native_callable(
            int_rshift_object, 2, function_type_ref
        ));
        this->define("__eq__", PyObject::create_native_callable(
            int_eq_object, 2, function_type_ref
        ));
        this->define("__ne__", PyObject::create_native_callable(
            int_ne_object, 2, function_type_ref
        ));
        this->define("__lt__", PyObject::create_native_callable(
            int_lt_object, 2, function_type_ref
        ));
        this->define("__le__", PyObject::create_native_callable(
            int_le_object, 2, function_type_ref
        ));
        this->define("__gt__", PyObject::create_native_callable(
            int_gt_object, 2, function_type_ref
        ));
        this->define("__ge__", PyObject::create_native_callable(
            int_ge_object, 2, function_type_ref
        ));
        return;
    }

    /*
     * Register methods for float.
     */
    if(this->type_name == "float") {
        this->define("__new__", PyObject::create_native_callable(
            float_new_object, 2, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            float_init_object, 2, function_type_ref
        ));
        this->define("__str__", PyObject::create_native_callable(
            float_display_object, 1, function_type_ref
        ));
        this->define("__float__", PyObject::create_native_callable(
            float_to_float_object, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            float_repr_object, 1, function_type_ref
        ));
        this->define("__add__", PyObject::create_native_callable(
            float_add_object, 2, function_type_ref
        ));
        this->define("__sub__", PyObject::create_native_callable(
            float_sub_object, 2, function_type_ref
        ));
        this->define("__mul__", PyObject::create_native_callable(
            float_mul_object, 2, function_type_ref
        ));
        this->define("__truediv__", PyObject::create_native_callable(
            float_truediv_object, 2, function_type_ref
        ));
        this->define("__eq__", PyObject::create_native_callable(
            float_eq_object, 2, function_type_ref
        ));
        this->define("__ne__", PyObject::create_native_callable(
            float_ne_object, 2, function_type_ref
        ));
        this->define("__lt__", PyObject::create_native_callable(
            float_lt_object, 2, function_type_ref
        ));
        this->define("__le__", PyObject::create_native_callable(
            float_le_object, 2, function_type_ref
        ));
        this->define("__gt__", PyObject::create_native_callable(
            float_gt_object, 2, function_type_ref
        ));
        this->define("__ge__", PyObject::create_native_callable(
            float_ge_object, 2, function_type_ref
        ));
        this->define("__bool__", PyObject::create_native_callable(
            float_bool_object, 1, function_type_ref
        ));
        return;
    }

    /*
     * Register methods for bool.
     */
    if(this->type_name == "bool") {
        this->define("__new__", PyObject::create_native_callable(
            bool_new_object, 2, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            bool_init_object, 2, function_type_ref
        ));
        this->define("__bool__", PyObject::create_native_callable(
            bool_bool_object, 1, function_type_ref
        ));
        this->define("__str__", PyObject::create_native_callable(
            bool_display_object, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            bool_repr_object, 1, function_type_ref
        ));
        return;
    }

    /*
     * Register methods for str.
     */
    if(this->type_name == "str") {
        this->define("__new__", PyObject::create_native_callable(
            str_new_object, 2, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            str_init_object, 2, function_type_ref
        ));
        this->define("__str__", PyObject::create_native_callable(
            str_display_object, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            str_repr_object, 1, function_type_ref
        ));
        this->define("__bool__", PyObject::create_native_callable(
            str_bool_object, 1, function_type_ref
        ));
        this->define("__add__", PyObject::create_native_callable(
            str_add_object, 2, function_type_ref
        ));
        this->define("__eq__", PyObject::create_native_callable(
            str_eq_object, 2, function_type_ref
        ));
        this->define("__ne__", PyObject::create_native_callable(
            str_ne_object, 2, function_type_ref
        ));
        return;
    }

    /*
     * Register methods for user-defined function objects.
     */
    if(this->type_name == "function") {
        this->define("__str__", PyObject::create_native_callable(
            function_display_object, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            function_repr_object, 1, function_type_ref
        ));
    }

    /*
     * Register methods for bound method wrapper objects.
     */
    if(this->type_name == "method_wrapper") {
        this->define("__str__", PyObject::create_native_callable(
            method_wrapper_display_object, 1, function_type_ref
        ));
        this->define("__repr__", PyObject::create_native_callable(
            method_wrapper_repr_object, 1, function_type_ref
        ));
    }

    /*
     * Register methods shared by every callable runtime type.
     */
    if(this->type_name == "function" || this->type_name == "builtin_function" || this->type_name == "method_wrapper") {
        this->define("__call__", PyObject::create_native_callable(
            callable_call_object, 1, function_type_ref
        ));
        this->define("__new__", PyObject::create_native_callable(
            callable_new_object, 1, function_type_ref
        ));
        this->define("__init__", PyObject::create_native_callable(
            callable_init_object, 1, function_type_ref
        ));
    }
}


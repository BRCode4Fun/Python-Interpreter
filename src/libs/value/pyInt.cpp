#include "./pyInt.hpp"
#include "./pyFunction.hpp"
#include "./pyStr.hpp"
#include "./pyBool.hpp"
#include "./pyFloat.hpp"

PyInt::PyInt(const std::string& v) 
    : PyObject(ObjectType::Int, new lld(std::stoll(v))) {}

PyInt::PyInt(lld v)
    : PyObject(ObjectType::Int, new lld(v)) {}

void PyInt::registerMethods() {

    this->define("__int__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 1);

            return args[0]; // self
        })
    );

    this->define("__str__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 1);

            PyInt* self = args[0]->unwrap_int_obj();

            lld n = self->getInt();

            return new PyStr(std::to_string(n));
        })
    );

    this->define("__bool__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 1);

            PyInt* self = args[0]->unwrap_int_obj();

            lld n = self->getInt();
            
            return new PyBool(n != 0L);
        })
    );

    this->define("__add__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyInt* self = args[0]->unwrap_int_obj();
            lld lhs_value = self->getInt();

            PyObject* other = args[1];

            if(other->is_int_type()){
                lld rhs_value = other->unwrap_int_obj()->getInt();
                return new PyInt(lhs_value + rhs_value);
            
            } else if(other->is_float_type()){
                llf rhs_value = other->unwrap_float_obj()->getFloat();
                llf lhs_float_value = static_cast<llf>(lhs_value);
                return new PyFloat(lhs_float_value + rhs_value);
            
            } else {
                throw std::logic_error("unsupported operand types(s) for +.");
            }
        })
    );

    this->define("__sub__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyInt* self = args[0]->unwrap_int_obj();
            lld lhs_value = self->getInt();

            PyObject* other = args[1];

            if(other->is_int_type()){
                lld rhs_value = other->unwrap_int_obj()->getInt();
                return new PyInt(lhs_value - rhs_value);
            
            } else if(other->is_float_type()){
                llf rhs_value = other->unwrap_float_obj()->getFloat();
                llf lhs_float_value = static_cast<llf>(lhs_value);
                return new PyFloat(lhs_float_value - rhs_value);
            
            } else {
                throw std::logic_error("unsupported operand types(s) for -.");
            }
        })
    );

    this->define("__mul__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyInt* self = args[0]->unwrap_int_obj();
            lld lhs_value = self->getInt();

            PyObject* other = args[1];

            if(other->is_int_type()){
                lld rhs_value = other->unwrap_int_obj()->getInt();
                return new PyInt(lhs_value * rhs_value);
            
            } else if(other->is_float_type()){
                llf rhs_value = other->unwrap_float_obj()->getFloat();
                llf lhs_float_value = static_cast<llf>(lhs_value);
                return new PyFloat(lhs_float_value * rhs_value);
            
            } else {
                throw std::logic_error("unsupported operand types(s) for *.");
            }
        })
    );

    this->define("__truediv__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyInt* self = args[0]->unwrap_int_obj();
            llf lhs_value = static_cast<llf>(self->getInt());

            PyObject* other = args[1];

            if(other->is_int_type()){
                llf rhs_value = static_cast<llf>(other->unwrap_int_obj()->getInt());
                if(rhs_value == 0.0) {
                    throw std::logic_error("division by zero");
                } else {
                    return new PyFloat(lhs_value / rhs_value);
                }
            
            } else if(other->is_float_type()){
                llf rhs_value = other->unwrap_float_obj()->getFloat();
                if(rhs_value == 0.0) {
                    throw std::logic_error("division by zero");
                } else {
                    return new PyFloat(lhs_value / rhs_value);
                }
            
            } else {
                throw std::logic_error("unsupported operand types(s) for /.");
            }
        })
    );

    this->define("__mod__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyInt* self = args[0]->unwrap_int_obj();
            lld lhs_value = self->getInt();

            PyObject* other = args[1];

            if(other->is_int_type()){
                lld rhs_value = other->unwrap_int_obj()->getInt();
                if(rhs_value == 0) {
                    throw std::logic_error("modulo by zero");
                } else {
                    return new PyInt(lhs_value % rhs_value);
                }
            
            } else {
                throw std::logic_error("unsupported operand types(s) for %.");
            }
        })
    );

    this->define("__and__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];

            if(!other->is_int_type()){
                throw std::logic_error("unsupported operand type(s) for &.");
            }
            PyInt* lhs = args[0]->unwrap_int_obj();
            PyInt* rhs = other->unwrap_int_obj();

            lld a = lhs->getInt();
            lld b = rhs->getInt();
            
            return new PyInt(a & b);
        })
    );

    this->define("__or__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];

            if(!other->is_int_type()){
                throw std::logic_error("unsupported operand type(s) for |.");
            }
            PyInt* lhs = args[0]->unwrap_int_obj();
            PyInt* rhs = other->unwrap_int_obj();

            lld a = lhs->getInt();
            lld b = rhs->getInt();
            
            return new PyInt(a | b);
        })
    );

    this->define("__xor__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];

            if(!other->is_int_type()){
                throw std::logic_error("unsupported operand type(s) for ^.");
            }
            PyInt* lhs = args[0]->unwrap_int_obj();
            PyInt* rhs = other->unwrap_int_obj();

            lld a = lhs->getInt();
            lld b = rhs->getInt();
            
            return new PyInt(a ^ b);
        })
    );

    this->define("__lshift__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];

            if(!other->is_int_type()){
                throw std::logic_error("unsupported operand type(s) for <<.");
            }
            PyInt* lhs = args[0]->unwrap_int_obj();
            PyInt* rhs = other->unwrap_int_obj();

            lld a = lhs->getInt();
            lld b = rhs->getInt();
            
            return new PyInt(a << b);
        })
    );

    this->define("__rshift__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];

            if(!other->is_int_type()){
                throw std::logic_error("unsupported operand type(s) for >>.");
            }
            PyInt* lhs = args[0]->unwrap_int_obj();
            PyInt* rhs = other->unwrap_int_obj();

            lld a = lhs->getInt();
            lld b = rhs->getInt();
            
            return new PyInt(a >> b);
        })
    );

    this->define("__lt__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyInt* self = args[0]->unwrap_int_obj();
            lld lhs_value = self->getInt();

            PyObject* other = args[1];

            if(other->is_int_type()){
                lld rhs_value = other->unwrap_int_obj()->getInt();
                return new PyBool(lhs_value < rhs_value);
            
            } else if(other->is_float_type()){
                llf rhs_value = other->unwrap_float_obj()->getFloat();
                llf lhs_float_value = static_cast<llf>(lhs_value);
                return new PyBool(lhs_float_value < rhs_value);
            
            } else {
                throw std::logic_error("unsupported operand types(s) for <.");
            }
        })
    );

    this->define("__gt__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyInt* self = args[0]->unwrap_int_obj();
            lld lhs_value = self->getInt();

            PyObject* other = args[1];

            if(other->is_int_type()){
                lld rhs_value = other->unwrap_int_obj()->getInt();
                return new PyBool(lhs_value > rhs_value);
            
            } else if(other->is_float_type()){
                llf rhs_value = other->unwrap_float_obj()->getFloat();
                llf lhs_float_value = static_cast<llf>(lhs_value);
                return new PyBool(lhs_float_value > rhs_value);
            
            } else {
                throw std::logic_error("unsupported operand types(s) for >.");
            }
        })
    );

    this->define("__le__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyInt* self = args[0]->unwrap_int_obj();
            lld lhs_value = self->getInt();

            PyObject* other = args[1];

            if(other->is_int_type()){
                lld rhs_value = other->unwrap_int_obj()->getInt();
                return new PyBool(lhs_value <= rhs_value);
            
            } else if(other->is_float_type()){
                llf rhs_value = other->unwrap_float_obj()->getFloat();
                llf lhs_float_value = static_cast<llf>(lhs_value);
                return new PyBool(lhs_float_value <= rhs_value);
            
            } else {
                throw std::logic_error("unsupported operand types(s) for <=.");
            }
        })
    );

    this->define("__ge__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyInt* self = args[0]->unwrap_int_obj();
            lld lhs_value = self->getInt();

            PyObject* other = args[1];

            if(other->is_int_type()){
                lld rhs_value = other->unwrap_int_obj()->getInt();
                return new PyBool(lhs_value >= rhs_value);
            
            } else if(other->is_float_type()){
                llf rhs_value = other->unwrap_float_obj()->getFloat();
                llf lhs_float_value = static_cast<llf>(lhs_value);
                return new PyBool(lhs_float_value >= rhs_value);
            
            } else {
                throw std::logic_error("unsupported operand types(s) for >=.");
            }
        })
    );

    this->define("__eq__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyInt* self = args[0]->unwrap_int_obj();
            lld lhs_value = self->getInt();

            PyObject* other = args[1];

            if(other->is_int_type()){
                lld rhs_value = other->unwrap_int_obj()->getInt();
                return new PyBool(lhs_value == rhs_value);
            
            } else if(other->is_float_type()){
                llf rhs_value = other->unwrap_float_obj()->getFloat();
                llf lhs_float_value = static_cast<llf>(lhs_value);
                return new PyBool(lhs_float_value == rhs_value);
            
            } else {
                return new PyBool(false);
            }
        })
    );

    this->define("__ne__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyInt* self = args[0]->unwrap_int_obj();
            lld lhs_value = self->getInt();

            PyObject* other = args[1];

            if(other->is_int_type()){
                lld rhs_value = other->unwrap_int_obj()->getInt();
                return new PyBool(lhs_value != rhs_value);
            
            } else if(other->is_float_type()){
                llf rhs_value = other->unwrap_float_obj()->getFloat();
                llf lhs_float_value = static_cast<llf>(lhs_value);
                return new PyBool(lhs_float_value != rhs_value);
            
            } else {
                return new PyBool(true);
            }
        })
    );

    this->define("__neg__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 1);

            PyInt* self = args[0]->unwrap_int_obj();

            lld self_value = self->getInt();
            
            return new PyInt(-self_value);
        })
    );

    this->define("__invert__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 1);

            PyInt* self = args[0]->unwrap_int_obj();

            lld self_value = self->getInt();
            
            return new PyInt(~self_value);
        })
    );
}

#include <sstream>
#include "./pyFloat.hpp"
#include "./pyFunction.hpp"
#include "./pyStr.hpp"
#include "./pyBool.hpp"

PyFloat::PyFloat(const std::string& v) 
    : PyObject(ObjectType::Float, new llf(std::stold(v))) {}

PyFloat::PyFloat(llf v)
    : PyObject(ObjectType::Float, new llf(v)) {}

void PyFloat::registerMethods() {

    this->define("__float__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 1);

            return args[0]; // self
        })
    );

    this->define("__int__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 1);

            PyFloat* self = args[0]->unwrap_float_obj();

            llf n = self->getFloat();

            return new PyInt(static_cast<lld>(n));
        })
    );

    this->define("__str__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 1);

            PyFloat* self = args[0]->unwrap_float_obj();

            llf number = self->getFloat();

            std::ostringstream oss;
            lld intPart = static_cast<lld>(number);
            llf fracPart = number - intPart;

            if(fracPart == 0.0){
                oss << intPart << ".0";
            } else {
                oss << number;
            }
            return new PyStr(oss.str());
        })
    );

    this->define("__bool__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 1);

            PyFloat* self = args[0]->unwrap_float_obj();

            llf n = self->getFloat();
            
            return new PyBool(n != 0.0);
        })
    );

    this->define("__add__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];
            llf rhs_float;

            if(other->is_int_type()){
                rhs_float = static_cast<llf>(other->unwrap_int_obj()->getInt());
            
            } else if(other->is_float_type()){
                rhs_float = other->unwrap_float_obj()->getFloat();
            
            } else {
                throw std::logic_error("unsupported operand types(s) for +.");
            }

            PyFloat* lhs = args[0]->unwrap_float_obj();
            llf lhs_float = lhs->getFloat();
            
            return new PyFloat(lhs_float + rhs_float);
        })
    );

    this->define("__sub__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];
            llf rhs_float;

            if(other->is_int_type()){
                rhs_float = static_cast<llf>(other->unwrap_int_obj()->getInt());
            
            } else if(other->is_float_type()){
                rhs_float = other->unwrap_float_obj()->getFloat();
            
            } else {
                throw std::logic_error("unsupported operand types(s) for -.");
            }

            PyFloat* lhs = args[0]->unwrap_float_obj();
            llf lhs_float = lhs->getFloat();
            
            return new PyFloat(lhs_float - rhs_float);
        })
    );

    this->define("__mul__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];
            llf rhs_float;

            if(other->is_int_type()){
                rhs_float = static_cast<llf>(other->unwrap_int_obj()->getInt());
            
            } else if(other->is_float_type()){
                rhs_float = other->unwrap_float_obj()->getFloat();
            
            } else {
                throw std::logic_error("unsupported operand types(s) for *.");
            }

            PyFloat* lhs = args[0]->unwrap_float_obj();
            llf lhs_float = lhs->getFloat();
            
            return new PyFloat(lhs_float * rhs_float);
        })
    );

    this->define("__truediv__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];
            llf rhs_float;

            if(other->is_int_type()){
                rhs_float = static_cast<llf>(other->unwrap_int_obj()->getInt());
            
            } else if(other->is_float_type()){
                rhs_float = other->unwrap_float_obj()->getFloat();
            
            } else {
                throw std::logic_error("unsupported operand types(s) for /.");
            }

            if(rhs_float == 0){
                throw std::logic_error("division by zero.");
            }

            PyFloat* lhs = args[0]->unwrap_float_obj();
            llf lhs_float = lhs->getFloat();
            
            return new PyFloat(lhs_float / rhs_float);
        })
    );

    /*this->define("__mod__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            PyObject* other = args[1];

            llf rhs_float;

            if(other->is_int_type()){
                rhs_float = static_cast<llf>(other->unwrap_int_obj()->getInt());
            
            } else if(other->is_float_type()){
                rhs_float = other->unwrap_float_obj()->getFloat();
            
            } else {
                throw std::logic_error("unsupported operand types(s) for %.");
            }

            if(rhs_float == 0){
                throw std::logic_error("modulo by zero.");
            }

            PyFloat* lhs = args[0]->unwrap_float_obj();
            llf lhs_float = lhs->getFloat();
            
            return new PyFloat(lhs_float % rhs_float);
        })
    );*/

    this->define("__lt__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];
            llf rhs_float;

            if(other->is_int_type()){
                rhs_float = static_cast<llf>(other->unwrap_int_obj()->getInt());
            
            } else if(other->is_float_type()){
                rhs_float = other->unwrap_float_obj()->getFloat();
            
            } else {
                throw std::logic_error("unsupported operand types(s) for <.");
            }
            PyFloat* lhs = args[0]->unwrap_float_obj();
            llf lhs_float = lhs->getFloat();
            
            return new PyBool(lhs_float < rhs_float);
        })
    );

    this->define("__gt__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];
            llf rhs_float;

            if(other->is_int_type()){
                rhs_float = static_cast<llf>(other->unwrap_int_obj()->getInt());
            
            } else if(other->is_float_type()){
                rhs_float = other->unwrap_float_obj()->getFloat();
            
            } else {
                throw std::logic_error("unsupported operand types(s) for >.");
            }
            PyFloat* lhs = args[0]->unwrap_float_obj();
            llf lhs_float = lhs->getFloat();
            
            return new PyBool(lhs_float > rhs_float);
        })
    );

    this->define("__le__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];
            llf rhs_float;

            if(other->is_int_type()){
                rhs_float = static_cast<llf>(other->unwrap_int_obj()->getInt());
            
            } else if(other->is_float_type()){
                rhs_float = other->unwrap_float_obj()->getFloat();
            
            } else {
                throw std::logic_error("unsupported operand types(s) for <=.");
            }
            PyFloat* lhs = args[0]->unwrap_float_obj();
            llf lhs_float = lhs->getFloat();
            
            return new PyBool(lhs_float <= rhs_float);
        })
    );

    this->define("__ge__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];
            llf rhs_float;

            if(other->is_int_type()){
                rhs_float = static_cast<llf>(other->unwrap_int_obj()->getInt());
            
            } else if(other->is_float_type()){
                rhs_float = other->unwrap_float_obj()->getFloat();
            
            } else {
                throw std::logic_error("unsupported operand types(s) for >=.");
            }
            PyFloat* lhs = args[0]->unwrap_float_obj();
            llf lhs_float = lhs->getFloat();
            
            return new PyBool(lhs_float >= rhs_float);
        })
    );

    this->define("__eq__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];
            llf rhs_float;

            if(other->is_int_type()){
                rhs_float = static_cast<llf>(other->unwrap_int_obj()->getInt());
            
            } else if(other->is_float_type()){
                rhs_float = other->unwrap_float_obj()->getFloat();
            
            } else {
                throw std::logic_error("unsupported operand types(s) for ==.");
            }
            PyFloat* lhs = args[0]->unwrap_float_obj();
            llf lhs_float = lhs->getFloat();
            
            return new PyBool(lhs_float == rhs_float);
        })
    );

    this->define("__ne__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];
            llf rhs_float;

            if(other->is_int_type()){
                rhs_float = static_cast<llf>(other->unwrap_int_obj()->getInt());
            
            } else if(other->is_float_type()){
                rhs_float = other->unwrap_float_obj()->getFloat();
            
            } else {
                throw std::logic_error("unsupported operand types(s) for !=.");
            }
            PyFloat* lhs = args[0]->unwrap_float_obj();
            llf lhs_float = lhs->getFloat();
            
            return new PyBool(lhs_float != rhs_float);
        })
    );

    this->define("__neg__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 1);

            PyFloat* self = args[0]->unwrap_float_obj();

            llf self_value = self->getFloat();
            
            return new PyFloat(-self_value);
        })
    );
}

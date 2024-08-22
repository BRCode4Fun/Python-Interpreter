#include "./pyStr.hpp"
#include "./pyBool.hpp"
#include "./pyFunction.hpp"

inline char upper(char c) { return c & 95; }
inline char lower(char c) { return c | 32; }
inline bool isalpha(char c) {
    return (c >= 'a' && c <= 'z') 
            || (c >= 'A' && c <= 'Z');
}
inline bool isnumeric(char c) {
    return (c >= '0' && c <= '9');
}

PyStr::PyStr(const std::string& v) 
    : PyObject(ObjectType::String, new std::string(v)) {}

void PyStr::registerMethods() {
    
    this->define("__str__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 1);

            return args[0]; // self
        })
    );

    this->define("__bool__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyStr* self = args[0]->unwrap_str_obj();

            const std::string& str = self->getStr();

            return new PyBool(str != "");
        })
    );

    this->define("__eq__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];

            if(!other->is_str_type()){
                return new PyBool(false);
            }
            PyStr* self = args[0]->unwrap_str_obj();
            const std::string& str_self = self->getStr();
            
            const std::string& str_other = other->unwrap_str_obj()->getStr();

            return new PyBool(str_self == str_other);
        })
    );

    this->define("__add__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];

            if(!other->is_str_type()){
                throw std::logic_error("can only concatenate str to str");
            }
            PyStr* self = args[0]->unwrap_str_obj();
            const std::string& str_self = self->getStr();
            
            const std::string& str_other = other->unwrap_str_obj()->getStr();

            return new PyStr(str_self + str_other);
        })
    );

    this->define("__mul__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {

            ASSERT_ARG_SIZE(args, 2);

            PyObject* other = args[1];

            if(!other->is_int_type()){
                throw std::logic_error("can't multiply sequence by non-int.");
            }
            PyStr* self = args[0]->unwrap_str_obj();

            const std::string& str_self = self->getStr();
            lld n = other->unwrap_int_obj()->getInt();

            if(n <= 0) return new PyStr("");
            
            std::string result;
            result.reserve(str_self.size() * n);

            for(lld i = 0; i < n; i++){
                result += str_self;
            }
            return new PyStr(result);
        })
    );

    this->define("__len__", 
        new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 1);

            PyStr* self = args[0]->unwrap_str_obj();

            const std::string& str = self->getStr();

            return new PyInt(str.size());
        })
    );
}

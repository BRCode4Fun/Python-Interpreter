#include "./PyBuiltin.hpp"
#include "../exceptions/runtime_exceptions.hpp"
#include "../interpreter/interpreter.hpp"
#include "./pyFunction.hpp"
#include "./pyStr.hpp"
#include "./pyNone.hpp"

PyBuiltin::PyBuiltin(
    std::function<PyObject*(std::vector<PyObject*>&, Interpreter*)> functor, 
    size_t arity
) : PyObject(ObjectType::Builtin), functor(functor), arity(arity) {}

void PyBuiltin::registerMethods() {
    
    this->define("__str__", new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
        
        ASSERT_ARG_SIZE(args, 1);

        return new PyStr("<built-in function>");
    }));

    this->define("__call__", this);
}

PyObject* PyBuiltin::call(std::vector<PyObject*>& args, Interpreter* interpreter){
    
    if(args.size() != this->arity){
        throw std::runtime_error("Expected " + std::to_string(this->arity) + " argument(s), but got " + std::to_string(args.size()));
    }

    if(this->functor){
        return this->functor(args, interpreter);
    }
    
    return new PyNone();
}

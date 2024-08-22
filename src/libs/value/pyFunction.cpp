#include "./pyFunction.hpp"
#include "../exceptions/runtime_exceptions.hpp"
#include "../interpreter/interpreter.hpp"
#include "./pyMethodWrapper.hpp"
#include "./pyStr.hpp"
#include "./pyNone.hpp"

PyFunction::PyFunction(
    std::function<PyObject*(std::vector<PyObject*>&, Interpreter*)> functor, 
    size_t arity
) : PyObject(ObjectType::Func), functor(functor), arity(arity) {  // undefined scope for builtins

}

PyFunction::PyFunction(FunctionNode* declaration, Scope* closure) 
    : PyObject(ObjectType::Func), declaration(declaration), closure(closure){

    this->fname = declaration->get_name();
    this->arity = declaration->get_params().size();
}

void PyFunction::registerMethods() {
    
    this->define("__str__", new PyFunction([](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
        
        ASSERT_ARG_SIZE(args, 1);

        return new PyStr("<function object>");
    }));

    this->define("__call__", this);
}

PyMethodWrapper* PyFunction::bind(PyObject* object){
    return new PyMethodWrapper(this, object);
}

PyObject* PyFunction::call(std::vector<PyObject*>& args, Interpreter* interpreter){
    
    if(this->functor){
        return this->functor(args, interpreter);
    
    } else if(this->declaration) {

        const std::vector<AstNode*>& params = this->declaration->get_params();

        if(args.size() != params.size()){
            throw std::runtime_error("Expected " + std::to_string(params.size()) + " argument(s), but got " + std::to_string(args.size()));
        }

        Scope* frame = this->closure;
        Scope* fnCallEnv = new Scope(frame);
            
        interpreter->pushContext(fnCallEnv);

        size_t expectedArgs = params.size();
        if(expectedArgs) {
            // Loop through parameters and define them in the context
            for(size_t paramIdx = 0; paramIdx < expectedArgs; ++paramIdx) {
                const std::string& paramName = params[paramIdx]->unwrap_name_node()->get_lexeme();
                interpreter->defineOnContext(paramName, args[paramIdx]);
            }
        }
        
        PyObject* retValue = nullptr;    
        try {
            // Visit the body of the function, switching back to the interpreter's context
            declaration->get_body()->accept(interpreter);
            
        } catch(ReturnException& re) {
            retValue = re.value;
        }
        interpreter->popContext();
        
        return retValue ? retValue : new PyNone();
    }
    return new PyNone();
}

#include "./interpreter.hpp"
#include "../token/token.hpp"
#include "../token/tokentype.hpp"
#include "../exceptions/runtime_exceptions.hpp" // break, continue, return
#include "../value/primitives.hpp"
//#include "../builtin/builtins.hpp"

Interpreter::Interpreter() {
    
    Scope* builtins = new Scope();
    pushContext(builtins);
    Scope* globals = new Scope(builtins);
    pushContext(globals);

    builtins->define("input", new PyBuiltin(
        [](std::vector<PyObject*> args, Interpreter* interpreter) -> PyObject* {
            
            ASSERT_ARG_SIZE(args, 0);

            std::string buffer;
            std::getline(std::cin, buffer);

            return new PyStr(buffer);
        }
    ));
}

PyObject* Interpreter::interpret(ProgramNode* node) {
    return node->body->accept(this);
}

PyObject* Interpreter::resolve(const std::string& method_name, PyObject* object, std::vector<PyObject*> args) {
    
    PyObject* method_obj = object->find(method_name);

    if(method_obj && method_obj->is_fn_type()){
        PyFunction* target = method_obj->unwrap_function_obj();
        return target->bind(object)->call(args, this);
    
    } else {
        throw std::runtime_error("Method not found or is not callable");
    }
}

inline const std::string& Interpreter::getString(PyObject* object){
    PyStr* _data = this->resolve("__str__", object)->unwrap_str_obj();
    return _data->getStr();
}

inline long long Interpreter::getInteger(PyObject* object){
    PyInt* _data = this->resolve("__int__", object)->unwrap_int_obj();
    return _data->getInt();
}

inline bool Interpreter::isTruthy(PyObject* object){
    try {
        PyBool* _data = this->resolve("__bool__", object)->unwrap_bool_obj();
        return _data->getBool();
    
    } catch(std::runtime_error&){
        // if no boolean method is found, try resolving the __len__ method
        try {
            PyInt* _data = this->resolve("__len__", object)->unwrap_int_obj();
            return _data->getInt() ? true : false;

        } catch(std::runtime_error&){
            // In the absence of a __bool__() or __len__() method, 
            // Python treats the existence of the object as an indicator of truth.
            return true;
        }
    }
}

PyObject* Interpreter::visitPrintNode(PrintNode* node) {

    PyObject* argValue = node->args != nullptr ? 
                            node->args[0].accept(this) : nullptr;

    if(argValue != nullptr) std::cout << getString(argValue);
    
    std::cout << '\n' << std::flush;

    return new PyNone();
}

PyObject* Interpreter::visitIntNode(IntNode* node){
    
    const std::string& lexeme = node->get_lexeme();
    PyObject* value = new PyInt(lexeme);
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitFloatNode(FloatNode* node){
    
    const std::string& lexeme = node->get_lexeme();
    PyObject* value = new PyFloat(lexeme);
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitFunctionNode(FunctionNode* node){

    const std::string& fname = node->get_name();
    PyFunction* value = new PyFunction(node, this->currentContext());
    this->defineOnContext(fname, value);

    return new PyNone();
}

PyObject* Interpreter::visitClassNode(ClassNode* node) {
    
    Scope* context = this->currentContext();
    Scope* classScope = new Scope(context);

    this->pushContext(classScope);
    node->get_body()->accept(this); // visit the class definition
    this->popContext();

    const std::string& kname = node->get_name();
    PyClass* value = new PyClass(kname, classScope);

    context->define(kname, value);

    return new PyNone();
}

PyObject* Interpreter::visitPropertyNode(PropertyNode* node) {
    
    PyObject* tp_object = node->object->accept(this);
    
    const std::string& target_name = 
        node->attribute->unwrap_name_node()->get_lexeme();
    
    PyObject* value = tp_object->find(target_name);
    
    return (value->is_fn_type() ? value->unwrap_function_obj()->bind(tp_object) : value);
}

PyObject* Interpreter::visitBlockNode(BlockNode* node) {
    
    for(auto& stt : node->statements){
        stt->accept(this);
    }
    return new PyNone();
}

PyObject* Interpreter::visitWhileNode(WhileNode* node){

    PyObject* condition = node->cond->accept(this);

    while(isTruthy(condition)){
        try {
            node->body->accept(this);
        } catch(BreakException& be) {
            break;
        } catch(ContinueException& ce) {
            ; // goes back to evaluate the condition again
        }
        condition = node->cond->accept(this);
    }
    return new PyNone();
}

PyObject* Interpreter::visitBreakNode(BreakNode* node) {
    throw BreakException();
    return nullptr; // unreachable
}

PyObject* Interpreter::visitContinueNode(ContinueNode* node) {
    throw ContinueException();
    return nullptr; // unreachable
}

PyObject* Interpreter::visitPassNode(PassNode* node) {
    return new PyNone(); // pass
}

PyObject* Interpreter::visitIfNode(IfNode* node) {

    PyObject* cond = node->cond->accept(this);
    
    if(isTruthy(cond)){
        return node->trueBranch->accept(this);
        
    } else {
        for(const auto& elif : node->elifBranches){
            PyObject* elifCond = elif.first->accept(this);
            if(isTruthy(elifCond)){
                return elif.second->accept(this);
            }
        }
        if(node->elseBranch != nullptr){
            return node->elseBranch->accept(this);
        }
    }
    return new PyNone(); // unreachable
}

PyObject* Interpreter::visitTernaryOpNode(TernaryOpNode* node) {

    PyObject* condition = node->cond->accept(this);

    if(isTruthy(condition)){
        return node->left->accept(this);
    } else {
        return node->right->accept(this);
    }
    return new PyNone(); // unreachable
}

PyObject* Interpreter::visitBinaryOpNode(BinaryOpNode* node)  {

    PyObject* lhs = node->left->accept(this);
    lhs->incRefCount();

    PyObject* rhs = node->right->accept(this);
    rhs->incRefCount();

    std::vector<PyObject*> args = {rhs};
    PyObject* value = nullptr;

    switch((*node).op.type){

        case TokenType::Plus: // a + b
            value = this->resolve("__add__", lhs, args);
            GC.pushObject(value);
            break;

        case TokenType::Minus: // a - b
            value = this->resolve("__sub__", lhs, args);
            GC.pushObject(value);
            break;

        case TokenType::Star: // a * b
            value = this->resolve("__mul__", lhs, args);
            GC.pushObject(value);
            break;

        case TokenType::Slash: // a / b
            value = this->resolve("__truediv__", lhs, args);
            GC.pushObject(value);
            break;

        case TokenType::Ampersand: // a & b
            value = this->resolve("__and__", lhs, args);
            GC.pushObject(value);
            break;

        case TokenType::Pipe: // a | b
            value = this->resolve("__or__", lhs, args);
            GC.pushObject(value);
            break;
        
        case TokenType::Caret: // a ^ b
            value = this->resolve("__xor__", lhs, args);
            GC.pushObject(value);
            break;

        case TokenType::Mod: // a % b
            value = this->resolve("__mod__", lhs, args);
            GC.pushObject(value);
            break;

        case TokenType::EqualEqual: // a == b
            try {
                value = this->resolve("__eq__", lhs, args);
            } catch(std::runtime_error&){
                
                try { // if a == b cannot be resolved, try b == a, which is equivalent
                    value = this->resolve("__eq__", rhs, {lhs});
                } catch(std::runtime_error&){
                    throw; // TODO: NotImplemented error
                }
            }
            GC.pushObject(value);
            break;

        case TokenType::BangEqual: // a != b
            try {
                value = this->resolve("__ne__", lhs, args);
            } catch(std::runtime_error&){

                try { // if a != b cannot be resolved, try not (a == b), which is equivalent
                    PyObject* temp = this->resolve("__eq__", lhs, args);
                    value = new PyBool(!isTruthy(temp));
                
                } catch(std::runtime_error&){

                    try { // if above methods cannot be resolved, try b != a, which is equivalent
                        value = this->resolve("__ne__", rhs, {lhs});
                    } catch(std::runtime_error&){
                        
                        try { // if above methods cannot be resolved, try not (b == a), which is equivalent
                            PyObject* temp = this->resolve("__eq__", rhs, {lhs});
                            value = new PyBool(!isTruthy(temp));
                        
                        } catch(std::runtime_error&) {
                            throw;
                        } 
                    }
                }
            }
            GC.pushObject(value);
            break;

        case TokenType::Less: // a < b
            try {
                value = this->resolve("__lt__", lhs, args);
            } catch(std::runtime_error&){
                try { // if a < b cannot be resolved, try b > a, which is equivalent
                    value = this->resolve("__gt__", rhs, {lhs});
                } catch(std::runtime_error&){
                    throw;
                }
            }
            GC.pushObject(value);
            break;

        case TokenType::Greater: // a > b
            try {
                value = this->resolve("__gt__", lhs, args);
            } catch(std::runtime_error&){
                try { // if a > b cannot be resolved, try b < a, which is equivalent
                    value = this->resolve("__lt__", rhs, {lhs});
                } catch(std::runtime_error&){
                    throw;
                }
            }
            GC.pushObject(value);
            break;

        case TokenType::LessEqual: // a <= b
            try {
                value = this->resolve("__le__", lhs, args);
            } catch(std::runtime_error&){
                try { // if a <= b cannot be resolved, try b >= a, which is equivalent
                    value = this->resolve("__ge__", rhs, {lhs});
                } catch(std::runtime_error&){
                    throw;
                }
            }
            GC.pushObject(value);
            break;

        case TokenType::GreaterEqual: // a >= b
            try {
                value = this->resolve("__ge__", lhs, args);
            } catch(std::runtime_error&){
                try { // if a >= b cannot be resolved, try b <= a, which is equivalent
                    value = this->resolve("__le__", rhs, {lhs});
                } catch(std::runtime_error&){
                    throw;
                }
            }
            GC.pushObject(value);
            break;

        case TokenType::LeftShift: // a << b
            value = this->resolve("__lshift__", lhs, args);
            GC.pushObject(value);
            break;

        case TokenType::RightShift: // a >> b
            value = this->resolve("__rshift__", lhs, args);
            GC.pushObject(value);
            break;

        case TokenType::Or: // a or b
            /*
             *  try to do short-circuit: if after evaluating the left operand, 
             *  the result of the logical expression is known, 
             *  do not evaluate the right operand
            */
            value = isTruthy(lhs) ? lhs : rhs;
            break;
        
        case TokenType::And: // a and b
            /*
             *  try to do short-circuit: if after evaluating the left operand, 
             *  the result of the logical expression is known, 
             *  do not evaluate the right operand
            */
           value = !isTruthy(lhs) ? lhs : rhs;
           break;

        default:
            throw std::logic_error("Unsupported binary operator");
    }
    lhs->decRefCount();
    rhs->decRefCount();

    return value;
}

PyObject* Interpreter::visitAssignNode(AssignNode* node) {
    
    AstNode* lhs = node->name;
    Scope* context = nullptr;
    std::string name;

    if(lhs->is_name_node()){

        name = lhs->unwrap_name_node()->get_lexeme();
        context = this->currentContext();
    
    } else if(lhs->is_property_node()){

        PropertyNode* pprt = lhs->unwrap_property_node();
        PyObject* object = pprt->object->accept(this);
        name = pprt->attribute->unwrap_name_node()->get_lexeme();
        context = object->getContext();

    } else {
        throw std::runtime_error("Unsupported target expression");
    }
    
    PyObject* value = node->value->accept(this);
    value->incRefCount();

    if(node->op.type == TokenType::Equals){ // a = b
        context->define(name, value);
    
    } else {
        PyObject* lhs_value = context->get(name);
        lhs_value->incRefCount();

        std::vector<PyObject*> args = {value};

        switch(node->op.type){
            case TokenType::PlusEqual: // a += b
                // TODO: __iadd__ in-place add
                value = this->resolve("__add__", lhs_value, args);
                break;
            
            case TokenType::MinusEqual: // a -= b
                // TODO: __isub__
                value = this->resolve("__sub__", lhs_value, args);
                break;

            case TokenType::StarEqual: // a *= b
                // TODO: __imul__
                value = this->resolve("__mul__", lhs_value, args);
                break;
            
            case TokenType::SlashEqual: // a /= b
                // TODO: __itruediv__
                value = this->resolve("__truediv__", lhs_value, args);
                break;

            case TokenType::ModEqual: // a %= b
                // TODO: __imod__
                value = this->resolve("__mod__", lhs_value, args);
                break;

            case TokenType::AndEqual: // a &= b
                // TODO: __iand__
                value = this->resolve("__and__", lhs_value, args);
                break;

            case TokenType::OrEqual: // a |= b
                // TODO: __ior__
                value = this->resolve("__or__", lhs_value, args);
                break;

            case TokenType::XorEqual: // a ^= b
                // TODO: __ixor__
                value = this->resolve("__xor__", lhs_value, args);
                break;
            
            case TokenType::LeftShiftEqual: // a <<= b
                // TODO: __ilshift__
                value = this->resolve("__lshift__", lhs_value, args);
                break;
            
            case TokenType::RightShiftEqual: // a >>= b
                // TODO: __irshift__
                value = this->resolve("__rshift__", lhs_value, args);
                break;
            
            default:
                throw std::runtime_error("Unsupported assignment operator");
        }
        context->define(name, value);
        lhs_value->decRefCount();
    }
    value->decRefCount();
    
    return value; // allows cascading
}

PyObject* Interpreter::visitNameNode(NameNode* node){

    const std::string& varname = node->get_lexeme();
    return getFromContext(varname);
}

PyObject* Interpreter::visitBooleanNode(BooleanNode* node){
    
    bool bl = node->value;
    PyObject* value = new PyBool(bl);
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitStringNode(StringNode* node){
    
    const std::string& lexeme = node->get_lexeme();
    PyObject* value = new PyStr(lexeme);
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitUnaryOpNode(UnaryOpNode* node){

    PyObject* rhs_value = node->right->accept(this);
    PyObject* value = nullptr;

    switch(node->op.type){
        
        case TokenType::Not: // not a
            value = new PyBool(!isTruthy(rhs_value));
            break;
        
        case TokenType::Minus: // -a
            value = this->resolve("__neg__", rhs_value);
            break;

        case TokenType::Tilde: // ~a
            value = this->resolve("__invert__", rhs_value);
            break;
        
        default:
            throw std::logic_error("Unsupported unary operator");
    }
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitNullNode(NullNode* expr){

    PyObject* value = new PyNone();
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitCallNode(CallNode* expr) {
    
    PyObject* callee = expr->caller->accept(this);
    
    std::vector<PyObject*> arguments;
    arguments.reserve(expr->args.size() + 1);
    for(AstNode* argNode : expr->args){
        arguments.push_back(argNode->accept(this));
    }
    
    if(callee->is_klass_type()){

        PyClass* klass = callee->unwrap_class_obj();

        PyInstance* instance = new PyInstance(klass);

        PyObject* initMethod = nullptr;

        try {
            initMethod = instance->find("__init__");
        
        } catch(const std::runtime_error&){
            ; // if no constructor is present, just ignore
        }

        if(initMethod && initMethod->is_fn_type()){
            PyFunction* target = initMethod->unwrap_function_obj();
            target->bind(instance)->call(arguments, this);
        }

        return instance;
    
    } else {

        PyObject* callable = callee->find("__call__");

        if(callable->is_builtin_fn_type()){
            return callable->unwrap_builtin_fn_obj()->call(arguments, this);

        } else if(callable->is_fn_type()){
            return callable->unwrap_function_obj()->call(arguments, this);
        
        } else if(callable->is_method_wrapper_type()){
            return callable->unwrap_method_obj()->call(arguments, this);

        } else {
            throw std::runtime_error("Object is not callable");
        }
    }
    return new PyNone();
}

PyObject* Interpreter::visitReturnNode(ReturnNode* node) {
    
    AstNode* value = node->value;
    PyObject* retValue = value ? value->accept(this) : new PyNone();
    throw ReturnException(retValue);
    return new PyNone(); // unreachable
}

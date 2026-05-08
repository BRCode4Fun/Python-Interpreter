#include "./interpreter.hpp"
#include <cmath>
#include <sstream>
#include "../token/token.hpp"
#include "../token/tokentype.hpp"
#include "../exceptions/runtime_exceptions.hpp" // break, continue, return
#include "../exceptions/python_errors.hpp"
#include "../value/primitives.hpp"
#include "../value/runtime_types.hpp"

Interpreter::Interpreter() {
    
    Scope* builtins = new Scope();
    pushContext(builtins);
    Scope* globals = new Scope(builtins);
    pushContext(globals);
    RuntimeTypes types = initialize_runtime_types(builtins, this);
    this->object_type = types.object_type;
    this->type_type = types.type_type;
    this->none_type = types.none_type;
    this->int_type = types.int_type;
    this->float_type = types.float_type;
    this->bool_type = types.bool_type;
    this->str_type = types.str_type;
    this->function_type = types.function_type;
    this->builtin_fn_type = types.builtin_fn_type;
    this->method_wrapper_type = types.method_wrapper_type;
}

PyObject* Interpreter::interpret(ProgramNode* node) {
    return node->body->accept(this);
}

PyObject* Interpreter::resolve(const std::string& method_name, PyObject* object, std::vector<PyObject*> args) {
    
    PyObject* method_obj = object->find(method_name);

    if(method_obj == nullptr) {
        this->throw_missing_method_error(object, method_name);
    }

    if(method_obj->is_fn_type()){
        PyObject* bound = method_obj->bind_callable(object, this->method_wrapper_type, method_name);
        this->validate_call_arity(bound, args.size());
        return bound->call_callable(args, this);
    
    } else {
        std::string type_name = "unknown";
        if(object != nullptr && object->get_type() != nullptr) {
            type_name = object->get_type()->get_name();
        }
        throw TypeError("Method '" + method_name + "' on type '" + type_name + "' is not callable");
    }
}

PyObject* Interpreter::try_resolve(const std::string& method_name, PyObject* object, std::vector<PyObject*> args) {
    try {
        return this->resolve(method_name, object, args);
    } catch(std::runtime_error&) {
        return nullptr;
    }
}

TokenType Interpreter::assignment_to_binary(TokenType assignment_op) {
    switch(assignment_op) {
        case TokenType::PlusEqual: return TokenType::Plus;
        case TokenType::MinusEqual: return TokenType::Minus;
        case TokenType::StarEqual: return TokenType::Star;
        case TokenType::SlashEqual: return TokenType::Slash;
        case TokenType::ModEqual: return TokenType::Mod;
        case TokenType::AndEqual: return TokenType::Ampersand;
        case TokenType::OrEqual: return TokenType::Pipe;
        case TokenType::XorEqual: return TokenType::Caret;
        case TokenType::LeftShiftEqual: return TokenType::LeftShift;
        case TokenType::RightShiftEqual: return TokenType::RightShift;
        default:
            throw RuntimeError("Unsupported assignment operator");
    }
}

PyObject* Interpreter::make_bool_obj(bool value) {
    return PyObject::create_bool(value, this->bool_type);
}

PyObject* Interpreter::make_int_obj(lld value) {
    return PyObject::create_int(value, this->int_type);
}

PyObject* Interpreter::make_float_obj(llf value) {
    return PyObject::create_float(value, this->float_type);
}

PyObject* Interpreter::make_str_obj(const std::string& value) {
    return PyObject::create_string(value, this->str_type);
}

PyObject* Interpreter::negate_truthy_obj(PyObject* value) {
    if(value == nullptr) {
        return nullptr;
    }
    return this->make_bool_obj(!this->isTruthy(value));
}

void Interpreter::throw_missing_method_error(PyObject* object, const std::string& method_name) {
    std::string type_name = "unknown";
    if(object != nullptr && object->get_type() != nullptr) {
        type_name = object->get_type()->get_name();
    }
    throw TypeError("Type '" + type_name + "' does not implement method '" + method_name + "'");
}

void Interpreter::validate_call_arity(PyObject* callable, size_t provided_arg_count) {
    if(callable == nullptr || !callable->is_callable_type()) {
        return;
    }
    size_t expected_arg_count = callable->expected_call_arity();
    if(expected_arg_count != provided_arg_count) {
        throw TypeError("Expected " + std::to_string(expected_arg_count) + " argument(s), but got " + std::to_string(provided_arg_count));
    }
}

inline const std::string& Interpreter::getString(PyObject* object){
    PyObject* text = this->resolve("__str__", object)->unwrap_str_obj();
    return text->as_string();
}

inline long long Interpreter::getInteger(PyObject* object){
    PyObject* numeric = this->resolve("__int__", object)->unwrap_int_obj();
    return numeric->as_int();
}

inline bool Interpreter::isTruthy(PyObject* object){
    PyObject* boolean_method_value = this->try_resolve("__bool__", object);
    if(boolean_method_value != nullptr && boolean_method_value->is_bool_type()) {
        return boolean_method_value->as_bool();
    }

    PyObject* len_method_value = this->try_resolve("__len__", object);
    if(len_method_value != nullptr && len_method_value->is_int_type()) {
        return len_method_value->as_int() != 0LL;
    }
    return true; // the single existence of an object is considered truthy by default
}

inline bool Interpreter::isNumeric(PyObject* object){
    return object->is_int_type() || object->is_float_type() || object->is_bool_type();
}

inline long double Interpreter::toFloat(PyObject* object){
    PyObject* float_value = this->resolve("__float__", object)->unwrap_float_obj();
    return float_value->as_float();
}

PyObject* Interpreter::evalBinary(TokenType op, PyObject* lhs, PyObject* rhs){
    
    std::vector<PyObject*> args = {rhs};
    std::vector<PyObject*> reverse_args = {lhs};

    PyObject* result = nullptr;
    switch(op) {
        case TokenType::Plus:
            return this->resolve("__add__", lhs, args); // a + b

        case TokenType::Minus:
            return this->resolve("__sub__", lhs, args); // a - b

        case TokenType::Star:
            return this->resolve("__mul__", lhs, args); // a * b

        case TokenType::Slash:
            return this->resolve("__truediv__", lhs, args); // a / b

        case TokenType::Ampersand:
            return this->resolve("__and__", lhs, args); // a & b

        case TokenType::Pipe:
            return this->resolve("__or__", lhs, args); // a | b

        case TokenType::Caret:
            return this->resolve("__xor__", lhs, args); // a ^ b

        case TokenType::Mod:
            return this->resolve("__mod__", lhs, args); // a % b

        case TokenType::LeftShift:
            return this->resolve("__lshift__", lhs, args); // a << b

        case TokenType::RightShift:
            return this->resolve("__rshift__", lhs, args); // a >> b

        case TokenType::EqualEqual:
            result = this->try_resolve("__eq__", lhs, args); // a == b

            if(result) return result;
            return this->resolve("__eq__", rhs, reverse_args); // b == a
            
        case TokenType::BangEqual:
            result = this->try_resolve("__ne__", lhs, args); // a != b
            if(result) return result;
            result = this->negate_truthy_obj(this->try_resolve("__eq__", lhs, args)); // !(a == b)
            if(result) return result;
            result = this->try_resolve("__ne__", rhs, reverse_args); // b != a
            if(result) return result;
            result = this->negate_truthy_obj(this->try_resolve("__eq__", rhs, reverse_args)); // !(b == a)
            if(result) return result;
            this->throw_missing_method_error(lhs, "__ne__");

        case TokenType::Less:
            result = this->try_resolve("__lt__", lhs, args); // a < b
            if(result) return result;
            result = this->try_resolve("__gt__", rhs, reverse_args); // b > a
            if(result) return result;
            result = this->negate_truthy_obj(this->try_resolve("__ge__", lhs, args)); // !(a >= b)
            if(result) return result;
            result = this->negate_truthy_obj(this->try_resolve("__le__", rhs, reverse_args)); // !(b <= a)
            if(result) return result;
            this->throw_missing_method_error(lhs, "__lt__");

        case TokenType::Greater:
            result = this->try_resolve("__gt__", lhs, args); // a > b
            if(result) return result;
            result = this->try_resolve("__lt__", rhs, reverse_args); // b < a
            if(result) return result;
            result = this->negate_truthy_obj(this->try_resolve("__le__", lhs, args)); // !(a <= b)
            if(result) return result;
            result = this->negate_truthy_obj(this->try_resolve("__ge__", rhs, reverse_args)); // !(b >= a)
            if(result) return result;
            this->throw_missing_method_error(lhs, "__gt__");

        case TokenType::LessEqual:
            result = this->try_resolve("__le__", lhs, args); // a <= b
            if(result) return result;
            result = this->try_resolve("__ge__", rhs, reverse_args); // b >= a
            if(result) return result;
            result = this->try_resolve("__lt__", lhs, args); // a < b
            if(result) return result;
            result = this->negate_truthy_obj(this->try_resolve("__gt__", lhs, args)); // !(a > b)
            if(result) return result;
            result = this->negate_truthy_obj(this->try_resolve("__lt__", rhs, reverse_args)); // !(b < a)
            if(result) return result;
            this->throw_missing_method_error(lhs, "__le__");

        case TokenType::GreaterEqual:
            result = this->try_resolve("__ge__", lhs, args); // a >= b
            if(result) return result;
            result = this->try_resolve("__le__", rhs, reverse_args); // b <= a
            if(result) return result;
            result = this->try_resolve("__gt__", lhs, args); // a > b
            if(result) return result;
            result = this->negate_truthy_obj(this->try_resolve("__lt__", lhs, args)); // !(a < b)
            if(result) return result;
            result = this->negate_truthy_obj(this->try_resolve("__gt__", rhs, reverse_args)); // !(b > a)
            if(result) return result;
            this->throw_missing_method_error(lhs, "__ge__");

        default:
            break;
    }
    throw RuntimeError("Unsupported binary operator");
}

PyObject* Interpreter::visitPrintNode(PrintNode* node) {

    PyObject* argValue = node->args != nullptr ? 
                            node->args[0].accept(this) : nullptr;

    if(argValue != nullptr) std::cout << getString(argValue);
    
    std::cout << '\n' << std::flush;

    return PyObject::create_none(this->none_type);
}

PyObject* Interpreter::visitIntNode(IntNode* node){
    
    const std::string& lexeme = node->get_lexeme();
    PyObject* value = PyObject::create_int(std::stoll(lexeme), this->int_type);
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitFloatNode(FloatNode* node){
    
    const std::string& lexeme = node->get_lexeme();
    PyObject* value = PyObject::create_float(std::stold(lexeme), this->float_type);
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitFunctionNode(FunctionNode* node){

    const std::string& fname = node->get_name();
    PyObject* value = PyObject::create_user_function(node, this->currentContext(), this->function_type);
    this->defineOnContext(fname, value);

    return PyObject::create_none(this->none_type);
}

PyObject* Interpreter::visitClassNode(ClassNode* node) {
    
    Scope* context = this->currentContext();
    Scope* classScope = new Scope(context);

    this->pushContext(classScope);
    node->get_body()->accept(this); // visit the class definition
    this->popContext();

    const std::string& kname = node->get_name();
    PyTypeObject* value = new PyTypeObject(kname, this->type_type, {this->object_type}, classScope);

    context->define(kname, value);

    return PyObject::create_none(this->none_type);
}

PyObject* Interpreter::visitPropertyNode(PropertyNode* node) {
    
    PyObject* tp_object = node->object->accept(this);
    
    const std::string& target_name = 
        node->attribute->unwrap_name_node()->get_lexeme();
    
    PyObject* value = tp_object->find(target_name);
    
    return (value->is_fn_type() ? value->bind_callable(tp_object, this->method_wrapper_type, target_name) : value);
}

PyObject* Interpreter::visitBlockNode(BlockNode* node) {
    
    for(AstNode* stt : node->statements){
        stt->accept(this);
    }
    return PyObject::create_none(this->none_type);
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
    return PyObject::create_none(this->none_type);
}

PyObject* Interpreter::visitBreakNode(BreakNode* node) {
    
    throw BreakException();

    return PyObject::create_none(this->none_type); // unreachable
}

PyObject* Interpreter::visitContinueNode(ContinueNode* node) {
    
    throw ContinueException();

    return PyObject::create_none(this->none_type); // unreachable
}

PyObject* Interpreter::visitPassNode(PassNode* node) {
    return PyObject::create_none(this->none_type); // pass
}

PyObject* Interpreter::visitIfNode(IfNode* node) {

    PyObject* cond = node->cond->accept(this);
    
    if(isTruthy(cond)){
        return node->trueBranch->accept(this);
        
    } else {
        for(const std::pair<AstNode*, AstNode*>& elif : node->elifBranches){
            PyObject* elifCond = elif.first->accept(this);
            if(isTruthy(elifCond)){
                return elif.second->accept(this);
            }
        }
        if(node->elseBranch != nullptr){
            return node->elseBranch->accept(this);
        }
    }
    return PyObject::create_none(this->none_type); // unreachable
}

PyObject* Interpreter::visitTernaryOpNode(TernaryOpNode* node) {

    PyObject* condition = node->cond->accept(this);

    if(isTruthy(condition)){
        return node->left->accept(this);
    } else {
        return node->right->accept(this);
    }
    return PyObject::create_none(this->none_type); // unreachable
}

PyObject* Interpreter::visitBinaryOpNode(BinaryOpNode* node)  {
    PyObject* lhs = node->left->accept(this);
    lhs->incRefCount();
    if(node->op.type == TokenType::Or) { // a or b
        /*
        *  try to do short-circuit: if after evaluating the left operand, 
        *  the result of the logical expression is known, 
        *  do not evaluate the right operand
        */
        if(isTruthy(lhs)) {
            lhs->decRefCount();
            return lhs;
        }
        PyObject* rhs = node->right->accept(this);
        lhs->decRefCount();
        return rhs;
    }

    if(node->op.type == TokenType::And) { // a and b
        /*
        *  try to do short-circuit: if after evaluating the left operand, 
        *  the result of the logical expression is known, 
        *  do not evaluate the right operand
        */
        if(!isTruthy(lhs)) {
            lhs->decRefCount();
            return lhs;
        }
        PyObject* rhs = node->right->accept(this);
        lhs->decRefCount();
        return rhs;
    }

    PyObject* rhs = node->right->accept(this);
    rhs->incRefCount();
    PyObject* value = this->evalBinary(node->op.type, lhs, rhs);
    GC.pushObject(value);
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
        throw RuntimeError("Unsupported target expression");
    }
    
    PyObject* value = node->value->accept(this);
    value->incRefCount();

    if(node->op.type == TokenType::Equals){ // a = b
        context->define(name, value);
    
    } else {
        PyObject* lhs_value = context->get(name);
        lhs_value->incRefCount();
        value = this->evalBinary(this->assignment_to_binary(node->op.type), lhs_value, value);
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
    PyObject* value = PyObject::create_bool(bl, this->bool_type);
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitStringNode(StringNode* node){
    
    const std::string& lexeme = node->get_lexeme();
    PyObject* value = PyObject::create_string(lexeme, this->str_type);
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitUnaryOpNode(UnaryOpNode* node){

    PyObject* rhs_value = node->right->accept(this);
    PyObject* value = nullptr;

    switch(node->op.type){
        
        case TokenType::Not: // not a
            value = PyObject::create_bool(!isTruthy(rhs_value), this->bool_type);
            break;
        
        case TokenType::Minus: // -a
            if(isNumeric(rhs_value)) {
                if(rhs_value->is_float_type()) {
                    value = PyObject::create_float(-rhs_value->as_float(), this->float_type);
                } else {
                    value = PyObject::create_int(-getInteger(rhs_value), this->int_type);
                }
            } else {
                value = this->resolve("__neg__", rhs_value);
            }
            break;

        case TokenType::Tilde: // ~a
            if(rhs_value->is_float_type()) {
                throw TypeError("unsupported operand type for ~");
            }
            if(rhs_value->is_int_type() || rhs_value->is_bool_type()) {
                value = PyObject::create_int(~getInteger(rhs_value), this->int_type);
            } else {
                value = this->resolve("__invert__", rhs_value);
            }
            break;
        
        default:
            throw RuntimeError("Unsupported unary operator");
    }
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitNullNode(NullNode* expr){

    PyObject* value = PyObject::create_none(this->none_type);
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
    
    if(callee->is_type_type()) {
        return callee->unwrap_type_obj()->call(arguments, this);
    }
    
    if(callee->is_callable_type()) {
        this->validate_call_arity(callee, arguments.size());
        return callee->call_callable(arguments, this);
    }

    PyObject* callable = callee->find("__call__");
    if(callable != nullptr && callable->is_fn_type()) {
        PyObject* bound_callable = callable->bind_callable(callee, this->method_wrapper_type, "__call__");
        this->validate_call_arity(bound_callable, arguments.size());
        return bound_callable->call_callable(arguments, this);
    }

    if(callable != nullptr && callable->is_callable_type()) {
        this->validate_call_arity(callable, arguments.size());
        return callable->call_callable(arguments, this);
    }
    throw TypeError("Object is not callable");
}

PyObject* Interpreter::visitReturnNode(ReturnNode* node) {
    
    AstNode* value = node->value;
    PyObject* retValue = value ? value->accept(this) : PyObject::create_none(this->none_type);
    throw ReturnException(retValue);
    
    return PyObject::create_none(this->none_type); // unreachable
}

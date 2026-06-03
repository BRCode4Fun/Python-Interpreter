#include <iostream>
#include "interpreter.hpp"
#include "../token/token_type.hpp"

#include "../exceptions/runtime_exceptions.hpp" // break, continue, return
#include "../exceptions/python_exceptions.hpp"
#include "../value/primitives.hpp"
#include "../value/py_object.hpp"
#include "../value/runtime_types.hpp"
    
Interpreter::Interpreter() {
    root_builtins_ = std::make_unique<Scope>();
    push_borrowed(root_builtins_.get());
    root_globals_ = std::make_unique<Scope>(root_builtins_.get());
    push_borrowed(root_globals_.get());
    RuntimeTypes types = initialize_runtime_types(root_builtins_.get(), this);
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

void Interpreter::push_borrowed(Scope* frame) {
    context_stack.push_back({frame, nullptr});
}

void Interpreter::push_owned(std::unique_ptr<Scope> frame) {
    Scope* p = frame.get();
    context_stack.push_back({p, std::move(frame)});
}

void Interpreter::pop_context() {

    if(context_stack.empty()) {
        throw RuntimeError("Cannot pop context from empty stack");
    }
    context_stack.pop_back();
}

Interpreter::~Interpreter() {
    collect_garbage();
    /* Free heap frames flagged on push (e.g. each call_callable activation record). */
    while(!context_stack.empty()) {
        pop_context();
    }
    /* GC only tracked pointers; Scopes now tear down objects — clear registry before ~GarbageCollector to avoid dangling pointers. */
    gc.relinquish_all_registrations();
    root_globals_.reset();
    root_builtins_.reset();
    /* Static type/singleton refs would otherwise point at objects already destroyed by Scopes. */
    PyObject::clear_runtime_statics();
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
        PyObject* result = bound->call_callable(args, this);
        track_temp(bound);
        return result;

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
    return this->make_bool_obj(!this->is_truthy(value));
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

inline const std::string& Interpreter::get_string(PyObject* object){
    PyObject* text = this->resolve("__str__", object)->unwrap_str_obj();
    return text->as_string();
}

inline long long Interpreter::get_integer(PyObject* object){
    PyObject* numeric = this->resolve("__int__", object)->unwrap_int_obj();
    return numeric->as_int();
}

inline bool Interpreter::is_truthy(PyObject* object){
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

inline bool Interpreter::is_numeric(PyObject* object){
    return object->is_int_type() || object->is_float_type() || object->is_bool_type();
}

inline long double Interpreter::to_float(PyObject* object){
    PyObject* float_value = this->resolve("__float__", object)->unwrap_float_obj();
    return float_value->as_float();
}

PyObject* Interpreter::eval_binary(TokenType op, PyObject* lhs, PyObject* rhs){
    
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

PyObject* Interpreter::visit_print_node(PrintNode* node) {

    PyObject* arg_value = node->args ? node->args->accept(this) : nullptr;

    if(arg_value != nullptr) std::cout << get_string(arg_value);
    
    std::cout << '\n' << std::flush;

    return PyObject::create_none(this->none_type);
}

PyObject* Interpreter::visit_int_node(IntNode* node){
    
    const std::string& lexeme = node->get_lexeme();
    PyObject* value = PyObject::create_int(std::stoll(lexeme), this->int_type);
    track_temp(value);
    return value;
}

PyObject* Interpreter::visit_float_node(FloatNode* node){
    
    const std::string& lexeme = node->get_lexeme();
    PyObject* value = PyObject::create_float(std::stold(lexeme), this->float_type);
    track_temp(value);
    return value;
}

PyObject* Interpreter::visit_function_node(FunctionNode* node){

    const std::string& fname = node->get_name();
    Scope* closure_tip = this->current_context();
    /* Snapshot only for defs nested under an active call: that frame is deleted on pop; clone preserves the closure.
     * Module-level and class body keep the live Scope (recursion / methods defined later). */
    const bool snapshot_closure = (closure_tip != root_globals_.get())
        && (closure_tip != active_class_scope_);
    PyObject* value =
        PyObject::create_user_function(node, closure_tip, this->function_type, snapshot_closure);
    this->define_on_context(fname, value);

    return PyObject::create_none(this->none_type);
}

PyObject* Interpreter::visit_class_node(ClassNode* node) {
    
    Scope* context = this->current_context();
    auto class_scope = std::make_unique<Scope>(context);

    /* Nested classes: save/restore so active_class_scope_ always refers to the innermost body being executed. */
    Scope* const prev_active_class_scope = active_class_scope_;
    active_class_scope_ = class_scope.get();

    this->push_borrowed(class_scope.get());
    node->get_body()->accept(this); // visit the class definition
    this->pop_context();

    active_class_scope_ = prev_active_class_scope;

    const std::string& kname = node->get_name();
    PyTypeObject* value = new PyTypeObject(kname, this->type_type, {this->object_type}, std::move(class_scope));

    context->define(kname, value);

    return PyObject::create_none(this->none_type);
}

PyObject* Interpreter::visit_property_node(PropertyNode* node) {
    
    PyObject* tp_object = node->object->accept(this);
    
    const std::string& target_name = 
        node->attribute->unwrap_name_node()->get_lexeme();
    
    PyObject* value = tp_object->find(target_name);

    if(value->is_fn_type()) {
        /* Do not track_temp(bound) here: visit_call may evaluate args after callee is known;
         * a sweep could reclaim rc==0 wrappers still needed for call_callable(this). */
        return value->bind_callable(tp_object, this->method_wrapper_type, target_name);
    }
    return value;
}

PyObject* Interpreter::visit_block_node(BlockNode* node) {
    
    for(const std::unique_ptr<AstNode>& stt : node->statements){
        stt->accept(this);
        maybe_collect();
    }
    return PyObject::create_none(this->none_type);
}

PyObject* Interpreter::visit_while_node(WhileNode* node){

    PyObject* condition = node->cond->accept(this);

    while(is_truthy(condition)){
        try {
            node->body->accept(this);
        } catch(BreakException& be) {
            break;
        } catch(ContinueException& ce) {
            ; // goes back to evaluate the condition again
        }
        track_temp(condition);
        maybe_collect();
        condition = node->cond->accept(this);
    }
    track_temp(condition);
    maybe_collect();
    return PyObject::create_none(this->none_type);
}

PyObject* Interpreter::visit_break_node(BreakNode* node) {
    
    throw BreakException();

    return PyObject::create_none(this->none_type); // unreachable
}

PyObject* Interpreter::visit_continue_node(ContinueNode* node) {
    
    throw ContinueException();

    return PyObject::create_none(this->none_type); // unreachable
}

PyObject* Interpreter::visit_pass_node(PassNode* node) {
    return PyObject::create_none(this->none_type); // pass
}

PyObject* Interpreter::visit_if_node(IfNode* node) {

    PyObject* cond = node->cond->accept(this);

    if(is_truthy(cond)){
        track_temp(cond);
        return node->true_branch->accept(this);

    }
    track_temp(cond);
    for(const auto& elif : node->elif_branches){
        PyObject* elif_cond = elif.first->accept(this);
        if(is_truthy(elif_cond)){
            track_temp(elif_cond);
            return elif.second->accept(this);
        }
        track_temp(elif_cond);
    }
    if(node->else_branch){
        return node->else_branch->accept(this);
    }
    return PyObject::create_none(this->none_type); // unreachable
}

PyObject* Interpreter::visit_ternary_op_node(TernaryOpNode* node) {

    PyObject* condition = node->cond->accept(this);

    if(is_truthy(condition)){
        track_temp(condition);
        return node->left->accept(this);
    }
    track_temp(condition);
    return node->right->accept(this);
}

PyObject* Interpreter::visit_binary_op_node(BinaryOpNode* node)  {
    PyObject* lhs = node->left->accept(this);
    lhs->inc_ref_count();
    if(node->op.type == TokenType::Or) { // a or b
        /*
        *  try to do short-circuit: if after evaluating the left operand, 
        *  the result of the logical expression is known, 
        *  do not evaluate the right operand
        */
        if(is_truthy(lhs)) {
            lhs->dec_ref_count();
            return lhs;
        }
        PyObject* rhs = node->right->accept(this);
        lhs->dec_ref_count();
        return rhs;
    }

    if(node->op.type == TokenType::And) { // a and b
        /*
        *  try to do short-circuit: if after evaluating the left operand, 
        *  the result of the logical expression is known, 
        *  do not evaluate the right operand
        */
        if(!is_truthy(lhs)) {
            lhs->dec_ref_count();
            return lhs;
        }
        PyObject* rhs = node->right->accept(this);
        lhs->dec_ref_count();
        return rhs;
    }

    PyObject* rhs = node->right->accept(this);
    rhs->inc_ref_count();
    PyObject* value = this->eval_binary(node->op.type, lhs, rhs);
    track_temp(value);
    lhs->dec_ref_count();
    rhs->dec_ref_count();
    return value;
}

PyObject* Interpreter::visit_assign_node(AssignNode* node) {
    
    AstNode* lhs = node->name.get();
    Scope* context = nullptr;
    std::string name;

    if(lhs->is_name_node()){ 

        name = lhs->unwrap_name_node()->get_lexeme();
        context = this->current_context();
    
    } else if(lhs->is_property_node()){

        PropertyNode* pprt = lhs->unwrap_property_node();
        PyObject* object = pprt->object->accept(this);
        name = pprt->attribute->unwrap_name_node()->get_lexeme();
        context = object->get_context();

    } else {
        throw RuntimeError("Unsupported target expression");
    }
    
    PyObject* value = node->value->accept(this);
    value->inc_ref_count();

    if(node->op.type == TokenType::Equals){ // a = b
        context->define(name, value);
    
    } else {
        PyObject* lhs_value = context->get(name);
        lhs_value->inc_ref_count();
        value = this->eval_binary(this->assignment_to_binary(node->op.type), lhs_value, value);
        context->define(name, value);
        lhs_value->dec_ref_count();
    }
    value->dec_ref_count();
    
    return value; // allows cascading
}

PyObject* Interpreter::visit_name_node(NameNode* node){

    const std::string& varname = node->get_lexeme();
    return get_from_context(varname);
}

PyObject* Interpreter::visit_boolean_node(BooleanNode* node){
    
    bool bl = node->value;
    PyObject* value = PyObject::create_bool(bl, this->bool_type);
    track_temp(value);
    return value;
}

PyObject* Interpreter::visit_string_node(StringNode* node){
    
    const std::string& lexeme = node->get_lexeme();
    PyObject* value = PyObject::create_string(lexeme, this->str_type);
    track_temp(value);
    return value;
}

PyObject* Interpreter::visit_unary_op_node(UnaryOpNode* node){

    PyObject* rhs_value = node->right->accept(this);
    PyObject* value = nullptr;

    switch(node->op.type){
        
        case TokenType::Not: // not a
            value = PyObject::create_bool(!is_truthy(rhs_value), this->bool_type);
            break;
        
        case TokenType::Minus: // -a
            if(is_numeric(rhs_value)) {
                if(rhs_value->is_float_type()) {
                    value = PyObject::create_float(-rhs_value->as_float(), this->float_type);
                } else {
                    value = PyObject::create_int(-get_integer(rhs_value), this->int_type);
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
                value = PyObject::create_int(~get_integer(rhs_value), this->int_type);
            } else {
                value = this->resolve("__invert__", rhs_value);
            }
            break;
        
        default:
            throw RuntimeError("Unsupported unary operator");
    }
    track_temp(value);
    return value;
}

PyObject* Interpreter::visit_null_node(NullNode* expr){

    PyObject* value = PyObject::create_none(this->none_type);
    track_temp(value);
    return value;
}

PyObject* Interpreter::visit_call_node(CallNode* expr) {
    
    PyObject* callee = expr->caller->accept(this);
    
    std::vector<PyObject*> arguments;
    arguments.reserve(expr->args.size() + 1);
    for(const std::unique_ptr<AstNode>& arg_node : expr->args){
        arguments.push_back(arg_node->accept(this));
    }
    
    if(callee->is_type_type()) {
        PyObject* ret = callee->unwrap_type_obj()->call(arguments, this);
        track_temp(callee);
        return ret;
    }
    
    if(callee->is_callable_type()) {
        this->validate_call_arity(callee, arguments.size());
        PyObject* ret = callee->call_callable(arguments, this);
        track_temp(callee);
        return ret;
    }

    PyObject* callable = callee->find("__call__");
    if(callable != nullptr && callable->is_fn_type()) {
        PyObject* bound_callable = callable->bind_callable(callee, this->method_wrapper_type, "__call__");
        this->validate_call_arity(bound_callable, arguments.size());
        PyObject* ret = bound_callable->call_callable(arguments, this);
        track_temp(bound_callable);
        track_temp(callee);
        return ret;
    }

    if(callable != nullptr && callable->is_callable_type()) {
        this->validate_call_arity(callable, arguments.size());
        PyObject* ret = callable->call_callable(arguments, this);
        track_temp(callee);
        return ret;
    }
    throw TypeError("Object is not callable");
}

PyObject* Interpreter::visit_return_node(ReturnNode* node) {
    
    AstNode* value = node->value.get();
    PyObject* ret_value = value ? value->accept(this) : PyObject::create_none(this->none_type);
    throw ReturnException(ret_value);
    
    return PyObject::create_none(this->none_type); // unreachable
}

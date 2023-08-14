#include "interpreter.hpp"
#include "../token/tokentype.hpp"
#include <stdexcept>

void todo() {
    throw std::logic_error("Function not implemented yet");
}

Value* Interpreter::visitProgramNode(ProgramNode* node) {
    return node->body->accept(this);
}

Value* Interpreter::visitPrintNode(PrintNode* node) {
    Value* argValue = nullptr;

    if (node->args != nullptr) {
        argValue = node->args[0].accept(this);
    }
    if (argValue != nullptr) {
        std::cout << *(argValue);
    }
    std::cout << "\n" << std::flush;

    return new Value(1.0L);
}

Value* Interpreter::visitIntNode(IntNode* node){
    Value* value = new Value(node->getValue());
    GC->Add_object(value);
    return value;
}

Value* Interpreter::visitFloatNode(FloatNode* node){
    Value* value = new Value(node->getValue());
    GC->Add_object(value);
    return value;
}

Value* Interpreter::visitFunctionNode(FunctionNode* node){

    Value* value = new Value(node);
    const std::string& fname = node->getName();
    Global_Environment->define(fname, value);
    
    return new Value();
}

Value* Interpreter::visitBlockNode(BlockNode* node) {

    Value* ret_value = nullptr;

    for(auto statement : node->statements) {

    	ret_value = statement->accept(this);
               
        if(statement->type == AstNodeType::Return){           
           break;
        }
    }
    return ret_value;
}

Value* Interpreter::visitWhileNode(WhileNode* node){

    Value* cond = node->cond->accept(this);

     while(cond->isTruthy()){
     	node->body->accept(this);
        cond = node->cond->accept(this);
     }
     return new Value(-1.0L);
}

Value* Interpreter::visitIfNode(IfNode* node) {

    Value* cond = node->cond->accept(this);

    if (cond->isTruthy()) {
        return node->trueBranch->accept(this);
    } else {
        for (const auto& elif : node->elifBranches) {
            Value* elifCond = elif.first->accept(this);
            if (elifCond->isTruthy()) {
                return elif.second->accept(this);
            }
        }
        if (node->elseBranch != nullptr) {
            return node->elseBranch->accept(this);
        }
    }
    return new Value(-1.0L);
}

Value* Interpreter::visitTernaryOpNode(TernaryOpNode* node) {

    Value* cond = node->cond->accept(this);

    if(cond->isTruthy()) {
        return node->left->accept(this);
    } else {
        return node->right->accept(this);
    }
}

Value* Interpreter::visitBinaryOpNode(BinaryOpNode* node)  {

    Value* leftValue = node->left->accept(this);
    leftValue->Increment_Reference_counting();

    Value* rightValue = node->right->accept(this);
    rightValue->Increment_Reference_counting();

    Value* value = nullptr;

    switch(node->op.type) {
        case TokenType::Plus: // TODO: replace with __add__ call
            value = *leftValue + *rightValue;
            GC->Add_object(value);
            break;
        case TokenType::Minus: // TODO: replace with __sub__ call
            value = *leftValue - *rightValue;
            GC->Add_object(value); 
            break;
        case TokenType::Star: // TODO: replace with __mul__ call
            value = *leftValue * *rightValue;
            GC->Add_object(value); 
            break;
        case TokenType::Slash: // TODO: replace with __truediv__ call
            value = *leftValue / *rightValue;
            GC->Add_object(value); 
            break;
        case TokenType::Ampersand: // TODO: replace with __and__ call
            value = *leftValue & *rightValue;
            GC->Add_object(value); 
            break;
        case TokenType::Pipe: // TODO: replace with __or__ call
            value = *leftValue | *rightValue;
            GC->Add_object(value); 
            break;
        case TokenType::Caret: // TODO: replace with __xor__ call
            value = *leftValue ^ *rightValue;
            GC->Add_object(value); 
            break;
        case TokenType::Mod: // TODO: replace with __mod__ call
            value = *leftValue % *rightValue;
            GC->Add_object(value); 
            break;
        case TokenType::EqualEqual: // TODO: replace with __eq__ call
            value = *leftValue == *rightValue;
            GC->Add_object(value); 
            break;
        case TokenType::BangEqual: // TODO: replace with __ne__ call
            value = !(*(*leftValue == *rightValue));
            GC->Add_object(value);
            break;
        case TokenType::Less: // TODO: replace with __lt__ call
            value = *leftValue < *rightValue; 
            GC->Add_object(value);
            break;
        case TokenType::Greater: // TODO: replace with __gt__ call
            value = *leftValue > *rightValue;
            GC->Add_object(value); 
            break;
        case TokenType::LessEqual: // TODO: replace with __le__ call
            value = !(*(*leftValue > *rightValue));
            GC->Add_object(value);
            break;
        case TokenType::GreaterEqual: // TODO: replace with __ge__ call
            value = !(*(*leftValue < *rightValue));
            GC->Add_object(value);
            break;
        case TokenType::LeftShift: // TODO: replace with __lshift__ call
            value = *leftValue << *rightValue;
            GC->Add_object(value); 
            break;
        case TokenType::RightShift: // TODO: replace with __rshift__ call
            value = *leftValue >> *rightValue;
            GC->Add_object(value); 
            break;
        case TokenType::Or:
            /*
             *  try to do short-circuit: if after evaluating the left operand, 
             *  the result of the logical expression is known, 
             *  do not evaluate the right operand
            */
            value = leftValue->isTruthy() ? leftValue : rightValue;
            break;
        case TokenType::And:
            /*
             *  try to do short-circuit: if after evaluating the left operand, 
             *  the result of the logical expression is known, 
             *  do not evaluate the right operand
            */
            value = !(leftValue->isTruthy()) ? leftValue : rightValue;
            break;
        default:
            throw std::runtime_error("Unsupported binary operator");
    } 
    leftValue->Decrement_Reference_counting();
    rightValue->Decrement_Reference_counting();

    return value;
}

Value* Interpreter::visitAssignNode(AssignNode* node) {

    auto value = node->value->accept(this);
    auto var_name = static_cast<NameNode*>(node->name)->getName();

    Environment.top()->define(var_name, value);

    return value;
}

Value* Interpreter::visitNameNode(NameNode* node){
    const std::string& varname = node->getName();
    return Environment.top()->get(varname);
}

Value* Interpreter::visitBooleanNode(BooleanNode* node){
    Value* value = new Value(node->value);
    GC->Add_object(value);
    return value;
}

Value* Interpreter::visitStringNode(StringNode* node){
    const std::string& str = node->getLexeme();
    Value* value = new Value(str);
    GC->Add_object(value);
    return value;
}

Value* Interpreter::visitUnaryOpNode(UnaryOpNode* node){

    Value* operandValue = node->right->accept(this);
    Value* result = nullptr;

    switch(node->op.type) {
        case TokenType::Minus:
            result = -(*operandValue);
            break;
        case TokenType::Not:
            result = !(*operandValue);
            break;
        case TokenType::Tilde:
            result = ~(*operandValue);
            break;
        default:
            throw std::runtime_error("Unsupported unary operator");
    }
    GC->Add_object(result);
    return result;
}

Value* Interpreter::visitNullNode(NullNode* expr){
    Value* value = new Value();
    GC->Add_object(value);
    return value;
}

Value* Interpreter::visitCallNode(CallNode* expr){

    const std::string& caller = static_cast<NameNode*>(expr->caller)->getName(); // if is NameNode
    Value* callee = Global_Environment->get(caller);

    FunctionNode* fndef = callee->getFunc().function_node; // rewrite
    Scope* func_env = new Scope();

    const std::vector<AstNode*>& params = fndef->getParams();

    for (size_t i = 0; i < expr->args.size(); i++){
        const std::string& arg_name = static_cast<NameNode*>(params[i])->getName(); // if is NameNode
        func_env->define(arg_name, expr->args[i]->accept(this));
    }
    Environment.push(func_env);
    Value* return_value = fndef->getBody()->accept(this);
    if(Environment.size() > 0) Environment.pop();

    return return_value;
}

Value* Interpreter::visitReturnNode(ReturnNode* node) {
    return node->value->accept(this);
}

Value* Interpreter::interpret(ProgramNode* node) {
    return node->accept(this);
}

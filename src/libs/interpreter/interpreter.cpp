#include <stdexcept>
#include "./interpreter.hpp"
#include "../token/tokentype.hpp"
#include "../exceptions/runtime_exceptions.hpp" // break, continue, return

void todo() {
    throw std::runtime_error("Function not implemented yet");
}

PyObject* Interpreter::visitProgramNode(ProgramNode* node) {
    return node->body->accept(this);
}

PyObject* Interpreter::visitPrintNode(PrintNode* node) {
    PyObject* argValue = nullptr;

    if (node->args != nullptr) {
        argValue = node->args[0].accept(this);
    }
    if (argValue != nullptr) {
        if((*argValue).isStr()) {
            const PyStr* value = dynamic_cast<const PyStr*>(argValue);
            std::cout << (*value).getStr();
        } else {
            std::cout << *argValue;
        }
    }
    std::cout << "\n" << std::flush;

    return new PyNone();
}

PyObject* Interpreter::visitIntNode(IntNode* node){
    const std::string& str = node->getLexeme();
    PyObject* value = new PyInt(str);
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitFloatNode(FloatNode* node){
    const std::string& str = node->getLexeme();
    PyObject* value = new PyFloat(str);
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitFunctionNode(FunctionNode* node){

    const std::string& fname = node->getName();
    PyFnObj* value = new PyFnObj(node);
    __globals__->define(fname, value);
    
    return new PyNone();
}

PyObject* Interpreter::visitBlockNode(BlockNode* node) {

    for(auto statement : node->statements) {
    	statement->accept(this);
    }
    return new PyNone();
}

PyObject* Interpreter::visitWhileNode(WhileNode* node){

    PyObject* cond = node->cond->accept(this);

    while(cond->isTruthy()){
     	try {
     	    node->body->accept(this);
     	} catch(BreakException be) {
     	    break;
     	} catch(ContinueException ce) {
     	    ; // goes to evaluate the condition again
     	}
        cond = node->cond->accept(this);
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

PyObject* Interpreter::visitIfNode(IfNode* node) {

    PyObject* cond = node->cond->accept(this);

    if (cond->isTruthy()) {
        return node->trueBranch->accept(this);
    } else {
        for (const auto& elif : node->elifBranches) {
            PyObject* elifCond = elif.first->accept(this);
            if (elifCond->isTruthy()) {
                return elif.second->accept(this);
            }
        }
        if (node->elseBranch != nullptr) {
            return node->elseBranch->accept(this);
        }
    }
    return new PyNone();
}

PyObject* Interpreter::visitTernaryOpNode(TernaryOpNode* node) {

    PyObject* cond = node->cond->accept(this);

    if(cond->isTruthy()) {
        return node->left->accept(this);
    } else {
        return node->right->accept(this);
    }
}

PyObject* Interpreter::visitBinaryOpNode(BinaryOpNode* node)  {

    PyObject* leftValue = node->left->accept(this);
    leftValue->incRc();

    PyObject* rightValue = node->right->accept(this);
    rightValue->incRc();

    PyObject* value = nullptr;

    switch(node->op.type) {
        case TokenType::Plus: // TODO: replace with __add__ call
            value = *leftValue + *rightValue;
            GC.pushObject(value);
            break;
        case TokenType::Minus: // TODO: replace with __sub__ call
            value = *leftValue - *rightValue;
            GC.pushObject(value); 
            break;
        case TokenType::Star: // TODO: replace with __mul__ call
            value = *leftValue * *rightValue;
            GC.pushObject(value); 
            break;
        case TokenType::Slash: // TODO: replace with __truediv__ call
            value = *leftValue / *rightValue;
            GC.pushObject(value); 
            break;
        case TokenType::Ampersand: // TODO: replace with __and__ call
            value = *leftValue & *rightValue;
            GC.pushObject(value); 
            break;
        case TokenType::Pipe: // TODO: replace with __or__ call
            value = *leftValue | *rightValue;
            GC.pushObject(value); 
            break;
        case TokenType::Caret: // TODO: replace with __xor__ call
            value = *leftValue ^ *rightValue;
            GC.pushObject(value); 
            break;
        case TokenType::Mod: // TODO: replace with __mod__ call
            value = *leftValue % *rightValue;
            GC.pushObject(value); 
            break;
        case TokenType::EqualEqual: // TODO: replace with __eq__ call
            value = *leftValue == *rightValue;
            GC.pushObject(value); 
            break;
        case TokenType::BangEqual: // TODO: replace with __ne__ call
            value = !(*(*leftValue == *rightValue));
            GC.pushObject(value);
            break;
        case TokenType::Less: // TODO: replace with __lt__ call
            value = *leftValue < *rightValue; 
            GC.pushObject(value);
            break;
        case TokenType::Greater: // TODO: replace with __gt__ call
            value = *leftValue > *rightValue;
            GC.pushObject(value); 
            break;
        case TokenType::LessEqual: // TODO: replace with __le__ call
            value = !(*(*leftValue > *rightValue));
            GC.pushObject(value);
            break;
        case TokenType::GreaterEqual: // TODO: replace with __ge__ call
            value = !(*(*leftValue < *rightValue));
            GC.pushObject(value);
            break;
        case TokenType::LeftShift: // TODO: replace with __lshift__ call
            value = *leftValue << *rightValue;
            GC.pushObject(value); 
            break;
        case TokenType::RightShift: // TODO: replace with __rshift__ call
            value = *leftValue >> *rightValue;
            GC.pushObject(value); 
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
            throw std::logic_error("Unsupported binary operator");
    } 
    leftValue->decRc();
    rightValue->decRc();

    return value;
}

PyObject* Interpreter::visitAssignNode(AssignNode* node) {

    PyObject* value = node->value->accept(this);
    const std::string& varName = static_cast<NameNode*>(node->name)->getLexeme();

    Scope* topEnv = currentEnv.top();
    topEnv->define(varName, value);

    return value;
}

PyObject* Interpreter::visitNameNode(NameNode* node){
    const std::string& varname = node->getLexeme();
    Scope* topEnv = currentEnv.top();
    return topEnv->get(varname);
}

PyObject* Interpreter::visitBooleanNode(BooleanNode* node){
    PyObject* value = new PyBool(node->value);
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitStringNode(StringNode* node){
    const std::string& str = node->getLexeme();
    PyObject* value = new PyStr(str);
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitUnaryOpNode(UnaryOpNode* node){

    PyObject* operandValue = node->right->accept(this);
    PyObject* result = nullptr;

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
            throw std::logic_error("Unsupported unary operator");
    }
    GC.pushObject(result);
    return result;
}

PyObject* Interpreter::visitNullNode(NullNode* expr){
    PyObject* value = new PyNone();
    GC.pushObject(value);
    return value;
}

PyObject* Interpreter::visitCallNode(CallNode* expr){

    NameNode* callerNode = static_cast<NameNode*>(expr->caller);
    const std::string& caller = callerNode->getLexeme(); // if is NameNode
    PyFnObj* callee = static_cast<PyFnObj*>(__globals__->get(caller));
    const FunctionNode* fndef = callee->getFunc();
    
    Scope* fnEnv = new Scope();
    const std::vector<AstNode*>& params = fndef->getParams();
    const std::vector<AstNode*>& args = expr->args;
    
    for (size_t i = 0; i < args.size(); i++){
        NameNode* paramNode = static_cast<NameNode*>(params[i]);
        const std::string& argName = paramNode->getLexeme(); // if is NameNode
        fnEnv->define(argName, args[i]->accept(this));
    }
    
    currentEnv.push(fnEnv);
    
    PyObject* retValue = nullptr;
    try {
        fndef->getBody()->accept(this);
    } catch(ReturnException re) {
        retValue = re.value;
    }
    currentEnv.pop();

    return retValue;
}

PyObject* Interpreter::visitReturnNode(ReturnNode* node) {
    AstNode* value = node->value;
    PyObject* ret_value = new PyNone();
    if(value) ret_value = value->accept(this);
    throw ReturnException(ret_value);
    return nullptr; // unreachable
}

PyObject* Interpreter::interpret(ProgramNode* node) {
    return node->accept(this);
}

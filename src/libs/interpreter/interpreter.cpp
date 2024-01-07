#include <iomanip>
#include <stdexcept>
#include "./interpreter.hpp"
#include "../token/token.hpp"
#include "../token/tokentype.hpp"
#include "../exceptions/runtime_exceptions.hpp" // break, continue, return
#include "../value/primitives.hpp"
#include "../builtin/builtins.hpp"

void todo() {
    throw std::runtime_error("Feature not implemented yet");
}

Interpreter::Interpreter() {
    
    Scope* builtins = new Scope();
    pushContext(builtins);
    Scope* globals = new Scope(builtins);
    pushContext(globals);
    
    std::string inputRepr("input");    
    builtins->define(inputRepr, new Input());
}

PyObject* Interpreter::interpret(ProgramNode* node) {
    return node->body->accept(this);
}

std::string formatNumber(llf number) {
    
    std::ostringstream oss;
    int intPart = static_cast<int>(number);
    double fracPart = number - intPart;

    if (fracPart == 0.0) {
        oss << intPart << ".0";
    } else {
        oss << number;
    }
    return oss.str();
}

PyObject* Interpreter::visitPrintNode(PrintNode* node) {
    
    PyObject* argValue = nullptr;

    if (node->args != nullptr) {
        argValue = node->args[0].accept(this);
    }
    if (argValue != nullptr) {
        if((*argValue).isStr()) {
            const PyStr* obj = dynamic_cast<const PyStr*>(argValue);
            std::cout << (*obj).getStr();
        } else if((*argValue).isFloat()) {
            const PyFloat* obj = dynamic_cast<const PyFloat*>(argValue);
            std::cout << formatNumber((*obj).getFloat());
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
    PyFunction* value = new PyFunction(node, this->currentContext());
    defineOnContext(fname, value);

    return new PyNone();
}

PyObject* Interpreter::visitClassNode(ClassNode* node) {
    
    Scope* closure = this->currentContext();
    Scope* classEnv = new Scope(closure);
    
    this->pushContext(classEnv);
    node->getBody()->accept(this);
    this->popContext();
    
    const std::string& kname = node->getName();
    PyClass* value = new PyClass(kname, classEnv);

    closure->define(kname, value);
    
    return new PyNone();
}

PyObject* Interpreter::visitPropertyNode(PropertyNode* node) {
    
    PyObject* object = node->object->accept(this);
    NameNode* attr = static_cast<NameNode*>(node->attribute);
    const std::string& name = attr->getLexeme();
    
    if(object->isInstance()) {
    
        PyInstance* instance = static_cast<PyInstance*>(object);
        
        Scope* context = instance->getContext();
        PyObject* value = context->get(name);
        
        if(value->isFunc()) {
            return static_cast<PyFunction*>(value)->bind(instance);
        } else {
            return value;
        }
        
    } else if(object->isFunc()) {
        
        PyFunction* function = static_cast<PyFunction*>(object);
        
        Scope* context = function->getContext();
        return context->get(name);
    }
    
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
     	} catch(BreakException& be) {
     	    break;
     	} catch(ContinueException& ce) {
     	    ; // goes back to evaluate the condition again
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

PyObject* Interpreter::visitPassNode(PassNode* node) {
    return new PyNone();
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
    return nullptr; // unreachable
}

PyObject* Interpreter::visitBinaryOpNode(BinaryOpNode* node)  {

    PyObject* leftValue = node->left->accept(this);
    leftValue->incRefCount();

    PyObject* rightValue = node->right->accept(this);
    rightValue->incRefCount();

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
    leftValue->decRefCount();
    rightValue->decRefCount();

    return value;
}

PyObject* Interpreter::visitAssignNode(AssignNode* node) {
    
    AstNode* targetNode = node->name;
    std::string varName;
    Scope* currCtx = nullptr;
    
    if(targetNode->is_name_node()) {
        NameNode* name = static_cast<NameNode*>(targetNode);
        varName = name->getLexeme();
        currCtx = this->currentContext();
        
    } else if (targetNode->is_property_node()) {
        PropertyNode* propertyNode = static_cast<PropertyNode*>(targetNode);
        PyObject* object = propertyNode->object->accept(this);
        PyInstance* instance = static_cast<PyInstance*>(object);
        NameNode* attribute = static_cast<NameNode*>(propertyNode->attribute);
        varName = attribute->getLexeme();
        currCtx = instance->getContext();
        
    } else {
        throw std::runtime_error("Unsupported target expression");
    }
    
    PyObject* value = node->value->accept(this);
    value->incRefCount();
    
    if(node->op.type == TokenType::Equals) {
        currCtx->define(varName, value);
    } else {
        PyObject* targetValue = currCtx->get(varName);
        targetValue->incRefCount();
        
        switch(node->op.type) {
            case TokenType::PlusEqual:
                value = *targetValue + *value;
                break;
            case TokenType::MinusEqual:
                value = *targetValue - *value;
                break;
            case TokenType::StarEqual:
                value = *targetValue * *value;
                break;
            case TokenType::SlashEqual:
                value = *targetValue / *value;
                break;
            case TokenType::ModEqual:
                value = *targetValue % *value;
                break;
            case TokenType::AndEqual:
                value = *targetValue & *value;
                break;
            case TokenType::OrEqual:
                value = *targetValue | *value;
                break;
            case TokenType::XorEqual:
                value = *targetValue ^ *value;
                break;
            case TokenType::LeftShiftEqual:
                value = *targetValue << *value;
                break;
            case TokenType::RightShiftEqual:
                value = *targetValue >> *value;
                break;
            default:
                throw std::runtime_error("Unsupported assignment operator");
        }
        currCtx->define(varName, value);
        targetValue->decRefCount();
    }
    value->decRefCount();

    return value;
}

PyObject* Interpreter::visitNameNode(NameNode* node){
    const std::string& varname = node->getLexeme();
    return getFromContext(varname);
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

PyObject* Interpreter::visitCallNode(CallNode* expr) {
    
    PyObject* callee = expr->caller->accept(this);
    
    if (!callee->isCallable()) {
        throw std::runtime_error("not a callable object");
    }
    PyCallable* callable = static_cast<PyCallable*>(callee);
    
    std::vector<PyObject*> arguments;
    arguments.reserve(expr->args.size());
    
    for(AstNode* argumentNode : expr->args) {
        arguments.push_back(argumentNode->accept(this));
    }
    return (*callable).call(this, arguments);
}


PyObject* Interpreter::visitReturnNode(ReturnNode* node) {
    
    AstNode* value = node->value;
    PyObject* retValue = value ? value->accept(this) : new PyNone();
    throw ReturnException(retValue);
    return nullptr; // unreachable
}

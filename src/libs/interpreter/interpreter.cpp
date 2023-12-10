#include <iomanip>
#include <stdexcept>
#include "./interpreter.hpp"
#include "../token/token.hpp"
#include "../token/tokentype.hpp"
#include "../exceptions/runtime_exceptions.hpp" // break, continue, return

void todo() {
    throw std::runtime_error("Function not implemented yet");
}

PyObject* Interpreter::visitProgramNode(ProgramNode* node) {
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
    PyFunction* value = new PyFunction(node);
    currentEnv.top()->define(fname, value);

    return new PyNone();
}

PyObject* Interpreter::visitClassNode(ClassNode* node) {

    Scope* parentEnv = currentEnv.top();
    Scope* objEnv = new Scope(parentEnv);

    currentEnv.push(objEnv);
    node->getBody()->accept(this);
    currentEnv.pop();

    const std::string& kname = node->getName();

    PyClass* value = new PyClass(kname, objEnv);

    currentEnv.top()->define(kname, value);

    return new PyNone();
}

PyObject* Interpreter::visitPropertyNode(PropertyNode* node) {

    PyObject* object = node->object->accept(this);

    if(object->isInstance()) {
        PyInstance* obj = static_cast<PyInstance*>(object);
        Scope* scope = obj->getScope();
        currentEnv.push(scope);
        PyObject* attr = node->attribute->accept(this);
        //currentEnv.pop();
        return attr;
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

    AstNode* targetNode = node->name;
    Scope* topEnv;

    std::string varName;

    if(targetNode->is_name_node()) {
        NameNode* name = static_cast<NameNode*>(targetNode);
        varName = name->getLexeme();
        topEnv = currentEnv.top();

    } else if(targetNode->is_property_node()) {
        PropertyNode* ppr = static_cast<PropertyNode*>(targetNode);
        PyInstance* object = static_cast<PyInstance*>(ppr->object->accept(this));
        NameNode* attribute = static_cast<NameNode*>(ppr->attribute);
        varName = attribute->getLexeme();
        topEnv = object->getScope();

    } else {
        throw std::runtime_error("Unsupported target expression");
    }

    PyObject* value = node->value->accept(this);
    value->incRc();

    if(node->op.type == TokenType::Equals) {
        topEnv->define(varName, value);
    } else {
        PyObject* targetValue = topEnv->get(varName);
        targetValue->incRc();

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
        topEnv->define(varName, value);
        targetValue->decRc();
    }
    value->decRc();

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

PyObject* Interpreter::visitCallNode(CallNode* expr)
{

    AstNode* caller = expr->caller;
    PyObject* calleeRef = caller->accept(this);

    if(calleeRef->isKlass()) {

        PyClass* callee = static_cast<PyClass*>(calleeRef);

        Scope* parentEnv = callee->getScope(); // get class's scope
        Scope* objEnv = new Scope(parentEnv);

        PyInstance* self = new PyInstance(objEnv);

        /*Token fname = Token(TokenType::Name, "__init__", 0);
        CallNode* init_target = new CallNode(new NameNode(fname), expr->args);
        init_target->accept(this, self);*/

        PyFunction* constructor = static_cast<PyFunction*>(objEnv->get("__init__"));

        //AstNode* init = constructor->getDecl();
        //CallNode* init_target = new CallNode(init, expr->args);
        //init_target->accept(this, self);

        const std::vector<AstNode*>& params = constructor->getParams();
        const std::vector<AstNode*>& args = expr->args;

        for (size_t i = 0; i < params.size(); i++){
            NameNode* paramNode = static_cast<NameNode*>(params[i]);
            const std::string& argName = paramNode->getLexeme(); // if is NameNode
            if(i == 0) {
                objEnv->define(argName, self);
            } else {
                objEnv->define(argName, args[i-1]->accept(this));
            }
        }
        currentEnv.push(objEnv);
        constructor->getBody()->accept(this);
        currentEnv.pop();

        return self;

    } else if(calleeRef->isFunc()) {



        // check if is builtin function and run it

        if(dynamic_cast<PyFunctionBuiltIn*>(calleeRef))
        {

            std::vector<PyObject *> builtin_args;

            const std::vector<AstNode*>& args = expr->args;

            for(AstNode*  arg : args)
            {
                 builtin_args.push_back(arg->accept(this));
            }

            PyFunctionBuiltIn builtin_func = static_cast<PyFunctionBuiltIn*>(calleeRef)->get_builtin();

            return  maxx(builtin_args);
        }

        // execute  user defined function call

        Scope* parentEnv = currentEnv.top();
        Scope* objEnv = new Scope(parentEnv);
        currentEnv.push(objEnv);

        PyFunction* callee = static_cast<PyFunction*>(calleeRef);

        const std::vector<AstNode*>& params = callee->getParams();
        const std::vector<AstNode*>& args = expr->args;

        PyInstance* bound = (caller->is_property_node() ? new PyInstance(objEnv) : nullptr);

        if((args.size() + (bound ? 1 : 0)) != params.size()) {
            throw std::runtime_error("error on positional args");
        }

        if(bound) {
            for (size_t i = 0; i < params.size(); i++){
                NameNode* paramNode = static_cast<NameNode*>(params[i]);
                const std::string& argName = paramNode->getLexeme(); // if is NameNode

                if(i == 0) {
                    objEnv->define(argName, bound);
                } else {
                    objEnv->define(argName, args[i-1]->accept(this));
                }
            }

        } else {
            for (size_t i = 0; i < args.size(); i++){
                NameNode* paramNode = static_cast<NameNode*>(params[i]);
                const std::string& argName = paramNode->getLexeme(); // if is NameNode
                objEnv->define(argName, args[i]->accept(this));
            }
        }

        PyObject* retValue = nullptr;
        try {
            callee->getBody()->accept(this);
        } catch(ReturnException re) {
            retValue = re.value;
        }
        currentEnv.pop();

        return retValue;

    } else {
        throw std::logic_error("Not a callable.");
    }
    return new PyNone();
}

PyObject* Interpreter::visitReturnNode(ReturnNode* node) {
    AstNode* value = node->value;
    PyObject* retValue = new PyNone();
    if(value) retValue = value->accept(this);
    throw ReturnException(retValue);
    return nullptr; // unreachable
}

PyObject* Interpreter::interpret(ProgramNode* node) {
    return node->accept(this);
}

#include "interpreter.hpp"
#include <stdexcept>

void todo() {
    throw std::logic_error("Function not implemented yet");
}

Value* Interpreter::interpretStmt(ProgramNode* node) {
    
    for (auto statement : node->statements) 
        interpret(statement);
    
    return new Value(1.0);
}

Value* Interpreter::interpret(AstNode* node) {

    if (dynamic_cast<NullNode*>(node) != nullptr) {
        return new Value(); // from an UNDEFINED value
        
    } if (dynamic_cast<NumNode*>(node) != nullptr) {
        return new Value(static_cast<NumNode*>(node)->value);
        
    } else if (dynamic_cast<BooleanNode*>(node) != nullptr) {
        return new Value(static_cast<BooleanNode*>(node)->value);
        
    } else if (dynamic_cast<AssignNode*>(node) != nullptr) {
        
        auto value = interpret(static_cast<AssignNode*>(node)->value);

        auto ident = static_cast<AssignNode*>(node)->name;          

        symbolTable[static_cast<NameNode*>(ident)->name] = value;
       
        return  value;

    } else if (dynamic_cast<BinaryOpNode*>(node) != nullptr) {
        return interpretBinaryOp(static_cast<BinaryOpNode*>(node));

    } else if (dynamic_cast<NameNode*>(node) != nullptr) {
        auto varname = static_cast<NameNode*>(node)->name;
        auto iter = symbolTable.find(varname);

        if(iter != symbolTable.end()) {
            return iter->second;
        } else {
            throw std::runtime_error("Undeclared variable '" + varname + "'");
        }
        
    } else if (dynamic_cast<ProgramNode*>(node) != nullptr) {
        return interpretStmt(static_cast<ProgramNode*>(node));
    
    } else if (dynamic_cast<PrintNode*>(node) != nullptr) {
        return interpretPrintNode(static_cast<PrintNode*>(node));
    
    } else if (dynamic_cast<UnaryOpNode*>(node) != nullptr) {
        return interpretUnaryOp(static_cast<UnaryOpNode*>(node));
    
    } else if (dynamic_cast<CallNode*>(node) != nullptr) {
        return interpretCall(static_cast<CallNode*>(node));
    
    } else {
        throw std::runtime_error("Unsupported node type");
    }
}

Value* Interpreter::interpretPrintNode(PrintNode* node) {
 
    Value* argValue = interpret(node->args);
    
    // invoke the << operator from the Value class
    std::cout << *argValue << "\n" << std::flush;
}

Value* Interpreter::interpretCall(CallNode* node) {
    todo();
}

Value* Interpreter::interpretUnaryOp(UnaryOpNode* node) {
    Value* operandValue = interpret(node->right);

    if(node->op == "-") {
        return  new Value(-operandValue->getFloat());
    } else if(node->op == "not") {
        return new Value(!operandValue->isTruthy());
    } else {
        throw std::runtime_error("Unsupported unary operator");
    }
}

Value* Interpreter::interpretBinaryOp(BinaryOpNode* node) {
    
    Value* leftValue = interpret(node->left);
    Value* rightValue = interpret(node->right);

    if(node->op ==  "+"){
        return new Value(leftValue->getFloat() + rightValue->getFloat());
    
    } else if (node->op ==  "-"){
        return new Value(leftValue->getFloat() - rightValue->getFloat());
    
    } else if (node->op ==  "*"){
        return new Value(leftValue->getFloat() * rightValue->getFloat());
    
    } else if (node->op ==  "/"){
        auto right = rightValue->getFloat();

        if(right == 0) throw std::runtime_error("Attempted to divide by zero");

        return new Value(leftValue->getFloat() / right);

    } else if (node->op == "==") { // TODO: replace with __eq__ call
        
        return new Value(*leftValue == *rightValue);

    } else if(node->op == "!=") { // TODO: replace with __ne__ call
        
        return new Value(!(*leftValue == *rightValue));

    } else if (node->op == "<") { // TODO: replace with __lt__ call
        
        return new Value(*leftValue < *rightValue);

    } else if(node->op == ">") { // TODO: replace with __gt__ call
        
        return new Value(*leftValue > *rightValue);

    } else if (node->op == "<=") { // TODO: replace with __le__ call
        
        return new Value(!(*leftValue > *rightValue));

    } else if(node->op == ">=") { // TODO: replace with __ge__ call
        
        return new Value(!(*leftValue < *rightValue));

    } else if(node->op == "or") { // TODO: replace with __or__ call
        /* 
         *  try to do short-circuit: if after evaluating the left operand,
         *  the result of the logical expression is known, do not evaluate the right operand 
        */
        if(leftValue->isTruthy()) return leftValue;

        return rightValue;

    } else if(node->op == "and") { // TODO: replace with __and__ call
        /* try to do short-circuit: if after evaluating the left operand,
         * the result of the logical expression is known, do not evaluate the right operand 
        */
        if(!(leftValue->isTruthy())) return leftValue;

        return rightValue;

    } else {
        throw std::runtime_error("Unsupported binary operator");
    }
}

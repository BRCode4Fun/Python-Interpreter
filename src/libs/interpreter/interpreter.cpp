#include "interpreter.hpp"
#include <stdexcept>

void todo() {
    throw std::logic_error("Function not implemented yet");
}

Value* Interpreter::visitProgramNode(ProgramNode* node) {
    
    for (auto statement : node->statements) 
        statement->accept(this);
    
    return new Value(1.0);
}

Value* Interpreter::visitPrintNode(PrintNode* node) {
 
    Value* argValue = node->args[0].accept(this);
  
    std::cout << *argValue << "\n" << std::flush;
}

Value* Interpreter::visitNumNode(NumNode* node){
    return new Value(node->value);
}

Value* Interpreter::visitWhileNode(WhileNode* node){    
    
    Value *cond = node->cond->accept(this);

     while(cond->isTruthy()){
           for(auto stmt : *node->stmts){
                stmt->accept(this);
           }
           cond = node->cond->accept(this);
     }     
     return new Value(-1.0); 
}

Value* Interpreter::visitIfNode(IfNode* node){

    Value* cond = node->cond->accept(this);

    if (cond->isTruthy()){
        for(auto stmt : *node->stmts){
            stmt->accept(this); 
        }
    }    
    return new Value(-1.0); 
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
    
    Value* leftValue  = node->left->accept(this);
    Value* rightValue = node->right->accept(this);

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

Value* Interpreter::visitAssignNode(AssignNode* node) {

    auto value = node->value->accept(this);

    symbolTable[static_cast<NameNode*>(node->name)->name] = value;
       
    return  value;
}

Value* Interpreter::visitNameNode(NameNode* node){
        
    auto varname = node->name;

    auto iter = symbolTable.find(varname);

    if(iter != symbolTable.end()) {
        return iter->second;
    } else {
        throw std::runtime_error("Undeclared variable '" + varname + "'");
    }
}

Value* Interpreter::visitBooleanNode(BooleanNode* node){    
    return new Value(node->value);
}

Value* Interpreter::visitUnaryOpNode(UnaryOpNode*  node){

    Value* operandValue = node->right->accept(this);
     
    if(node->op == "-") {
        return  new Value(-operandValue->getFloat());
    } else if(node->op == "not") {
        return new Value(!operandValue->isTruthy());
    } else {
        throw std::runtime_error("Unsupported unary operator");
    }
}

Value* Interpreter::visitNullNode(NullNode* expr){
    return new Value(); 
}

Value* Interpreter::interpret(AstNode* node) {
    return node->accept(this);
}

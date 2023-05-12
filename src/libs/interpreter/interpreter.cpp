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

    if (dynamic_cast<NumNode*>(node) != nullptr) {
        return new Value(static_cast<NumNode*>(node)->value);
        
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
 
    Value *argValue = interpret(node->args);
       
    std::cout << argValue->getFloat() << std::endl;
        
    return new Value(1.0);
}

Value* Interpreter::interpretCall(CallNode* node) {
    todo();
}

Value* Interpreter::interpretUnaryOp(UnaryOpNode* node) {
    Value operandValue = *interpret(node->right);
 
    if(node->op.compare("+")) {
        return  new Value(+operandValue.getFloat());
    } else {
        throw std::runtime_error("Unsupported unary operator");
    }
}

Value* Interpreter::interpretBinaryOp(BinaryOpNode* node) {
    Value *leftValue = interpret(node->left);
    Value *rightValue = interpret(node->right);

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
    } else {
        throw std::runtime_error("Unsupported binary operator");
    }
}

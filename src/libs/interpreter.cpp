#include "interpreter.hpp"

Value* Interpreter::interpretStmt(ProgramNode* node) {
    
    for (auto statement : node->statements) {
        interpret(statement);
    }
    return new Value(4.0);
}

Value* Interpreter::interpret(AstNode* node) {
    if (dynamic_cast<NumNode*>(node) != nullptr) {
        return new  Value(static_cast<NumNode*>(node)->value);
    } else if (dynamic_cast<AssignNode*>(node) != nullptr) {
        
        auto *   value = interpret(static_cast<AssignNode*>(node)->value);
        symbolTable[static_cast<AssignNode*>(node)->name->name] = value;
       // double t = symbolTable[static_cast<AssignNode*>(node)->name->name]->toNumber();
        std::cout << static_cast<AssignNode*>(node)->name->name  << std::endl;
        return new Value(1.0);

    }else if (dynamic_cast<BinaryOpNode*>(node) != nullptr) {
         return interpretBinaryOp(static_cast<BinaryOpNode*>(node));
    }else if (dynamic_cast<NameNode*>(node) != nullptr) {
        

      return symbolTable[static_cast<NameNode*>(node)->name]; 

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
       
    std::cout << argValue->toNumber() << std::endl;
        
    return new Value(1.0);
}

Value* Interpreter::interpretCall(CallNode* node) {
    std::string functionName = static_cast<NameNode*>(node->name)->name;
    if (functionName == "print") {
        if (node->args.size() != 1) {
            throw std::runtime_error("Invalid number of arguments for print function");
        }
        Value argValue = interpret(node->args[0]);
        std::cout << argValue.toString() << std::endl;
        return new Value(1.0);
    } else {
        throw std::runtime_error("Unknown function: " + functionName);
    }
}

Value* Interpreter::interpretUnaryOp(UnaryOpNode* node) {
    Value operandValue = interpret(node->right);
 
    if(node->op.compare("+")) {
        return  new Value(+operandValue.toNumber());
    } else {
        throw std::runtime_error("Unsupported unary operator");
    }
}

Value* Interpreter::interpretBinaryOp(BinaryOpNode* node) {
    Value *leftValue = interpret(node->left);
    Value *rightValue = interpret(node->right);
   
    if(node->op ==  "+"){
        return new Value(leftValue->toNumber() + rightValue->toNumber());
    } else if (node->op ==  "-"){
        return new Value(leftValue->toNumber() - rightValue->toNumber());
    } else{
        throw std::runtime_error("Unsupported binary operator");
    }
}

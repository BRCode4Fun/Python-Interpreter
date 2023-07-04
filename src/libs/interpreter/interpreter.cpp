#include "interpreter.hpp"
#include <stdexcept>

void todo() {
    throw logic_error("Function not implemented yet");
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
        cout << *(argValue);
    }
    cout << "\n" << flush;
    
    return new Value(1.0); 
}


Value* Interpreter::visitIntNode(IntNode* node){
    Value* value = new Value(node->value);
    GC->Add_object(value);
    return value;
}

Value* Interpreter::visitFloatNode(FloatNode* node){
    Value* value = new Value(node->value);
    GC->Add_object(value);
    return value;
}

Value* Interpreter::visitFunctionNode(FunctionNode* node)
{

    Value *value = new Value(node);

    auto func_name = static_cast<NameNode*>(node->getName())->name;

    Global_Environment->define(func_name ,  value);

    return (Value *)nullptr;
}

Value* Interpreter::visitBlockNode(BlockNode* node) {
    for (auto statement : node->statements) {
        statement->accept(this);
    }
    return new Value(1.0);
}

Value* Interpreter::visitWhileNode(WhileNode* node){    
    
    Value* cond = node->cond->accept(this);

     while(cond->isTruthy()){
          node->body->accept(this);
          cond = node->cond->accept(this);
     }     
     return new Value(-1.0); 
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
    leftValue->Increment_Reference_counting(); 
    
    
    Value* rightValue = node->right->accept(this);
    rightValue->Increment_Reference_counting(); 


    
    if(node->op ==  "+"){ // TODO: replace with __add__ call
        Value* value = *leftValue + *rightValue;
        GC->Add_object(value);
        
        leftValue->Decrement_Reference_counting(); 
        rightValue->Decrement_Reference_counting(); 
    
        return value;
    
    } else if (node->op ==  "-"){ // TODO: replace with __sub__ call
         Value* value = *leftValue - *rightValue;
         GC->Add_object(value);
         
         leftValue->Decrement_Reference_counting(); 
         rightValue->Decrement_Reference_counting(); 
        
         return value;
         
    } else if (node->op ==  "*"){ // TODO: replace with __mul__ call
        Value* value = *leftValue * *rightValue;
        GC->Add_object(value); 
        
        leftValue->Decrement_Reference_counting(); 
        rightValue->Decrement_Reference_counting();
         
        return value;
    
    } else if (node->op ==  "/"){ // TODO: replace with __truediv__ call
        Value* value = *leftValue / *rightValue;
        GC->Add_object(value); 
        
        leftValue->Decrement_Reference_counting(); 
        rightValue->Decrement_Reference_counting();
        
        
        return value;

    } else if (node->op ==  "%"){ // TODO: replace with __mod__ call
        Value* value = *leftValue % *rightValue;
        GC->Add_object(value);
        
        leftValue->Decrement_Reference_counting(); 
        rightValue->Decrement_Reference_counting();
        
        return value;

    } else if (node->op == "==") { // TODO: replace with __eq__ call
        Value* value = *leftValue == *rightValue;
        GC->Add_object(value); 
        
        leftValue->Decrement_Reference_counting(); 
        rightValue->Decrement_Reference_counting();
        
        return value;

    } else if(node->op == "!=") { // TODO: replace with __ne__ call
        Value* value = !(*(*leftValue == *rightValue));
        GC->Add_object(value);
        
        leftValue->Decrement_Reference_counting(); 
        rightValue->Decrement_Reference_counting();
        
        
        return value;

    } else if (node->op == "<") { // TODO: replace with __lt__ call
        Value* value = *leftValue < *rightValue;
        GC->Add_object(value); 
        
        leftValue->Decrement_Reference_counting(); 
        rightValue->Decrement_Reference_counting();
        
        
        return value;

    } else if(node->op == ">") { // TODO: replace with __gt__ call
        Value* value = *leftValue > *rightValue;
        GC->Add_object(value); 
        
        leftValue->Decrement_Reference_counting(); 
        rightValue->Decrement_Reference_counting();
        
        
        return value;

    } else if (node->op == "<=") { // TODO: replace with __le__ call
        Value* value = !(*(*leftValue > *rightValue));
        GC->Add_object(value);
        
        leftValue->Decrement_Reference_counting(); 
        rightValue->Decrement_Reference_counting();
        
        
        return value;  
        
    } else if(node->op == ">=") { // TODO: replace with __ge__ call
        Value* value = !(*(*leftValue < *rightValue));
        GC->Add_object(value);
        
        leftValue->Decrement_Reference_counting(); 
        rightValue->Decrement_Reference_counting();
        
        
        return value;

    } else if(node->op == "or") { // TODO: replace with __or__ call
		
        leftValue->Decrement_Reference_counting(); 
        rightValue->Decrement_Reference_counting();
        
        
        return *leftValue || *rightValue;
    } else if(node->op == "and") { // TODO: replace with __and__ call
		
	leftValue->Decrement_Reference_counting(); 
        rightValue->Decrement_Reference_counting();
        
        
        return *leftValue && *rightValue;
    } else {
        throw runtime_error("Unsupported binary operator");
    }
}


Value* Interpreter::visitAssignNode(AssignNode* node) {

    auto value = node->value->accept(this);
    auto var_name = static_cast<NameNode*>(node->name)->name;



    Environment.top()->define(var_name , value);

    

    return value;
}

Value* Interpreter::visitNameNode(NameNode* node)
{
        
    auto varname = node->name;

  //  auto iter = symbolTable.find(varname);

  //  if(iter != symbolTable.end()) {
  //      return iter->second;
  //  } else {

     return Environment.top()->get(varname);
  //      throw runtime_error("Undeclared variable '" + varname + "'");
  //  }
}

Value* Interpreter::visitBooleanNode(BooleanNode* node){       
     Value* value = new Value(node->value);    
     GC->Add_object(value); 
     return value;
}

Value* Interpreter::visitStringNode(StringNode* node){       
     Value* value = new Value(node->lexeme);    
     GC->Add_object(value); 
     return value;
}

Value* Interpreter::visitUnaryOpNode(UnaryOpNode*  node){

    Value* operandValue = node->right->accept(this);
     
    if(node->op == "-") {
        Value* value = -(*operandValue);
        GC->Add_object(value);
        return value;
        
    } else if(node->op == "not") {
        Value* value = !(*operandValue);
        GC->Add_object(value); 
        return value;
    } else {
        throw runtime_error("Unsupported unary operator");
    }
}

Value* Interpreter::visitNullNode(NullNode* expr){
    Value* value = new Value();  
    GC->Add_object(value);
    return value;
}

Value* Interpreter::interpret(ProgramNode* node) {
    
    return node->accept(this);
}


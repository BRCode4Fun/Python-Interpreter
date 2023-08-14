#pragma once 

#include "../ast/ast.hpp"

class FuncObj {

public:
    FuncObj(FunctionNode* function) : function_node(function) {}  
    
    FunctionNode* function_node;
};

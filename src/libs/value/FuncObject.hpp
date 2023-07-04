
    #pragma once 
    #include "../ast/ast.hpp"

    class FuncObj 
    {
    public:
      
        FunctionNode *function_node;
       
        FuncObj(FunctionNode *function) :   function_node(function_node) {}  
    };
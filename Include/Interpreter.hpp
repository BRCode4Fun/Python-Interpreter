#ifndef INTERPRETER_HPP 
#define  INTERPRETER_HPP

#include <cctype>
#include <stdexcept>
#include <unordered_map>
#include "Value.hpp"
#include "Ast.hpp"



class Interpreter {
public:
        Interpreter() {}
        Value * interpretUnaryOp(UnaryOpNode* node);
        Value * interpretBinaryOp(BinaryOpNode* node);
        Value * interpretCall(CallNode* node);
        Value * interpret(AstNode* node);
        Value * interpretPrintNode(PrintNode* node);
        Value * interpretStmt(ProgramNode* node);
        void interpret(ProgramNode* node);


        unordered_map<string  , Value *> symbolTable;

 
};

#endif
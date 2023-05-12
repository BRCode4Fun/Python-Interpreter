#pragma once

#include <unordered_map>
#include "../value/value.hpp"
#include "../ast/ast.hpp"

class Interpreter {
    public:
        Interpreter() {}
        Value* interpretUnaryOp(UnaryOpNode* node);
        Value* interpretBinaryOp(BinaryOpNode* node);
        Value* interpretCall(CallNode* node);
        Value* interpret(AstNode* node);
        Value* interpretPrintNode(PrintNode* node);
        Value* interpretStmt(ProgramNode* node);
        Value* interpret(ProgramNode* node);
    private:
        unordered_map<string, Value*> symbolTable;
};

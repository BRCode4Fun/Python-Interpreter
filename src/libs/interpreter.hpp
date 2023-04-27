#pragma once

#include <cctype>
#include <stdexcept>
#include <unordered_map>
#include "value.hpp"
#include "ast.hpp"

class Interpreter {
    public:
        Interpreter() {}
        Value* interpretUnaryOp(UnaryOpNode* node);
        Value* interpretBinaryOp(BinaryOpNode* node);
        Value* interpretCall(CallNode* node);
        Value* interpret(AstNode* node);
        Value* interpretPrintNode(PrintNode* node);
        Value* interpretStmt(ProgramNode* node);
        void interpret(ProgramNode* node);
    private:
        unordered_map<string, Value*> symbolTable;
};

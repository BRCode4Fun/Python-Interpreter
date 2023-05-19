#pragma once

#include <unordered_map>
#include "../value/value.hpp"
#include "../ast/ast.hpp"

class Interpreter : public NodeVisitor {
    public:
        Interpreter() {}
        Value* interpretUnaryOp(UnaryOpNode* node);
        Value* interpretBinaryOp(BinaryOpNode* node);
        Value* interpretCall(CallNode* node);
        Value* interpret(AstNode* node);
        Value* interpretPrintNode(PrintNode* node);
        Value* interpretStmt(ProgramNode* node);
        Value* interpret(ProgramNode* node);


        virtual Value * visitPrintNode( PrintNode* node) override;
        virtual Value * visitProgramNode( ProgramNode* node) override;
        virtual Value * visitBinaryOpNode( BinaryOpNode* node) override;
        virtual Value * visitNumNode( NumNode* node)  override;
        virtual Value * visitNameNode( NameNode* node) override;
        virtual Value * visitBooleanNode(BooleanNode* node) override;
        virtual Value * visitUnaryOpNode(UnaryOpNode* node)  override;
        virtual Value * visitNullNode(NullNode* expr) override;
    //  virtual Value * visitCallNode(const CallNode*  expr) override;
        virtual Value * visitAssignNode(AssignNode*  node)  override;
    private:
        unordered_map<string, Value*> symbolTable;
};

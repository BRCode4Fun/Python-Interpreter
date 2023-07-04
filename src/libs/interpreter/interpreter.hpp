#pragma once

#include <stack>
#include <unordered_map>
#include "../value/value.hpp"
#include "../ast/ast.hpp"
#include "../gc/gc.hpp"
#include "./scope.hpp"

using namespace std;

class Interpreter : public NodeVisitor {
   
    public:

        Interpreter() {

            GC = new GarbageCollector();

            if(!GC) throw runtime_error("Error while allocating garbage collector");


       
          
            
            Global_Environment = new Scope();
            Environment.push(new Scope());

            





        }
        
        Value* interpret(ProgramNode* node);

        virtual Value* visitProgramNode( ProgramNode* node) override;
        virtual Value* visitBlockNode( BlockNode* node) override;
        virtual Value* visitPrintNode( PrintNode* node) override;
        virtual Value* visitWhileNode(WhileNode* node) override;
        virtual Value* visitIfNode(IfNode* node) override;
        virtual Value* visitAssignNode(AssignNode* node)  override;
        virtual Value* visitTernaryOpNode( TernaryOpNode* node) override;
        virtual Value* visitBinaryOpNode( BinaryOpNode* node) override;
        virtual Value* visitUnaryOpNode(UnaryOpNode* node)  override;
        virtual Value* visitIntNode( IntNode* node)  override;
        virtual Value* visitFloatNode( FloatNode* node)  override;
        virtual Value* visitNameNode( NameNode* node) override;
        virtual Value* visitStringNode( StringNode* node) override;
        virtual Value* visitBooleanNode(BooleanNode* node) override;
        virtual Value* visitNullNode(NullNode* expr) override;
        virtual Value* visitFunctionNode(FunctionNode* node) override;
        virtual Value* visitCallNode(CallNode* node) override;

    //  virtual Value * visitCallNode(const CallNode*  expr) override;
    private:
        unordered_map<string, Value*> symbolTable;

        GarbageCollector* GC = nullptr; 

       // unsigned int num_objects_allocated = 0;

       Scope * Global_Environment;
       stack<Scope *> Environment;
       
};

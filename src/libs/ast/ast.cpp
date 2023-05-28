#include "ast.hpp"

Value * AssignNode::accept(NodeVisitor * visitor) {
      return  visitor->visitAssignNode(this);
}

Value * CallNode::accept(NodeVisitor * visitor) {
     //   return  visitor->visitNumNode(this);
      return (Value *)0; 
}

Value * BinaryOpNode::accept(NodeVisitor * visitor) {
        return  visitor->visitBinaryOpNode(this);
       
}

Value * NameNode::accept(NodeVisitor * visitor) {
       return  visitor->visitNameNode(this);
}

Value * BooleanNode::accept(NodeVisitor * visitor) {
       return  visitor->visitBooleanNode(this);
}

Value * UnaryOpNode::accept(NodeVisitor * visitor) {
       return  visitor->visitUnaryOpNode(this);
}

Value * NumNode::accept(NodeVisitor * visitor) {
   
       return  visitor->visitNumNode(this);
}

Value * ProgramNode::accept(NodeVisitor * visitor) {
     return  visitor->visitProgramNode(this);
}

Value * PrintNode::accept(NodeVisitor * visitor) {
    return  visitor->visitPrintNode(this);
}

Value * NullNode::accept(NodeVisitor * visitor) {
    return  visitor->visitNullNode(this);
}

 Value * WhileNode::accept(NodeVisitor* visitor) {
     return  visitor->visitWhileNode(this);
 }

 Value * IfNode::accept(NodeVisitor* visitor) {
     return  visitor->visitIfNode(this);
 }
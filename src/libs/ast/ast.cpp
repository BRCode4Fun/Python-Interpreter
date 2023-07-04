#include "ast.hpp"

Value* AssignNode::accept(NodeVisitor* visitor) {
    return visitor->visitAssignNode(this);
}

Value* CallNode::accept(NodeVisitor* visitor) {
    return  visitor->visitCallNode(this);
    return (Value*)0; 
}

Value* BinaryOpNode::accept(NodeVisitor* visitor) {
    return visitor->visitBinaryOpNode(this);  
}

Value* TernaryOpNode::accept(NodeVisitor* visitor) {
    return visitor->visitTernaryOpNode(this);  
}

Value* NameNode::accept(NodeVisitor* visitor) {
    return visitor->visitNameNode(this);
}

Value* StringNode::accept(NodeVisitor* visitor) {
    return visitor->visitStringNode(this);
}

Value* BooleanNode::accept(NodeVisitor* visitor) {
    return visitor->visitBooleanNode(this);
}

Value* UnaryOpNode::accept(NodeVisitor* visitor) {
    return visitor->visitUnaryOpNode(this);
}

Value* IntNode::accept(NodeVisitor* visitor) {
    return visitor->visitIntNode(this);
}

Value* FloatNode::accept(NodeVisitor* visitor) {
    return visitor->visitFloatNode(this);
}

Value* ProgramNode::accept(NodeVisitor* visitor) {
    return visitor->visitProgramNode(this);
}

Value* PrintNode::accept(NodeVisitor* visitor) {
    return visitor->visitPrintNode(this);
}

Value* NullNode::accept(NodeVisitor* visitor) {
    return visitor->visitNullNode(this);
}

Value* BlockNode::accept(NodeVisitor* visitor) {
    return  visitor->visitBlockNode(this);
}

Value* WhileNode::accept(NodeVisitor* visitor) {
    return visitor->visitWhileNode(this);
}

Value* IfNode::accept(NodeVisitor* visitor) {
    return visitor->visitIfNode(this);
}

Value* FunctionNode::accept(NodeVisitor* visitor) {
    return visitor->visitFunctionNode(this);
}

#include "ast.hpp"

PyObject* AssignNode::accept(NodeVisitor* visitor) {
    return visitor->visitAssignNode(this);
}

PyObject* CallNode::accept(NodeVisitor* visitor) {
    return visitor->visitCallNode(this);
}

PyObject* BinaryOpNode::accept(NodeVisitor* visitor) {
    return visitor->visitBinaryOpNode(this);  
}

PyObject* TernaryOpNode::accept(NodeVisitor* visitor) {
    return visitor->visitTernaryOpNode(this);  
}

PyObject* NameNode::accept(NodeVisitor* visitor) {
    return visitor->visitNameNode(this);
}

PyObject* StringNode::accept(NodeVisitor* visitor) {
    return visitor->visitStringNode(this);
}

PyObject* BooleanNode::accept(NodeVisitor* visitor) {
    return visitor->visitBooleanNode(this);
}

PyObject* UnaryOpNode::accept(NodeVisitor* visitor) {
    return visitor->visitUnaryOpNode(this);
}

PyObject* IntNode::accept(NodeVisitor* visitor) {
    return visitor->visitIntNode(this);
}

PyObject* FloatNode::accept(NodeVisitor* visitor) {
    return visitor->visitFloatNode(this);
}

PyObject* ProgramNode::accept(NodeVisitor* visitor) {
    return visitor->visitProgramNode(this);
}

PyObject* PrintNode::accept(NodeVisitor* visitor) {
    return visitor->visitPrintNode(this);
}

PyObject* NullNode::accept(NodeVisitor* visitor) {
    return visitor->visitNullNode(this);
}

PyObject* BlockNode::accept(NodeVisitor* visitor) {
    return  visitor->visitBlockNode(this);
}

PyObject* WhileNode::accept(NodeVisitor* visitor) {
    return visitor->visitWhileNode(this);
}

PyObject* BreakNode::accept(NodeVisitor* visitor) {
    return visitor->visitBreakNode(this);
}

PyObject* ContinueNode::accept(NodeVisitor* visitor) {
    return visitor->visitContinueNode(this);
}

PyObject* PassNode::accept(NodeVisitor* visitor) {
    return visitor->visitPassNode(this);
}

PyObject* IfNode::accept(NodeVisitor* visitor) {
    return visitor->visitIfNode(this);
}

PyObject* FunctionNode::accept(NodeVisitor* visitor) {
    return visitor->visitFunctionNode(this);
}

PyObject* ClassNode::accept(NodeVisitor* visitor) {
    return visitor->visitClassNode(this);
}

PyObject* PropertyNode::accept(NodeVisitor* visitor) {
    return visitor->visitPropertyNode(this);
}

PyObject* ReturnNode::accept(NodeVisitor* visitor) {
    return visitor->visitReturnNode(this);
}


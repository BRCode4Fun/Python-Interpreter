#include "ast.hpp"

PyObject* AssignNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "AssignNode" << "\n";
    #endif
    return visitor->visitAssignNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "AssignNode" << "\n";
    #endif
}

PyObject* CallNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "CallNode" << "\n";
    #endif
    return visitor->visitCallNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "CallNode" << "\n";
    #endif
}

PyObject* BinaryOpNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "BinaryOpNode" << "\n";
    #endif
    return visitor->visitBinaryOpNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "BinaryOpNode" << "\n";
    #endif  
}

PyObject* TernaryOpNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "TernaryOpNode" << "\n";
    #endif
    return visitor->visitTernaryOpNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "TernaryOpNode" << "\n";
    #endif  
}

PyObject* NameNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "NameNode" << "\n";
    #endif
    return visitor->visitNameNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "NameNode" << "\n";
    #endif
}

PyObject* StringNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "StringNode" << "\n";
    #endif
    return visitor->visitStringNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "StringNode" << "\n";
    #endif
}

PyObject* BooleanNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "BooleanNode" << "\n";
    #endif
    return visitor->visitBooleanNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "BooleanNode" << "\n";
    #endif
}

PyObject* UnaryOpNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "UnaryOpNode" << "\n";
    #endif
    return visitor->visitUnaryOpNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "UnaryOpNode" << "\n";
    #endif
}

PyObject* IntNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "IntNode" << "\n";
    #endif
    return visitor->visitIntNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "IntNode" << "\n";
    #endif
}

PyObject* FloatNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "FloatNode" << "\n";
    #endif
    return visitor->visitFloatNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "FloatNode" << "\n";
    #endif
}

PyObject* ProgramNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "ProgramNode" << "\n";
    #endif
    return visitor->visitProgramNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "ProgramNode" << "\n";
    #endif
}

PyObject* PrintNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "PrintNode" << "\n";
    #endif
    return visitor->visitPrintNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "PrintNode" << "\n";
    #endif
}

PyObject* NullNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "NullNode" << "\n";
    #endif
    return visitor->visitNullNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "NullNode" << "\n";
    #endif
}

PyObject* BlockNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "BlockNode" << "\n";
    #endif
    return  visitor->visitBlockNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "BlockNode" << "\n";
    #endif
}

PyObject* WhileNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "WhileNode" << "\n";
    #endif
    return visitor->visitWhileNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "WhileNode" << "\n";
    #endif
}

PyObject* BreakNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "BreakNode" << "\n";
    #endif
    return visitor->visitBreakNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "BlockNode" << "\n";
    #endif
}

PyObject* ContinueNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "ContinueNode" << "\n";
    #endif
    return visitor->visitContinueNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "ContinueNode" << "\n";
    #endif
}

PyObject* PassNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "PassNode" << "\n";
    #endif
    return visitor->visitPassNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "PassNode" << "\n";
    #endif
}

PyObject* IfNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "IfNode" << "\n";
    #endif
    return visitor->visitIfNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "IfNode" << "\n";
    #endif
}

PyObject* FunctionNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "FunctionNode" << "\n";
    #endif
    return visitor->visitFunctionNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "FunctionNode" << "\n";
    #endif
}

PyObject* ClassNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "ClassNode" << "\n";
    #endif
    return visitor->visitClassNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "ClassNode" << "\n";
    #endif
}

PyObject* PropertyNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "PropertyNode" << "\n";
    #endif
    return visitor->visitPropertyNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "PropertyNode" << "\n";
    #endif
}

PyObject* ReturnNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "ReturnNode" << "\n";
    #endif
    return visitor->visitReturnNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "ReturnNode" << "\n";
    #endif
}


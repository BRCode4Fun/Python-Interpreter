#include "ast.hpp"

PyObject* AssignNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "AssignNode" << "\n";
    #endif
    PyObject* value = visitor->visitAssignNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "AssignNode" << "\n";
    #endif
    return value;
}

PyObject* CallNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "CallNode" << "\n";
    #endif
    PyObject* value = visitor->visitCallNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "CallNode" << "\n";
    #endif
    return value;
}

PyObject* BinaryOpNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "BinaryOpNode" << "\n";
    #endif
    PyObject* value = visitor->visitBinaryOpNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "BinaryOpNode" << "\n";
    #endif
    return value;
}

PyObject* TernaryOpNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "TernaryOpNode" << "\n";
    #endif
    PyObject* value = visitor->visitTernaryOpNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "TernaryOpNode" << "\n";
    #endif
    return value;
}

PyObject* NameNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "NameNode" << "\n";
    #endif
    PyObject* value = visitor->visitNameNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "NameNode" << "\n";
    #endif
    return value;
}

PyObject* StringNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "StringNode" << "\n";
    #endif
    PyObject* value = visitor->visitStringNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "StringNode" << "\n";
    #endif
    return value;
}

PyObject* BooleanNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "BooleanNode" << "\n";
    #endif
    PyObject* value = visitor->visitBooleanNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "BooleanNode" << "\n";
    #endif
    return value;
}

PyObject* UnaryOpNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "UnaryOpNode" << "\n";
    #endif
    PyObject* value = visitor->visitUnaryOpNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "UnaryOpNode" << "\n";
    #endif
    return value;
}

PyObject* IntNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "IntNode" << "\n";
    #endif
    PyObject* value = visitor->visitIntNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "IntNode" << "\n";
    #endif
    return value;
}

PyObject* FloatNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "FloatNode" << "\n";
    #endif
    PyObject* value = visitor->visitFloatNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "FloatNode" << "\n";
    #endif
    return value;
}

PyObject* ProgramNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "ProgramNode" << "\n";
    #endif
    PyObject* value = visitor->visitProgramNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "ProgramNode" << "\n";
    #endif
    return value;
}

PyObject* PrintNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "PrintNode" << "\n";
    #endif
    PyObject* value = visitor->visitPrintNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "PrintNode" << "\n";
    #endif
    return value;
}

PyObject* NullNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "NullNode" << "\n";
    #endif
    PyObject* value = visitor->visitNullNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "NullNode" << "\n";
    #endif
    return value;
}

PyObject* BlockNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "BlockNode" << "\n";
    #endif
    PyObject* value = visitor->visitBlockNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "BlockNode" << "\n";
    #endif
    return value;
}

PyObject* WhileNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "WhileNode" << "\n";
    #endif
    PyObject* value = visitor->visitWhileNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "WhileNode" << "\n";
    #endif
    return value;
}

PyObject* BreakNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "BreakNode" << "\n";
    #endif
    PyObject* value = visitor->visitBreakNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "BlockNode" << "\n";
    #endif
    return value;
}

PyObject* ContinueNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "ContinueNode" << "\n";
    #endif
    PyObject* value = visitor->visitContinueNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "ContinueNode" << "\n";
    #endif
    return value;
}

PyObject* PassNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "PassNode" << "\n";
    #endif
    PyObject* value = visitor->visitPassNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "PassNode" << "\n";
    #endif
    return value;
}

PyObject* IfNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "IfNode" << "\n";
    #endif
    PyObject* value = visitor->visitIfNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "IfNode" << "\n";
    #endif
    return value;
}

PyObject* FunctionNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "FunctionNode" << "\n";
    #endif
    PyObject* value = visitor->visitFunctionNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "FunctionNode" << "\n";
    #endif
    return value;
}

PyObject* ClassNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "ClassNode" << "\n";
    #endif
    PyObject* value = visitor->visitClassNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "ClassNode" << "\n";
    #endif
    return value;
}

PyObject* PropertyNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "PropertyNode" << "\n";
    #endif
    PyObject* value = visitor->visitPropertyNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "PropertyNode" << "\n";
    #endif
    return value;
}

PyObject* ReturnNode::accept(NodeVisitor* visitor) {
    #ifdef DEBUG
        std::cout << "visiting " << "ReturnNode" << "\n";
    #endif
    PyObject* value = visitor->visitReturnNode(this);
    #ifdef DEBUG
        std::cout << "exiting " << "ReturnNode" << "\n";
    #endif
    return value;
}


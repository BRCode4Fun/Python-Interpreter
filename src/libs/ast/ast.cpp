#include <iostream>
#include "ast.hpp"

ProgramNode* AstNode::unwrap_program_node() {
    if (type == AstNodeType::Program) {
        return dynamic_cast<ProgramNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

BlockNode* AstNode::unwrap_block_node() {
    if (type == AstNodeType::Block) {
        return dynamic_cast<BlockNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

TernaryOpNode* AstNode::unwrap_ternary_op_node() {
    if (type == AstNodeType::TernaryOp) {
        return dynamic_cast<TernaryOpNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

BinaryOpNode* AstNode::unwrap_binary_op_node() {
    if (type == AstNodeType::BinaryOp) {
        return dynamic_cast<BinaryOpNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

UnaryOpNode* AstNode::unwrap_unary_op_node() {
    if (type == AstNodeType::UnaryOp) {
        return dynamic_cast<UnaryOpNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

AssignNode* AstNode::unwrap_assign_node() {
    if (type == AstNodeType::Assign) {
        return dynamic_cast<AssignNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

IntNode* AstNode::unwrap_int_node() {
    if (type == AstNodeType::Int) {
        return dynamic_cast<IntNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

BooleanNode* AstNode::unwrap_bool_node() {
    if (type == AstNodeType::Boolean) {
        return dynamic_cast<BooleanNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

FloatNode* AstNode::unwrap_float_node() {
    if (type == AstNodeType::Float) {
        return dynamic_cast<FloatNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

StringNode* AstNode::unwrap_string_node() {
    if (type == AstNodeType::String) {
        return dynamic_cast<StringNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

NullNode* AstNode::unwrap_null_node() {
    if (type == AstNodeType::Null) {
        return dynamic_cast<NullNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

NameNode* AstNode::unwrap_name_node() {
    if (type == AstNodeType::Name) {
        return dynamic_cast<NameNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

IfNode* AstNode::unwrap_cond_node() {
    if (type == AstNodeType::If) {
        return dynamic_cast<IfNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

WhileNode* AstNode::unwrap_rep_node() {
    if (type == AstNodeType::While) {
        return dynamic_cast<WhileNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

BreakNode* AstNode::unwrap_break_node() {
    if (type == AstNodeType::Break) {
        return dynamic_cast<BreakNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

ContinueNode* AstNode::unwrap_continue_node() {
    if (type == AstNodeType::Continue) {
        return dynamic_cast<ContinueNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

PassNode* AstNode::unwrap_pass_node() {
    if (type == AstNodeType::Pass) {
        return dynamic_cast<PassNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

CallNode* AstNode::unwrap_call_node() {
    if (type == AstNodeType::Call) {
        return dynamic_cast<CallNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

FunctionNode* AstNode::unwrap_function_node() {
    if (type == AstNodeType::Function) {
        return dynamic_cast<FunctionNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

ReturnNode* AstNode::unwrap_ret_node() {
    if (type == AstNodeType::Return) {
        return dynamic_cast<ReturnNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

ClassNode* AstNode::unwrap_class_node() {
    if (type == AstNodeType::Class) {
        return dynamic_cast<ClassNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

PropertyNode* AstNode::unwrap_property_node() {
    if (type == AstNodeType::AttrRef) {
        return dynamic_cast<PropertyNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

PrintNode* AstNode::unwrap_print_node() {
    if (type == AstNodeType::Print) {
        return dynamic_cast<PrintNode*>(this);
    } else {
        throw std::runtime_error("Failed to unwrap AstNode.");
    }
}

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


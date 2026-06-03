#pragma once

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include "../token/token.hpp"

using llf = long double;
using lld = long long int;

class PyObject;
class NodeVisitor;

// Define the AST node types
enum class AstNodeType {
    // Root
    Program,

    // Group Statements
    Block,

    // Statements
    Print, While, Break, Continue,
    If, Function, Return,
    Class, AttrRef,
    Pass,

    // Expressions
    UnaryOp, BinaryOp, TernaryOp,
    Assign, Call,

    // Literals
    Name, String,
    Int, Float, Boolean, Null
};

class AstNode {

public:

    AstNode(AstNodeType type) : type(type) {}

    virtual ~AstNode() = default;

    bool is_name_node() { return type == AstNodeType::Name; }
    bool is_property_node() { return type == AstNodeType::AttrRef; }
    bool is_block_node() { return type == AstNodeType::Block; }
    bool is_function_node() { return type == AstNodeType::Function; }

    virtual PyObject* accept(NodeVisitor* visitor) = 0;

    class ProgramNode*   unwrap_program_node();
    class BlockNode*     unwrap_block_node();
    class TernaryOpNode* unwrap_ternary_op_node();
    class BinaryOpNode*  unwrap_binary_op_node();
    class UnaryOpNode*   unwrap_unary_op_node();
    class AssignNode*    unwrap_assign_node();
    class IntNode*       unwrap_int_node();
    class BooleanNode*   unwrap_bool_node();
    class FloatNode*     unwrap_float_node();
    class StringNode*    unwrap_string_node();
    class NullNode*      unwrap_null_node();
    class NameNode*      unwrap_name_node();
    class IfNode*        unwrap_cond_node();
    class WhileNode*     unwrap_rep_node();
    class BreakNode*     unwrap_break_node();
    class ContinueNode*  unwrap_continue_node();
    class PassNode*      unwrap_pass_node();
    class CallNode*      unwrap_call_node();
    class FunctionNode*  unwrap_function_node();
    class ReturnNode*    unwrap_ret_node();
    class ClassNode*     unwrap_class_node();
    class PropertyNode*  unwrap_property_node();
    class PrintNode*     unwrap_print_node();

    AstNodeType type;
};

class PrintNode : public AstNode {

public:

    explicit PrintNode(std::unique_ptr<AstNode> args)
      : AstNode(AstNodeType::Print), args(std::move(args)) {}

    std::unique_ptr<AstNode> args;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class BlockNode : public AstNode {

public:

    explicit BlockNode(std::vector<std::unique_ptr<AstNode>> statements_in)
      : AstNode(AstNodeType::Block), statements(std::move(statements_in)) {}

    std::vector<std::unique_ptr<AstNode>> statements;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class ProgramNode : public AstNode {

public:

    explicit ProgramNode(std::unique_ptr<BlockNode> body_in)
      : AstNode(AstNodeType::Program), body(std::move(body_in)) {}

    std::unique_ptr<BlockNode> body;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class TernaryOpNode : public AstNode {

public:

    TernaryOpNode(std::unique_ptr<AstNode> condition, std::unique_ptr<AstNode> left_in, std::unique_ptr<AstNode> right_in)
      : AstNode(AstNodeType::TernaryOp),
        cond(std::move(condition)), left(std::move(left_in)), right(std::move(right_in)) {}

    std::unique_ptr<AstNode> cond, left, right;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class BinaryOpNode : public AstNode {

public:

    BinaryOpNode(std::unique_ptr<AstNode> left_in, Token op, std::unique_ptr<AstNode> right_in)
      : AstNode(AstNodeType::BinaryOp),
        left(std::move(left_in)), op(op), right(std::move(right_in)) {}

    Token op;
    std::unique_ptr<AstNode> left, right;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class UnaryOpNode : public AstNode {

public:

    UnaryOpNode(Token op, std::unique_ptr<AstNode> right_in)
      : AstNode(AstNodeType::UnaryOp),
        op(op), right(std::move(right_in)) {}

    Token op;
    std::unique_ptr<AstNode> right;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class IntNode : public AstNode {

public:

    explicit IntNode(Token value)
      : AstNode(AstNodeType::Int), value(value) {}

    Token value; // TODO: implement the BigInt class

    const std::string& get_lexeme() { return value.lexeme; }

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class FloatNode : public AstNode {

public:

    explicit FloatNode(Token value)
      : AstNode(AstNodeType::Float), value(value) {}

    Token value;

    const std::string& get_lexeme() { return value.lexeme; }

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class NameNode : public AstNode {

public:

    explicit NameNode(Token name)
      : AstNode(AstNodeType::Name), value(name) {}

    Token value;

    const std::string& get_lexeme() { return value.lexeme; }

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class StringNode : public AstNode {

public:

    explicit StringNode(Token value)
      : AstNode(AstNodeType::String), value(value) {}

    Token value;

    const std::string& get_lexeme() { return value.lexeme; }

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class BooleanNode : public AstNode {

public:

    explicit BooleanNode(bool value)
      : AstNode(AstNodeType::Boolean), value(value) {}

    bool value;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class ReturnNode : public AstNode {

public:

    ReturnNode(Token keyword, std::unique_ptr<AstNode> value_in)
      : AstNode(AstNodeType::Return), kwd(keyword), value(std::move(value_in)) {}

    Token kwd;
    std::unique_ptr<AstNode> value;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class NullNode : public AstNode {

public:

    explicit NullNode(Token none) : AstNode(AstNodeType::Null), kwd(none) {}

    Token kwd;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class CallNode : public AstNode {

public:

    CallNode(std::unique_ptr<AstNode> caller_in, std::vector<std::unique_ptr<AstNode>> args_in)
      : AstNode(AstNodeType::Call), caller(std::move(caller_in)), args(std::move(args_in)) {}

    std::unique_ptr<AstNode> caller;
    std::vector<std::unique_ptr<AstNode>> args;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class AssignNode : public AstNode {
public:
    AssignNode(std::unique_ptr<AstNode> name_in, std::unique_ptr<AstNode> value_in, Token op)
      : AstNode(AstNodeType::Assign), name(std::move(name_in)), value(std::move(value_in)), op(op) {}

    std::unique_ptr<AstNode> name, value;
    Token op;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class WhileNode : public AstNode {
public:

    WhileNode(std::unique_ptr<AstNode> cond_in, std::unique_ptr<AstNode> body_in)
      : AstNode(AstNodeType::While), cond(std::move(cond_in)), body(std::move(body_in)) {}

    std::unique_ptr<AstNode> cond, body;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class BreakNode : public AstNode {

public:

    explicit BreakNode(Token keyword)
      : AstNode(AstNodeType::Break), kwd(keyword) {}

    Token kwd;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class ContinueNode : public AstNode {

public:

    explicit ContinueNode(Token keyword)
      : AstNode(AstNodeType::Continue), kwd(keyword) {}

    Token kwd;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class PassNode : public AstNode {

public:

    explicit PassNode(Token keyword)
      : AstNode(AstNodeType::Pass), kwd(keyword) {}

    Token kwd;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class FunctionNode : public AstNode {

public:

    FunctionNode(Token fname, std::vector<std::unique_ptr<AstNode>> params_in, std::unique_ptr<AstNode> body_in)
        : AstNode(AstNodeType::Function),
          fname(fname), params(std::move(params_in)), body(std::move(body_in)) {}

    const std::string& get_name() const { return fname.lexeme; };
    const std::vector<std::unique_ptr<AstNode>>& get_params() const { return params; }
    AstNode* get_body() const { return body.get(); }

    virtual PyObject* accept(NodeVisitor* visitor) override;

private:
    Token fname;
    std::vector<std::unique_ptr<AstNode>> params;
    std::unique_ptr<AstNode> body;
};

class ClassNode : public AstNode {

public:

    ClassNode(Token kname, std::unique_ptr<AstNode> body_in)
        : AstNode(AstNodeType::Class), kname(kname), body(std::move(body_in)) {}

    const std::string& get_name() const { return kname.lexeme; }
    AstNode* get_body() const { return body.get(); }

    virtual PyObject* accept(NodeVisitor* visitor) override;

private:
    Token kname;
    std::unique_ptr<AstNode> body;
};

class PropertyNode : public AstNode {

public:

    PropertyNode(std::unique_ptr<AstNode> object_in, std::unique_ptr<AstNode> attr_in)
        : AstNode(AstNodeType::AttrRef), object(std::move(object_in)), attribute(std::move(attr_in)) {}

    virtual PyObject* accept(NodeVisitor* visitor) override;

    std::unique_ptr<AstNode> object, attribute;
};

class IfNode : public AstNode {

public:

    IfNode(std::unique_ptr<AstNode> cond_in, std::unique_ptr<AstNode> true_branch_in,
           std::vector<std::pair<std::unique_ptr<AstNode>, std::unique_ptr<AstNode>>> elif_branches_in,
           std::unique_ptr<AstNode> else_branch_in)
        : AstNode(AstNodeType::If), cond(std::move(cond_in)),
          true_branch(std::move(true_branch_in)),
          elif_branches(std::move(elif_branches_in)),
          else_branch(std::move(else_branch_in)) {}

    std::unique_ptr<AstNode> cond, true_branch, else_branch;
    std::vector<std::pair<std::unique_ptr<AstNode>, std::unique_ptr<AstNode>>> elif_branches;

    virtual PyObject* accept(NodeVisitor* visitor) override;
};

class NodeVisitor {

public:

    ~NodeVisitor() = default;

    virtual PyObject* visit_program_node(ProgramNode* node) = 0;
    virtual PyObject* visit_block_node(BlockNode* node) = 0;
    virtual PyObject* visit_print_node(PrintNode* node) = 0;
    virtual PyObject* visit_while_node(WhileNode* node) = 0;
    virtual PyObject* visit_break_node(BreakNode* node) = 0;
    virtual PyObject* visit_continue_node(ContinueNode* node) = 0;
    virtual PyObject* visit_pass_node(PassNode* node) = 0;
    virtual PyObject* visit_if_node(IfNode* node) = 0;
    virtual PyObject* visit_assign_node(AssignNode* node) = 0;
    virtual PyObject* visit_ternary_op_node(TernaryOpNode* node) = 0;
    virtual PyObject* visit_binary_op_node(BinaryOpNode* node) = 0;
    virtual PyObject* visit_unary_op_node(UnaryOpNode* node) = 0;
    virtual PyObject* visit_int_node(IntNode* node) = 0;
    virtual PyObject* visit_float_node(FloatNode* node) = 0;
    virtual PyObject* visit_name_node(NameNode* node) = 0;
    virtual PyObject* visit_string_node(StringNode* node) = 0;
    virtual PyObject* visit_boolean_node(BooleanNode* node) = 0;
    virtual PyObject* visit_null_node(NullNode* node) = 0;
    virtual PyObject* visit_function_node(FunctionNode* node) = 0;
    virtual PyObject* visit_call_node(CallNode* node) = 0;
    virtual PyObject* visit_return_node(ReturnNode* node) = 0;
    virtual PyObject* visit_class_node(ClassNode* node) = 0;
    virtual PyObject* visit_property_node(PropertyNode* node) = 0;
};


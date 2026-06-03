#pragma once

#include <memory>

#include "../ast/ast.hpp"
#include "../gc/gc.hpp"
#include "../scope/scope.hpp"
#include "../exceptions/python_exceptions.hpp"

class PyObject;

class Interpreter : public NodeVisitor {
   
public:
    Interpreter();
    ~Interpreter();

    PyObject* interpret(ProgramNode* node);

    /** Register a temporary PyObject (rc may be 0) so a later GC sweep can delete it if still unused. */
    void track_temp(PyObject* value) { gc.push_object(value); }

    void collect_garbage() { gc.free_unused(); }

    /** Batched sweep: avoids deleting freshly returned rc==0 objects mid-expression (see GC::push_object). */
    void maybe_collect() {
        if(gc.over_registration_budget()) {
            collect_garbage();
        }
    }

    virtual PyObject* visit_program_node(ProgramNode* node) override { return nullptr; }
    virtual PyObject* visit_block_node(BlockNode* node) override;
    virtual PyObject* visit_print_node(PrintNode* node) override;
    virtual PyObject* visit_while_node(WhileNode* node) override;
    virtual PyObject* visit_break_node(BreakNode* node) override;
    virtual PyObject* visit_continue_node(ContinueNode* node) override;
    virtual PyObject* visit_pass_node(PassNode* node) override;
    virtual PyObject* visit_if_node(IfNode* node) override;
    virtual PyObject* visit_assign_node(AssignNode* node) override;
    virtual PyObject* visit_ternary_op_node(TernaryOpNode* node) override;
    virtual PyObject* visit_binary_op_node(BinaryOpNode* node) override;
    virtual PyObject* visit_unary_op_node(UnaryOpNode* node) override;
    virtual PyObject* visit_int_node(IntNode* node) override;
    virtual PyObject* visit_float_node(FloatNode* node) override;
    virtual PyObject* visit_name_node(NameNode* node) override;
    virtual PyObject* visit_string_node(StringNode* node) override;
    virtual PyObject* visit_boolean_node(BooleanNode* node) override;
    virtual PyObject* visit_null_node(NullNode* expr) override;
    virtual PyObject* visit_function_node(FunctionNode* node) override;
    virtual PyObject* visit_call_node(CallNode* node) override;
    virtual PyObject* visit_return_node(ReturnNode* node) override;
    virtual PyObject* visit_class_node(ClassNode* node) override;
    virtual PyObject* visit_property_node(PropertyNode* node) override;

    /** Stack frame: `active` is looked up; `owned_frame` non-null means pop destroys that Scope (per-call env). */
    void push_borrowed(Scope* frame);
    void push_owned(std::unique_ptr<Scope> frame);

    void pop_context();

    Scope* current_context() {

        if(!context_stack.empty()) {
            return context_stack.back().active;
        }
        return nullptr;
    }

    void define_on_context(const std::string& name, PyObject* value) {

        if(!context_stack.empty()) {
            context_stack.back().active->define(name, value);
        } else {
            throw RuntimeError("Cannot define variable outside of context");
        }
    }

    PyObject* get_from_context(const std::string& name) {

        if(!context_stack.empty()) {
            return context_stack.back().active->get(name);
        }
        throw RuntimeError("Cannot access variable outside of context");
    }

private:
    struct ContextFrame {
        Scope* active = nullptr;
        std::unique_ptr<Scope> owned_frame;
    };

    GarbageCollector gc;
    std::vector<ContextFrame> context_stack;
    /* Declare builtins before globals so destructor tears down globals (enclosing builtins) first. */
    std::unique_ptr<Scope> root_builtins_;
    std::unique_ptr<Scope> root_globals_;
    /* During class body: defs share this live Scope (avoids a frozen snapshot missing later methods). */
    Scope* active_class_scope_ = nullptr;

    PyTypeObject* object_type = nullptr;
    PyTypeObject* type_type = nullptr;
    PyTypeObject* int_type = nullptr;
    PyTypeObject* float_type = nullptr;
    PyTypeObject* bool_type = nullptr;
    PyTypeObject* str_type = nullptr;
    PyTypeObject* none_type = nullptr;
    PyTypeObject* function_type = nullptr;
    PyTypeObject* builtin_fn_type = nullptr;
    PyTypeObject* method_wrapper_type = nullptr;

    // auxiliar methods
    PyObject* resolve(const std::string& method_name, PyObject* object, 
                std::vector<PyObject*> args = std::vector<PyObject*>());
    PyObject* try_resolve(const std::string& method_name, PyObject* object,
                std::vector<PyObject*> args = std::vector<PyObject*>());
    TokenType assignment_to_binary(TokenType assignment_op);
    
    void throw_missing_method_error(PyObject* object, const std::string& method_name);

    void validate_call_arity(PyObject* callable, size_t provided_arg_count);
    PyObject* eval_binary(TokenType op, PyObject* lhs, PyObject* rhs);
    
    PyObject* make_bool_obj(bool value);
    PyObject* make_int_obj(lld value);
    PyObject* make_float_obj(llf value);
    PyObject* make_str_obj(const std::string& value);
    PyObject* negate_truthy_obj(PyObject* value);

    inline const std::string& get_string(PyObject* object);
    inline long long get_integer(PyObject* object);
    inline bool is_truthy(PyObject* object);
    inline bool is_numeric(PyObject* object);
    inline long double to_float(PyObject* object);
};

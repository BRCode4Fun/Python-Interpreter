#pragma once

#include <vector>
#include <initializer_list>
#include <memory>
#include "../token/token.hpp"
#include "../ast/ast.hpp"

class Parser {

public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ProgramNode> parse();

private:
    std::unique_ptr<ProgramNode> parse_program();
    std::unique_ptr<AstNode> parse_stmt();
    std::unique_ptr<AstNode> parse_stmt_list();
    std::unique_ptr<AstNode> parse_simple_stmt();
    std::unique_ptr<AstNode> parse_print_stmt();
    std::unique_ptr<AstNode> parse_suite();
    std::unique_ptr<AstNode> parse_if_stmt();
    std::unique_ptr<AstNode> parse_while_stmt();
    std::unique_ptr<AstNode> parse_break_stmt();
    std::unique_ptr<AstNode> parse_continue_stmt();
    std::unique_ptr<AstNode> parse_pass_stmt();
    std::unique_ptr<AstNode> parse_return_stmt();
    std::unique_ptr<AstNode> parse_assign_simple();
    std::unique_ptr<AstNode> parse_assign();
    std::unique_ptr<AstNode> parse_expr();
    std::unique_ptr<AstNode> parse_conditional_expr();
    std::unique_ptr<AstNode> parse_disjunction();
    std::unique_ptr<AstNode> parse_conjunction();
    std::unique_ptr<AstNode> parse_inversion();
    std::unique_ptr<AstNode> parse_comparison();
    std::unique_ptr<AstNode> parse_bitwise_or();
    std::unique_ptr<AstNode> parse_bitwise_xor();
    std::unique_ptr<AstNode> parse_bitwise_and();
    std::unique_ptr<AstNode> parse_shift_expr();
    std::unique_ptr<AstNode> parse_factor();
    std::unique_ptr<AstNode> parse_term();
    std::unique_ptr<AstNode> parse_power();
    std::unique_ptr<AstNode> parse_unary();
    std::unique_ptr<AstNode> parse_call(std::unique_ptr<AstNode> func_name);
    std::unique_ptr<AstNode> parse_primary();
    std::unique_ptr<AstNode> parse_atom();
    std::vector<std::unique_ptr<AstNode>> parse_parameter_list();
    std::unique_ptr<AstNode> parse_function_def();
    std::unique_ptr<AstNode> parse_class_def();

    const std::vector<Token>& tokens;
    int current = 0;

    bool is_at_end() const;
    Token consume(TokenType type);
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();

    int loop_depth = 0;
    int function_depth = 0;
};

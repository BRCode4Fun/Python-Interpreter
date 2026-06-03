#include "parser.hpp"
#include "../exceptions/python_exceptions.hpp"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<ProgramNode> Parser::parse() {
    return parse_program();
}

std::unique_ptr<ProgramNode> Parser::parse_program() {
    /*
     *   program ::= (statement | NEWLINE)* $
    */
    std::vector<std::unique_ptr<AstNode>> statements;
    while (!is_at_end()) {
        if(match(TokenType::Newline)){
            continue;
        } else {
            statements.push_back(parse_stmt());
        }
    }
    auto block = std::make_unique<BlockNode>(std::move(statements));
    return std::make_unique<ProgramNode>(std::move(block));
}

std::unique_ptr<AstNode> Parser::parse_stmt() {
    /*
     *    statement ::= stmt_list NEWLINE
     *                  | compound_stmt
     *
     *    compound_stmt ::= if_stmt
     *                      | while_stmt
     *                      | for_stmt      TODO
     *                      | try_stmt      TODO
     *                      | with_stmt     TODO
     *                      | funcdef
     *                      | classdef
    */
    if (match(TokenType::If)){
        return parse_if_stmt();

    } else if (match(TokenType::While)){
        return parse_while_stmt();

    } else if (match(TokenType::Def)){
        return parse_function_def();

    } else if (match(TokenType::Class)) {
        return parse_class_def();

    } else {
        std::unique_ptr<AstNode> stmt_list = parse_stmt_list();
        while(match(TokenType::Newline)) continue;
        return stmt_list;
    }
}

std::vector<std::unique_ptr<AstNode>> Parser::parse_parameter_list() {
    /*
     *   parameter_list ::= identifier ("," identifier)*    // keep simple for now
    */
    std::vector<std::unique_ptr<AstNode>> parameters;
    do {
        Token tk = consume(TokenType::Name);
        parameters.push_back(std::make_unique<NameNode>(tk));
    } while (match(TokenType::Comma));

    return parameters;
}

std::unique_ptr<AstNode> Parser::parse_function_def(){
    /*
     *   funcdef ::= "def" funcname "(" (parameter_list)? ")" ":" suite
     *   funcname ::= identifier
    */
    Token fname = consume(TokenType::Name);
    consume(TokenType::LeftParen);

    std::vector<std::unique_ptr<AstNode>> parameters;

    if (peek().type != TokenType::RightParen) {
       parameters = parse_parameter_list();
    }
    consume(TokenType::RightParen);
    consume(TokenType::Colon);

    ++function_depth;
    std::unique_ptr<AstNode> body = parse_suite();
    --function_depth;

    return std::make_unique<FunctionNode>(fname, std::move(parameters), std::move(body));
}

std::unique_ptr<AstNode> Parser::parse_class_def(){
    /*
    *   classdef  ::= "class" classname (inheritance)? ":" suite // TODO inheritance
    *   classname ::= identifier
    */
    Token kname = consume(TokenType::Name);
    consume(TokenType::Colon);
    std::unique_ptr<AstNode> body = parse_suite();

    return std::make_unique<ClassNode>(kname, std::move(body));
}

std::unique_ptr<AstNode> Parser::parse_stmt_list() {
    /*
     *    stmt_list ::= simple_stmt (";" simple_stmt)* (";")?
    */
    std::vector<std::unique_ptr<AstNode>> stmts;

    stmts.push_back(parse_simple_stmt());

    while(match(TokenType::Semicolon)){
        if(peek().type == TokenType::Newline) break;
        stmts.push_back(parse_simple_stmt());
    }
    if(peek().type == TokenType::Semicolon) ++current;

    if(stmts.size() == 1) {
        return std::move(stmts[0]);
    }
    return std::make_unique<BlockNode>(std::move(stmts));
}

std::unique_ptr<AstNode> Parser::parse_simple_stmt() {
    /*
     *  simple_stmt ::= assignment_stmt
     *                   | assert_stmt     TODO
     *                   | pass_stmt       TODO
     *                   | del_stmt        TODO
     *                   | print_stmt
     *                   | return_stmt
     *                   | yield_stmt      TODO
     *                   | raise_stmt      TODO
     *                   | break_stmt
     *                   | continue_stmt
     *                   | import_stmt     TODO
     *                   | global_stmt     TODO
     *
    */
     if (match(TokenType::Print)) {
        return parse_print_stmt();
    } else if(match(TokenType::Return)){
        return parse_return_stmt();
    } else if(match(TokenType::Break)){
        return parse_break_stmt();
    } else if(match(TokenType::Continue)){
        return parse_continue_stmt();
    } else if(match(TokenType::Pass)) {
        return parse_pass_stmt();
    } else {
        return parse_assign();
    }
}

std::unique_ptr<AstNode> Parser::parse_return_stmt() {
    /*
     *  return_stmt ::= "return" (expression)?
    */
    if(!function_depth) {
        throw SyntaxError("Error at line " + std::to_string(peek().line) + ": Return outside Function.");
    }

    Token keyword = previous();
    std::unique_ptr<AstNode> expr = match({TokenType::Newline, TokenType::Semicolon}) ?
                    std::unique_ptr<AstNode>{} : parse_expr();
    return std::make_unique<ReturnNode>(keyword, std::move(expr));
}

std::unique_ptr<AstNode> Parser::parse_break_stmt() {
    /*
     *  break_stmt ::= "break"
    */
    if(!loop_depth) {
        throw SyntaxError("Error at line " + std::to_string(peek().line) + ": Break outside Loop.");
    }

    Token keyword = previous();
    return std::make_unique<BreakNode>(keyword);
}

std::unique_ptr<AstNode> Parser::parse_continue_stmt() {
    /*
     *  continue_stmt ::= "continue"
    */
    if(!loop_depth) {
        throw SyntaxError("Error at line " + std::to_string(peek().line) + ": Continue outside Loop.");
    }

    Token keyword = previous();
    return std::make_unique<ContinueNode>(keyword);
}

std::unique_ptr<AstNode> Parser::parse_pass_stmt() {
    /*
     *  pass_stmt ::= "pass"
    */
    Token keyword = previous();
    return std::make_unique<PassNode>(keyword);
}

std::unique_ptr<AstNode> Parser::parse_print_stmt() {
    /*
     *    print_stmt ::= "print" "(" (expression)? ")"
    */
    consume(TokenType::LeftParen);

    std::unique_ptr<AstNode> expr;
    if(peek().type != TokenType::RightParen) {
        expr = parse_expr();
    }
    consume(TokenType::RightParen);

    return std::make_unique<PrintNode>(std::move(expr));
}

std::unique_ptr<AstNode> Parser::parse_suite() {
    /*
     *    suite ::= NEWLINE* INDENT statement+ DEDENT
     *              | stmt_list NEWLINE
    */
    if(match(TokenType::Newline)) {

        // consumes '\n'
        while(match(TokenType::Newline)) continue;

        consume(TokenType::Indent);

        std::vector<std::unique_ptr<AstNode>> stmts;
        stmts.push_back(parse_stmt());

        while(peek().type != TokenType::Dedent) {
            std::unique_ptr<AstNode> node = parse_stmt();

            if(node->is_block_node()) {
                BlockNode* block = node->unwrap_block_node();

                for(auto& stmt : block->statements) {
                    stmts.push_back(std::move(stmt));
                }
            } else {
                stmts.push_back(std::move(node));
            }
        }
        consume(TokenType::Dedent);

        return std::make_unique<BlockNode>(std::move(stmts));

    } else {
        std::unique_ptr<AstNode> stmt_list = parse_stmt_list();
        consume(TokenType::Newline);
        return stmt_list;
    }
}

std::unique_ptr<AstNode> Parser::parse_if_stmt() {
    /*
     *    if_stmt ::= "if" expression ":" suite
     *                ("elif" expression ":" suite)*
     *                ("else" ":" suite)?
    */
    std::unique_ptr<AstNode> cond = parse_expr();
    consume(TokenType::Colon);
    std::unique_ptr<AstNode> true_branch = parse_suite();

    std::vector<std::pair<std::unique_ptr<AstNode>, std::unique_ptr<AstNode>>> elif_branches;
    while(match(TokenType::Elif)) {
        std::unique_ptr<AstNode> elif_cond = parse_expr();
        consume(TokenType::Colon);
        std::unique_ptr<AstNode> elif_suite = parse_suite();
        elif_branches.emplace_back(std::move(elif_cond), std::move(elif_suite));
    }

    std::unique_ptr<AstNode> else_branch;
    if(match(TokenType::Else)) {
        consume(TokenType::Colon);
        else_branch = parse_suite();
    }

    return std::make_unique<IfNode>(
        std::move(cond), std::move(true_branch), std::move(elif_branches), std::move(else_branch));
}

std::unique_ptr<AstNode> Parser::parse_while_stmt() {
    /*
     *   while_stmt ::= "while" expression ":" suite
     *                  ("else" ":" suite)?  //TODO
    */
    std::unique_ptr<AstNode> cond = parse_expr();
    consume(TokenType::Colon);
    loop_depth++;
    std::unique_ptr<AstNode> body = parse_suite();
    loop_depth--;

    return std::make_unique<WhileNode>(std::move(cond), std::move(body));
}

std::unique_ptr<AstNode> Parser::parse_assign_simple() {
    /*
     *  assign_simple ::= expression ("=" assign_stmt)* // TODO target
    */
    std::unique_ptr<AstNode> left = parse_expr();

    if(match(TokenType::Equals)) {
        Token op = previous();
        std::unique_ptr<AstNode> right = parse_assign_simple();
        return std::make_unique<AssignNode>(std::move(left), std::move(right), op);
    }
    return left;
}

std::unique_ptr<AstNode> Parser::parse_assign() {
    /*
     *  assign_stmt ::= assign_simple | augmented_assignment_stmt
     *  augmented_assignment_stmt ::= expression augop expression // TODO target, expression_list
     *  augop ::= "+="  | "-=" ¦ "*=" | "/="
     *          ¦ "%="  | "&=" | "|=" ¦ "^="
     *          | "<<=" | ">>="
    */
    std::unique_ptr<AstNode> left = parse_assign_simple();

    if(match({TokenType::PlusEqual, TokenType::MinusEqual,
              TokenType::StarEqual, TokenType::SlashEqual, TokenType::ModEqual,
              TokenType::AndEqual, TokenType::OrEqual, TokenType::XorEqual,
              TokenType::LeftShiftEqual, TokenType::RightShiftEqual})) {
        Token op = previous();
        std::unique_ptr<AstNode> right = parse_expr();

        if(left->is_name_node() or left->is_property_node()) {
            return std::make_unique<AssignNode>(std::move(left), std::move(right), op);
        } else {
            throw SyntaxError("Error at line " + std::to_string(peek().line) + ": Invalid compound assignment.");
        }
    }
    return left;
}

std::unique_ptr<AstNode> Parser::parse_expr() {
    /*
     *  expression ::= conditional_expression
     *                 | lambda_form // TODO
    */
    return parse_conditional_expr();
}

std::unique_ptr<AstNode> Parser::parse_conditional_expr() {
    /*
     *  conditional_expression ::= disjunction ("if" disjunction "else" expression)?
    */
    std::unique_ptr<AstNode> left = parse_disjunction();

    if(match(TokenType::If)) {
        std::unique_ptr<AstNode> condition = parse_disjunction();
        consume(TokenType::Else);
        std::unique_ptr<AstNode> right = parse_expr();
        left = std::make_unique<TernaryOpNode>(std::move(condition), std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<AstNode> Parser::parse_disjunction() {
    /*
     *  disjunction ::= conjunction ("or" conjunction)*
    */
    std::unique_ptr<AstNode> left = parse_conjunction();

    while (match(TokenType::Or)) {
        Token op = previous();
        std::unique_ptr<AstNode> right = parse_conjunction();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<AstNode> Parser::parse_conjunction() {
    /*
     *  conjunction ::= inversion ("and" inversion)*
    */
    std::unique_ptr<AstNode> left = parse_inversion();

    while (match(TokenType::And)) {
        Token op = previous();
        std::unique_ptr<AstNode> right = parse_inversion();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<AstNode> Parser::parse_inversion() {
    /*
     *   inversion ::= "not" inversion
     *                | comparison
    */
    if (match(TokenType::Not)) {
        Token op = previous();
        std::unique_ptr<AstNode> right = parse_inversion();
        return std::make_unique<UnaryOpNode>(op, std::move(right));
    } else {
        return parse_comparison();
    }
}

std::unique_ptr<AstNode> Parser::parse_comparison() {
    /*
     *   comparison ::= factor (comp_operator factor)*
     *   comp_operator ::= "<" | ">" | "==" | ">=" | "<=" | "!="
                           | "is" ("not")? | ("not")? "in"  // TODO
    */
    std::unique_ptr<AstNode> left = parse_bitwise_or();

    while (match({TokenType::Less, TokenType::Greater,
                  TokenType::EqualEqual, TokenType::GreaterEqual,
                  TokenType::LessEqual, TokenType::BangEqual})) {
        Token op = previous();
        std::unique_ptr<AstNode> right = parse_bitwise_or();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<AstNode> Parser::parse_bitwise_or() {
    /*
     *    bitwise_or ::= bitwise_xor ("|" bitwise_xor)*
    */
    std::unique_ptr<AstNode> left = parse_bitwise_xor();

    while(match(TokenType::Pipe)) {
        Token op = previous();
        std::unique_ptr<AstNode> right = parse_bitwise_xor();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<AstNode> Parser::parse_bitwise_xor() {
    /*
     *    bitwise_xor ::= bitwise_and ("^" bitwise_and)*
    */
    std::unique_ptr<AstNode> left = parse_bitwise_and();

    while(match(TokenType::Caret)) {
        Token op = previous();
        std::unique_ptr<AstNode> right = parse_bitwise_and();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<AstNode> Parser::parse_bitwise_and() {
    /*
     *   bitwise_and ::= shift_expr ("&" shift_expr)*
    */
    std::unique_ptr<AstNode> left = parse_shift_expr();

    while (match(TokenType::Ampersand)) {
        Token op = previous();
        std::unique_ptr<AstNode> right = parse_shift_expr();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<AstNode> Parser::parse_shift_expr() {
    /*
     *   shift_expr ::= factor (("<<" | ">>") factor)*
    */
    std::unique_ptr<AstNode> left = parse_factor();

    while (match({TokenType::LeftShift, TokenType::RightShift})) {
        Token op = previous();
        std::unique_ptr<AstNode> right = parse_factor();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<AstNode> Parser::parse_factor() {
    /*
     *   factor ::= term (("+" | "-") term)*
    */
    std::unique_ptr<AstNode> left = parse_term();

    while (match({TokenType::Plus, TokenType::Minus})) {
        Token op = previous();
        std::unique_ptr<AstNode> right = parse_term();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<AstNode> Parser::parse_term() {
    /*
     *   term ::= unary (("*" | "/" | "%") unary)*
    */
    std::unique_ptr<AstNode> left = parse_unary();

    while (match({TokenType::Star, TokenType::Slash, TokenType::Mod})) {
        Token op = previous();
        std::unique_ptr<AstNode> right = parse_unary();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<AstNode> Parser::parse_unary() {
    /*
     *   unary ::= (("+" | "-" | "~") unary) | primary
    */
    if (match({TokenType::Minus, TokenType::Tilde})) {
        Token op = previous();
        std::unique_ptr<AstNode> right = parse_unary();
        return std::make_unique<UnaryOpNode>(op, std::move(right));

    } else if (match(TokenType::Plus)) {
        return parse_unary();

    } else {
        return parse_primary();
    }
}

std::unique_ptr<AstNode> Parser::parse_call(std::unique_ptr<AstNode> callee) {
    /*
     *  call ::= primary "(" (argument_list)? ")"
    */
    std::vector<std::unique_ptr<AstNode>> args;

    if(peek().type != TokenType::RightParen) {
        do {
            args.push_back(parse_expr());
        } while(match(TokenType::Comma));
    }
    consume(TokenType::RightParen);

    return std::make_unique<CallNode>(std::move(callee), std::move(args));
}

std::unique_ptr<AstNode> Parser::parse_primary() {
    /*
     *   primary ::= atom | attributeref
     *               | subscription | slicing | call
     *   attributeref ::= primary "." identifier
     *   subscription ::= primary "[" expression_list "]"    // TODO
     *   slicing ::= simple_slicing | extended_slicing       // TODO
    */
    std::unique_ptr<AstNode> left = parse_atom();

    while(true) {
        if(match(TokenType::LeftParen)) {
            left = parse_call(std::move(left));

        } else if(match(TokenType::Dot)) {
            std::unique_ptr<AstNode> right = std::make_unique<NameNode>(consume(TokenType::Name));
            left = std::make_unique<PropertyNode>(std::move(left), std::move(right));

        } else {
            break;
        }
    }
    return left;
}

std::unique_ptr<AstNode> Parser::parse_atom() {
    /*
     *  atom ::= identifier | literal | enclosure
     *  literal ::= stringliteral | integer | floatnumber
     *             | "None" | "True" | "False"
     *  enclosure ::= "(" expression ")"
    */
    if (match(TokenType::LeftParen)) {
        std::unique_ptr<AstNode> expr = parse_expr();
        consume(TokenType::RightParen);
        return expr;
    } else {
        Token tk = advance();

        switch(tk.type) {
            case TokenType::Int:
                return std::make_unique<IntNode>(tk);
            case TokenType::Float:
                return std::make_unique<FloatNode>(tk);
            case TokenType::True:
                return std::make_unique<BooleanNode>(true);
            case TokenType::False:
                return std::make_unique<BooleanNode>(false);
            case TokenType::None:
                return std::make_unique<NullNode>(tk);
            case TokenType::String:
                return std::make_unique<StringNode>(tk);
            case TokenType::Name: {
                return std::make_unique<NameNode>(tk);
            } default: {
                throw SyntaxError("Error at line " + std::to_string(peek().line) + ": Expected a primary expression");
            }
        }
    }
}

Token Parser::consume(TokenType type) {
    if (match(type)) {
        return previous();
    }
    else {
        throw SyntaxError("Error at line " + std::to_string(peek().line) + ": Expected " + std::to_string(type));
    }
}

bool Parser::match(TokenType type) {
    if (is_at_end() || peek().type != type) {
        return false;
    } else {
        ++current;
        return true;
    }
}

bool Parser::match(std::initializer_list<TokenType> types) {

    if(is_at_end()) return false;

    for(const TokenType& type : types) {
        if(peek().type == type) {
            ++current;
            return true;
        }
    }
    return false;
}

bool Parser::is_at_end() const {
    return peek().type == TokenType::EndOfFile;
}

const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::previous() const {
    return tokens[(current > 0) ? current - 1 : 0];
}

const Token& Parser::advance() {
    return tokens[current++];
}

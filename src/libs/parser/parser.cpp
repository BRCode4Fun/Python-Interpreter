#include "parser.hpp"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

ProgramNode* Parser::parse() {
    return parseProgram();
}

ProgramNode* Parser::parseProgram() {
    /*
     *   program ::= (statement | NEWLINE)* $
    */
    std::vector<AstNode*> statements;
    while (!isAtEnd()) {
        if(match(TokenType::Newline)){
            continue;
        } else {
            statements.push_back(parseStmt());
        }
    }
    return new ProgramNode(new BlockNode(statements));
}

AstNode* Parser::parseStmt() {
    /*
     *    statement ::= stmt_list NEWLINE
     *                  | compound_stmt
     *
     *    compound_stmt ::= if_stmt
     *                      | while_stmt
     *                      | for_stmt      TODO
     *                      | try_stmt      TODO
     *                      | with_stmt     TODO
     *                      | funcdef       TODO
     *                      | classdef      TODO
     */
    if (match(TokenType::If)){
        return parseIfStmt();
        
    } else if (match(TokenType::While)){
        return parseWhileStmt();
        
    }  else {
        auto stmtList = parseStmtList();
        while(match(TokenType::Newline)) continue;
        return stmtList;
    }
}

AstNode* Parser::parseStmtList() {
    /*
     *    stmt_list ::= simple_stmt (SEMICOLON simple_stmt)* SEMICOLON?
     */
    vector<AstNode*> stmts;
    
    stmts.push_back(parseSimpleStmt());
    
    while(match(TokenType::Semicolon)){
        if(peek().type == TokenType::Newline) break;
        stmts.push_back(parseSimpleStmt());
    }
    if(peek().type == TokenType::Semicolon) ++current;
    
    return new BlockNode(stmts);
}

AstNode* Parser::parseSimpleStmt() {
    /*
     *  simple_stmt ::= assignment_stmt
     *                   | assert_stmt     TODO
     *                   | pass_stmt       TODO
     *                   | del_stmt        TODO
     *                   | print_stmt      
     *                   | return_stmt     TODO
     *                   | yield_stmt      TODO
     *                   | raise_stmt      TODO
     *                   | break_stmt      TODO
     *                   | continue_stmt   TODO
     *                   | import_stmt     TODO
     *                   | global_stmt     TODO
     *                   | exec_stmt       TODO
     *
     */
     if (match(TokenType::Print)) {
        return parsePrintStmt();
    } else {
        return parseAssign();
    }
}

AstNode* Parser::parsePrintStmt() {
    /*
     *    print_stmt ::= PRINT LEFT_PAREN expression RIGHT_PAREN
     */
    consume(TokenType::LeftParen);
    auto expr = parseExpr();
    consume(TokenType::RightParen);

    return new PrintNode(expr);
}

AstNode* Parser::parseSuite() {
    /*
     *    suite ::= NEWLINE INDENT statement+ DEDENT
     *              | stmt_list NEWLINE
     */
    if(match(TokenType::Newline)) {
        consume(TokenType::Indent);
        
        vector<AstNode*> stmts;
    
        stmts.push_back(parseStmt());
    
        while(peek().type != TokenType::Dedent){
            stmts.push_back(parseStmt());
        }
        consume(TokenType::Dedent);
      
        return new BlockNode(stmts);
    } else {
        auto stmtList = parseStmtList();
        consume(TokenType::Newline);
        return stmtList;
    }
}

AstNode* Parser::parseIfStmt() {
    /*
     *    if_stmt ::= IF expression COLON suite
     *                (ELIF expression COLON suite)*
     *                (ELSE COLON suite)?
    */
    auto cond =  parseExpr();
      
    consume(TokenType::Colon);
    
    auto trueBranch = parseSuite();

    return new IfNode(cond, trueBranch);
}

AstNode* Parser::parseWhileStmt() {
    /*
     *   while_stmt ::= WHILE expression COLON suite
     *                  (ELSE COLON suite)?  //TODO
    */
    auto cond =  parseExpr();
      
    consume(TokenType::Colon);

    auto body = parseSuite();

    return new WhileNode(cond, body);
}

AstNode* Parser::parseAssign() {
    /*
     *  assign_stmt ::= expression EQUALS assign_stmt
     *                  | expression
    */
    auto left = parseExpr();
    
    while (match(TokenType::Equals)) {
        //auto op = previous();
        auto right = parseAssign();
        if(!left->is_name_node())
            error("cannot assign to expression");
        left = new AssignNode(left, right);
    }
    return left;
}

AstNode* Parser::parseExpr() {
    return parseTernary();
}

AstNode* Parser::parseTernary() {
    
    auto left = parseLogicOr();
    
    if(match(TokenType::If)) {
        auto condition = parseLogicOr();
        consume(TokenType::Else);
        auto right = parseExpr();
        left = new TernaryOpNode(condition, left, right);
    }
    return left;
}

AstNode* Parser::parseLogicOr() {
    
    auto left = parseLogicAnd();
    
    while (match(TokenType::Or)) {
        auto op = previous();
        auto right = parseLogicAnd();
        left = new BinaryOpNode(left, op.lexeme, right);
    }
    return left;
}

AstNode* Parser::parseLogicAnd() {
    
    auto left = parseEquality();
    
    while (match(TokenType::And)) {
        auto op = previous();
        auto right = parseEquality();
        left = new BinaryOpNode(left, op.lexeme, right);
    }
    return left;
}

AstNode* Parser::parseEquality() {
    
    auto left = parseRelational();
    
    while (match(TokenType::EqualEqual) || match(TokenType::BangEqual)) {
        auto op = previous();
        auto right = parseRelational();
        left = new BinaryOpNode(left, op.lexeme, right);
    }
    return left;
}

AstNode* Parser::parseRelational() {
    
    auto left = parseFactor();
    
    while (match(TokenType::Less) || match(TokenType::LessEqual) ||
            match(TokenType::Greater) || match(TokenType::GreaterEqual)) {
        auto op = previous();
        auto right = parseFactor();
        left = new BinaryOpNode(left, op.lexeme, right);
    }
    return left;
}

AstNode* Parser::parseFactor() {
    
    auto left = parseTerm();
    
    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        auto op = previous();
        auto right = parseTerm();
        left = new BinaryOpNode(left, op.lexeme, right);
    }
    return left;
}

AstNode* Parser::parseTerm() {
   
    auto left = parseUnary();
    
    while (match(TokenType::Star) || match(TokenType::Slash)) {
        auto op = previous();
        auto right = parseUnary();
        left = new BinaryOpNode(left, op.lexeme, right);
    }
    return left;
}

AstNode* Parser::parseUnary() {
    
    if (match(TokenType::Minus) || match(TokenType::Not)) {
        auto op = previous();
        auto right = parseUnary();
        return new UnaryOpNode(op.lexeme, right);

    } else if (match(TokenType::Plus)) {
        return parseUnary();
    
    } else {
        return parseCall();
    }
}

AstNode* Parser::parseCall() {
    
    auto primary = parsePrimary();

    while (match(TokenType::LeftParen)) {
        std::vector<AstNode*> args;
        if (!match(TokenType::RightParen)) {
            do {
                args.push_back(parseExpr());
            } while (match(TokenType::Comma));
            consume(TokenType::RightParen);
        }
        primary = new CallNode(primary, args);
    }
    return primary;
}

AstNode* Parser::parsePrimary() {
    
    if (match(TokenType::Number)) {
        return new NumNode(std::stod(previous().lexeme));
        
    } else if(match(TokenType::True)) {
        return new BooleanNode(true);

    } else if(match(TokenType::False)) {
        return new BooleanNode(false);

    } else if(match(TokenType::None)) {
        return new NullNode();

    } else if (match(TokenType::Name)) {
        return new NameNode(previous().lexeme);

    } else if (match(TokenType::String)) {
        return new StringNode(previous().lexeme);

    } else if (match(TokenType::LeftParen)) {
        auto expr = parseExpr();
        consume(TokenType::RightParen);
        return expr;
    } else {
        error("Expected a primary expression");
        return nullptr;
    }
}

Token Parser::consume(TokenType type) {
    if (match(type)) return previous();
    else {
        error("Expected " );
        return Token(TokenType::Error, "", 0);
    }
}

bool Parser::match(TokenType type) {
    if (isAtEnd() or peek().type != type) {
        return false;
    }
    ++current;
    return true;
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

void Parser::error(const std::string& message) {
    
    std::cerr << "Error at line " << peek().line << ": " << message << std::endl;
    exit(EXIT_FAILURE);
}

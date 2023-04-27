#include "parser.hpp"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

ProgramNode* Parser::parse() {
    return parseProgram();
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
}

AstNode* Parser::parseExpr() {
    return parseFactor();
}

AstNode* Parser::parsePrimary() {
    if (match(TokenType::Number)) {
        return new NumNode(std::stod(previous().lexeme));
        
    } else if (match(TokenType::Identifier)) {
        return new NameNode(previous().lexeme);
    
    } else if (match(TokenType::LeftParen)) {
        auto expr = parseExpr();
        consume(TokenType::RightParen);
        return expr;
    
    } else {
        error("Expected a primary expression");
        return nullptr;
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

AstNode* Parser::parseUnary() {
    if (match(TokenType::Minus)) {
        auto right = parseUnary();
        return new UnaryOpNode("-", right);
    } else {
        return parseCall();
    }
}

AstNode* Parser::parseTerm() {
   
    auto left = parseUnary();
    while (match(TokenType::Multiply) || match(TokenType::Divide)) {
        auto op = previous();
        auto right = parseUnary();
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

AstNode* Parser::parseStmt() {
    
    if (match(TokenType::Print)) {
        
        consume(TokenType::LeftParen);
        auto expr = parseExpr();
        consume(TokenType::RightParen);
        
        consume(TokenType::Indent);
        return new PrintNode(expr);

    } else if (match(TokenType::Identifier)) {

        auto name = previous().lexeme;
        consume(TokenType::Equal);
        auto expr = parseExpr();

        consume(TokenType::Indent);
        return new AssignNode(new NameNode(name), expr);

    } else {
        error("Expected a statement");
        return nullptr;
    }
}

ProgramNode* Parser::parseProgram() {
    std::vector<AstNode*> statements;
    while (!isAtEnd()) {
        statements.push_back(parseStmt());
    }
    return new ProgramNode(statements);
}

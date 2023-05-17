#include "parser.hpp"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

ProgramNode* Parser::parse() {
    return parseProgram();
}

ProgramNode* Parser::parseProgram() {
    
    std::vector<AstNode*> statements;
    while (!isAtEnd()) {
        statements.push_back(parseStmt());
    }
    return new ProgramNode(statements);
}

AstNode* Parser::parseStmt() {
    
    if (match(TokenType::Print)) {
        consume(TokenType::LeftParen);
        auto expr = parseExpr();
        consume(TokenType::RightParen);

        if(!isAtEnd())
            consume(TokenType::Indent); 

        return new PrintNode(expr);

    } else if (match(TokenType::Identifier)){
        
        current--;
        auto expr =  parseExpr();

        if(!isAtEnd())
            consume(TokenType::Indent);
        
        return expr;
    } else {
        cout << peek().lexeme << "\n";
        error("Expected statement");
    }
}

AstNode* Parser::parseExpr() {

    return parseAssign();
}

AstNode* Parser::parseAssign() {
    
    auto left = parseLogicOr();
    
    while (match(TokenType::Equals)) {
        auto op = previous();
        auto right = parseAssign();
        left = new AssignNode(left, right);
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
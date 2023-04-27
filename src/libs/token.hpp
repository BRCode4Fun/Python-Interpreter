#pragma once

enum TokenType {
    EndOfFile,
    Error,
    LessEqual,
    EqualEqual,
    LeftBrace,
    Print,
    Less,
    BangEqual,
    Bang,
    Indent,
    Identifier,
    Number,
    Plus,
    Minus,
    Multiply,
    Divide,
    Equals,
    LeftParen,
    GreaterEqual,
    RightParen,
    Comma,
    Semicolon,
    Dedent,
    Dot,
    Greater,
    Equal,
    Slash,
    Star,
    RightBrace, 
    Colon
};

struct Token {

    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType type, const std::string& lexeme, int line)
        : type(type), lexeme(lexeme), line(line) {}

    std::string getTokenTypeString(TokenType type) {
        switch (type) {
            case TokenType::LeftParen:
                return "LeftParen";
            case TokenType::RightParen:
                return "RightParen";
            case TokenType::Plus:
                return "Plus";
            case TokenType::Minus:
                return "Minus";
            case TokenType::Equal:
                return "Assign";
            case TokenType::Multiply:
                return "Multiply";
            case TokenType::Divide:
                return "Divide";
            case TokenType::Semicolon:
                return "Semicolon";
            case TokenType::Number:
                return "Number";
            case TokenType::Identifier:
                return "Identifier";
            case TokenType::Print:
                return "Print";
            case TokenType::EndOfFile:
                return "EndOfFile";
            default:
                return "Error";
        }
    } 
};

#pragma once

#include "./tokentype.hpp"

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
            case TokenType::Equals:
                return "Assign";
            case TokenType::Star:
                return "Star";
            case TokenType::Slash:
                return "Slash";
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

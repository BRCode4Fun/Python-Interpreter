#pragma once

#include <iostream>
#include "./tokentype.hpp"

class Token {

public:
    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType type, const std::string& lexeme, int line)
        : type(type), lexeme(lexeme), line(line) {}

    friend std::ostream& operator<<(std::ostream& out, Token& token) {
        
        switch (token.type) {
            case TokenType::LeftParen:
                out << "LeftParen"; break;
            case TokenType::RightParen:
                out << "RightParen"; break;
            case TokenType::Indent:
                out << "Indent"; break;
            case TokenType::Dedent:
                out << "Dedent"; break;
            case TokenType::Plus:
                out << "Plus"; break;
            case TokenType::Minus:
                out << "Minus"; break;
            case TokenType::Star:
                out << "Star"; break;
            case TokenType::Slash:
                out << "Slash"; break;
            case TokenType::Equals:
                out << "Assign"; break;
            case TokenType::EqualEqual:
                out << "EqualEqual"; break;
            case TokenType::BangEqual:
                out << "BangEqual"; break;
            case TokenType::Less:
                out << "Less"; break;
            case TokenType::LessEqual:
                out << "LessEqual"; break;
            case TokenType::Greater:
                out << "Greater"; break;
            case TokenType::GreaterEqual:
                out << "GreaterEqual"; break;
            case TokenType::Semicolon:
                out << "Semicolon"; break;
            case TokenType::Number:
                out << "Number"; break;
            case TokenType::Identifier:
                out << "Identifier"; break;
            case TokenType::True:
                out << "True"; break;
            case TokenType::False:
                out << "False"; break;
            case TokenType::None:
                out << "None"; break;
            case TokenType::Print:
                out << "Print"; break;
            case TokenType::While:
                out << "While"; break;
            case TokenType::If:
                out << "If"; break;
            case TokenType::And:
                out << "And"; break;
            case TokenType::Or:
                out << "Or"; break;
            case TokenType::Not:
                out << "Not"; break;
            case TokenType::EndOfFile:
                out << "EndOfFile"; break;
            default:
                out << "Error"; break;
        }
        out << " <- " << "\'" << token.lexeme << "\'";

        return out;
    }
};

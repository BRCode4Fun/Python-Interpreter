#pragma once

#include <iostream>
#include "./tokentype.hpp"

class Token {

public:
    Token(TokenType type, const std::string& lexeme, int line)
        : type(type), lexeme(lexeme), line(line) {}

    friend std::ostream& operator<<(std::ostream& out, const Token& token) {
        
        switch (token.type) {
            case TokenType::LeftParen:
                out << "LeftParen"; break;
            case TokenType::RightParen:
                out << "RightParen"; break;
            case TokenType::Indent:
                out << "Indent"; break;
            case TokenType::Dedent:
                out << "Dedent"; break;
            case TokenType::Dot:
                out << "Dot"; break;
            case TokenType::Plus:
                out << "Plus"; break;
            case TokenType::Minus:
                out << "Minus"; break;
            case TokenType::Star:
                out << "Star"; break;
            case TokenType::DoubleStar:
                out << "DoubleStar"; break;
            case TokenType::Slash:
                out << "Slash"; break;
            case TokenType::DoubleSlash:
                out << "DoubleSlash"; break;
            case TokenType::Mod:
                out << "Mod"; break;
            case TokenType::Pipe:
                out << "Pipe"; break;
            case TokenType::Caret:
                out << "Caret"; break;
            case TokenType::Ampersand:
                out << "Ampersand"; break;
            case TokenType::Tilde:
                out << "Tilde"; break;
            case TokenType::LeftShift:
                out << "LeftShift"; break;
            case TokenType::RightShift:
                out << "RightShift"; break;
            case TokenType::Equals:
                out << "Equals"; break;
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
            case TokenType::PlusEqual:
                out << "PlusEqual"; break;
            case TokenType::MinusEqual:
                out << "MinusEqual"; break;
            case TokenType::StarEqual:
                out << "StarEqual"; break;
            case TokenType::SlashEqual:
                out << "SlashEqual"; break;
            case TokenType::ModEqual:
                out << "ModEqual"; break;
            case TokenType::AndEqual:
                out << "AndEqual"; break;
            case TokenType::OrEqual:
                out << "OrEqual"; break;
            case TokenType::XorEqual:
                out << "XorEqual"; break;
            case TokenType::LeftShiftEqual:
                out << "LeftShiftEqual"; break;
            case TokenType::RightShiftEqual:
                out << "RightShiftEqual"; break;
            case TokenType::Semicolon:
                out << "Semicolon"; break;
            case TokenType::Colon:
                out << "Colon"; break;
            case TokenType::Comma:
                out << "Comma"; break;
            case TokenType::Int:
                out << "Int"; break;
            case TokenType::Float:
                out << "Float"; break;
            case TokenType::Name:
                out << "Name"; break;
            case TokenType::String:
                out << "String"; break;
            case TokenType::True:
                out << "True"; break;
            case TokenType::False:
                out << "False"; break;
            case TokenType::None:
                out << "None"; break;
            case TokenType::Print:
                out << "Print"; break;
            case TokenType::Class:
                out << "Class"; break;
            case TokenType::Def:
                out << "Def"; break;
            case TokenType::While:
                out << "While"; break;
            case TokenType::Continue:
                out << "Continue"; break;
            case TokenType::Break:
                out << "Break"; break;
            case TokenType::Pass:
                out << "Pass"; break;
            case TokenType::If:
                out << "If"; break;
            case TokenType::Elif:
                out << "Elif"; break;
            case TokenType::Else:
                out << "Else"; break;
            case TokenType::And:
                out << "And"; break;
            case TokenType::Return:
                out << "Return"; break;
            case TokenType::Or:
                out << "Or"; break;
            case TokenType::Not:
                out << "Not"; break;
            case TokenType::Newline:
                out << "Newline"; break;
            case TokenType::EndOfFile:
                out << "EndOfFile"; break;
            default:
                out << "Error"; break;
        }
        
        out << " <- " << "\'";
            
        for (char c : token.lexeme) {
            if (c == '\n') {
                out << "\\n";
            } else {
                out << c;
            }
        }
        out << "\'";
  
        return out;
    }
    
    TokenType type;
    std::string lexeme;
    int line;
};

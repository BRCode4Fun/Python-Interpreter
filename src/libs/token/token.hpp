#pragma once

#include <iosfwd>
#include <string>
#include "./token_type.hpp"

class Token {

public:
    Token(TokenType type, const std::string& lexeme, int line)
        : type(type), lexeme(lexeme), line(line) {}

    friend std::ostream& operator<<(std::ostream& out, const Token& token);
    
    TokenType type;
    std::string lexeme;
    int line;
};

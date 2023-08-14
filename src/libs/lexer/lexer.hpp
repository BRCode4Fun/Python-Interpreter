#pragma once

#include <vector>
#include <string>
#include <stack>
#include <unordered_map>
#include "../token/token.hpp"

class Lexer {

public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& lexeme);
    bool match(char expected);
    char peek() const;
    char peekNext() const;
    bool isAtEnd() const;

    void handleNumber();
    void handleIdentifier();
    void handleString(char quotedType);
    void handleNewline();
    void handleIndentation(); 

    const std::string& source;
    std::vector<Token> tokens;
    std::unordered_map<std::string, TokenType> keywords;
    long unsigned start = 0;
    long unsigned current = 0;
    unsigned int line = 1;
    std::stack<int> indentLevels;
    bool isBlock;
};

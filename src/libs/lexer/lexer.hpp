#pragma once

#include <vector>
#include <string>
#include "../token/token.hpp"
#include <unordered_map>
#include <stack>

class Lexer {
    public:
        Lexer(const std::string& source);
        std::vector<Token> scanTokens();

    private:
        const std::string& source;
        std::vector<Token> tokens;
        long unsigned start = 0;
        long unsigned current = 0;
        unsigned int line = 1;
        int s = 10;
        int t = 10;

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

        std::unordered_map<std::string, TokenType> keywords;

        std::stack<int> indentLevels;
        bool isBlock;
};

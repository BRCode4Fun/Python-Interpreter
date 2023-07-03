#pragma once

#include <vector>
#include <string>
#include <stack>
#include <unordered_map>
#include "../token/token.hpp"

using namespace std;

class Lexer {
    public:
        Lexer(const string& source);
        vector<Token> scanTokens();

    private:
        const string& source;
        vector<Token> tokens;
        long unsigned start = 0;
        long unsigned current = 0;
        unsigned int line = 1;

        void scanToken();
        char advance();
        void addToken(TokenType type);
        void addToken(TokenType type, const string& lexeme);
        bool match(char expected);
        char peek() const;
        char peekNext() const;
        bool isAtEnd() const;

        void handleNumber();
        void handleIdentifier();
        void handleString(char quotedType);
        void handleNewline();
        void handleIndentation(); 

        unordered_map<string, TokenType> keywords;

        stack<int> indentLevels;
        bool isBlock;
};

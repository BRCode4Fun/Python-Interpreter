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
      int start = 0;
      int current = 0;
      int line = 1;

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
      void handleNewline();

      int currentIndentLevel = 0; 
      std::stack<int> indentLevels;

      std::unordered_map<std::string, TokenType> keywords;
};

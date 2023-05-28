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
      void handleWhitespace();
     

      int currentIndentLevel; 
      bool start_newline = false; 
      bool  is_block =  false;

      bool has_symbol = false; 

      std::stack<int> indentLevels;
      std::unordered_map<std::string, TokenType> keywords;
};

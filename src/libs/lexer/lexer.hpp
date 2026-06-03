#pragma once

#include <vector>
#include <string>
#include <stack>
#include <unordered_map>
#include "../token/token.hpp"

class Lexer {

public:
    Lexer(const std::string& source);
    std::vector<Token> scan_tokens();

private:
    void scan_token();
    char advance();
    void add_token(TokenType type);
    void add_token(TokenType type, const std::string& lexeme);
    bool match(char expected);
    char peek() const;
    char peek_next() const;
    bool is_at_end() const;

    void handle_number();
    void handle_identifier();
    void handle_string(char quote_type);
    void handle_newline();
    void handle_indentation(); 

    const std::string& source;
    std::vector<Token> tokens;
    std::unordered_map<std::string, TokenType> keywords;
    long unsigned start = 0;
    long unsigned current = 0;
    unsigned int line = 1;
    std::stack<int> indent_levels;
    bool is_block;
};

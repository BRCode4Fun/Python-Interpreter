#include <stdexcept>
#include <iostream>
#include <fstream>
#include "lexer.hpp"
#include "../exceptions/python_exceptions.hpp"

Lexer::Lexer(const std::string& source) 
    : source(source), current(0), line(1) {
    /*
     *  keyword ::= "and" | "del" | "from" | "not" | "while"
     *            | "as" | "elif" | "global" | "or" | "with"
     *            | "assert" | "else" | "if" | "pass" | "yield"
     *            | "break" | "except" | "import" | "print"
     *            | "class" | "in" | "raise" | "continue" 
     *            | "finally" | "is" | "return" | "def" 
     *            | "for" | "lambda" | "try"
    */
    keywords = {
        {"print", TokenType::Print},
        {"True", TokenType::True},
        {"False", TokenType::False},
        {"None", TokenType::None},
        {"and", TokenType::And},
        {"or", TokenType::Or},
        {"not", TokenType::Not},
        {"while", TokenType::While},
        {"if", TokenType::If},
        {"elif", TokenType::Elif},
        {"else", TokenType::Else},
        {"def", TokenType::Def}, 
        {"return", TokenType::Return},
        {"break", TokenType::Break},
        {"continue", TokenType::Continue},
        {"pass", TokenType::Pass},
        {"class", TokenType::Class}
    };
}

std::vector<Token> Lexer::scan_tokens() {

    is_block = false;
    indent_levels.push(0); // Initial indentation level
    tokens.clear();
    
    handle_indentation(); // at the beginning
    
    while (!is_at_end()) {
        start = current;
        scan_token();
    }
    tokens.push_back(Token(TokenType::EndOfFile, "", line));
    
    return tokens;
}

void Lexer::scan_token() {
    
    char c = advance();
   
    switch (c) {
        case ':':
            add_token(TokenType::Colon);         
            break;
        case '(':
            add_token(TokenType::LeftParen);
            break;
        case ')':
            add_token(TokenType::RightParen);
            break;
        case '{':
            add_token(TokenType::LeftBrace);
            is_block = true;
            break;
        case '}':
            add_token(TokenType::RightBrace);
            is_block = false;
            break;
        case ',':
            add_token(TokenType::Comma);
            break;
        case '.':
            add_token(TokenType::Dot);
            break;
        case '-':
            add_token(match('=') ? TokenType::MinusEqual : TokenType::Minus);
            break;
        case '+':
            add_token(match('=') ? TokenType::PlusEqual : TokenType::Plus);
            break;
        case ';':
            add_token(TokenType::Semicolon);
            break;
        case '*':
            add_token(match('*') ? TokenType::DoubleStar : 
                     match('=') ? TokenType::StarEqual : TokenType::Star);
            break;
        case '/':
            add_token(match('/') ? TokenType::DoubleSlash : 
                     match('=') ? TokenType::SlashEqual : TokenType::Slash);
            break;
        case '%':
            add_token(match('=') ? TokenType::ModEqual : TokenType::Mod);
            break;
        case '|':
            add_token(match('=') ? TokenType::OrEqual : TokenType::Pipe);
            break;
        case '&':
            add_token(match('=') ? TokenType::AndEqual : TokenType::Ampersand);
            break;
        case '^':
            add_token(match('=') ? TokenType::XorEqual : TokenType::Caret);
            break;
        case '~':
            add_token(TokenType::Tilde);
            break;
        case '!':
            add_token(match('=') ? TokenType::BangEqual : TokenType::Bang);
            break;
        case '=':
            add_token(match('=') ? TokenType::EqualEqual : TokenType::Equals);
            break;
        case '<':
            add_token(match('<') ? 
                        (match('=') ? TokenType::LeftShiftEqual : TokenType::LeftShift) : 
                     match('=') ? TokenType::LessEqual : TokenType::Less);
            break;
        case '>':
            add_token(match('>') ? 
                        (match('=') ? TokenType::RightShiftEqual : TokenType::RightShift) : 
                     match('=') ? TokenType::GreaterEqual : TokenType::Greater);
            break;
         case '#':
            while (peek() != '\n' && !is_at_end()) advance();
            break;    
        case ' ':
        case '\t':
        case '\r':
            // ignore whitespaces
            break;
        case '\n':
            line++;
            add_token(TokenType::Newline);
            start = current;
            handle_indentation();
            break;
        case '\"':
        case '\'':
            handle_string(c);
            break;
        default:
            if (isdigit(c)) {
                handle_number();
            } else if (isalpha(c) or c == '_') {
                handle_identifier();
            } else {
                throw SyntaxError("Unexpected character.");
            }
            break;
    }
}

void Lexer::handle_number() {
    /*
     *    number ::= integer | floatnumber
     *    integer ::= decimalinteger
     *                | octinteger | hexinteger  // TODO
    */
    while (isdigit(peek())) advance();    
    
    bool has_decimal = false;
    if (peek() == '.') {
        has_decimal = true;
        do {
          advance();
        } while (isdigit(peek()));
    }
    add_token(has_decimal ? TokenType::Float : TokenType::Int);
}

void Lexer::handle_identifier() {
    /*
     *   identifier ::= (letter | "_") (letter | digit | "_")*
     *   letter ::= lowercase | uppercase
     *   lowercase ::= "a" | "b" | ... | "z"
     *   uppercase ::= "A" | "B" | ... | "Z"
     *   digit ::= "0" | "1" | ... | "9"
     *
     *  (where the matched string is not a keyword)
    */
    char c;
    while (c = peek(), isalnum(c) or c == '_') advance();

    std::string text = source.substr(start, current - start);
    TokenType type = keywords.count(text) ? keywords.at(text) : TokenType::Name;

    // Check if the identifier indicates the start of a block
    switch(type) {
        case TokenType::If:
        case TokenType::Elif:
        case TokenType::Else:
        case TokenType::While:
        case TokenType::Def: 
        case TokenType::Class: { 
            is_block = true; 
            break;
        }
        default: break;
    }
    add_token(type);
}

void Lexer::handle_string(char quote_type){
    /*
     *  string ::= "'" <any source character except "'" or newline> "'"
     *             | '"' <any source character except '"' or newline> '"'
     * // TODO: add escape sequences and multi-line strings
    */
    char c;
    while(c = peek(), c != quote_type && c != '\n') advance();
    if(peek() == '\n') {
        throw SyntaxError("Unterminated string at line " + std::to_string(line));
    }
    advance(); // Consume the closing quote
    
    // Remove the surrounding quotes and add the token
    std::string value = start < current - 2 ? 
                        source.substr(start + 1, current - start - 2) : "";
    
    add_token(TokenType::String, value);
}

bool Lexer::match(char expected) {

    if (is_at_end() || source[current] != expected) {
        return false;
    } else {
        ++current;
        return true;
    }
}

char Lexer::peek() const {

    if (is_at_end()) {
        return '\0';
    }
    return source[current];
}

char Lexer::peek_next() const {
    if (current + 1 >= source.length()) {
        return '\0';
    }
    return source[current + 1];
}

char Lexer::advance() {
    return source[current++];
}

bool Lexer::is_at_end() const {
    return current >= source.length();
}

void Lexer::add_token(TokenType type) {
    add_token(type, "");
}

void Lexer::add_token(TokenType type, const std::string& lexeme) {
    std::string text = (type == TokenType::String) ? 
                        lexeme : source.substr(start, current - start);
    tokens.push_back(Token(type, text, line));
}

bool is_whitespace(char c) {
    return c == ' ' or c == '\t';
}

void Lexer::handle_indentation() {
    
    int indent = 0;
    while (is_whitespace(peek())) {
        indent++;
        advance();
    }
    if (peek() == '\n') {
        // Only whitespaces, ignore the line
        return;
    }
    if(indent and not is_block) {
        throw SyntaxError("Unexpected indentation at line " + std::to_string(line));
    }
    int previous_indent_level = indent_levels.top();
    
    if(indent > previous_indent_level) {
        indent_levels.push(indent);
        add_token(TokenType::Indent);
    } else if(indent < previous_indent_level){
        while(not indent_levels.empty() and indent < indent_levels.top()) {
            add_token(TokenType::Dedent);
            indent_levels.pop();
        }
        if (indent_levels.empty() or indent != indent_levels.top()) {
            throw SyntaxError("Inconsistent indentation at line " + std::to_string(line));
        }
    } /*else if(indent == previous_indent_level) {
        // dont generate Indent token
    }*/
    if(indent_levels.top() == 0) is_block = false;
}


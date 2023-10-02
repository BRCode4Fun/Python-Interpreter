#include <stdexcept>
#include <iostream>
#include <fstream>
#include "./lexer.hpp"

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

std::vector<Token> Lexer::scanTokens() {

    isBlock = false;
    indentLevels.push(0); // Initial indentation level
    tokens.clear();
    
    handleIndentation(); // at the beginning
    
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back(Token(TokenType::EndOfFile, "", line));
    
    return tokens;
}

void Lexer::scanToken() {
    
    char c = advance();
   
    switch (c) {
        case ':':
            addToken(TokenType::Colon);         
            break;
        case '(':
            addToken(TokenType::LeftParen);
            break;
        case ')':
            addToken(TokenType::RightParen);
            break;
        case '{':
            addToken(TokenType::LeftBrace);
            isBlock = true;
            break;
        case '}':
            addToken(TokenType::RightBrace);
            isBlock = false;
            break;
        case ',':
            addToken(TokenType::Comma);
            break;
        case '.':
            addToken(TokenType::Dot);
            break;
        case '-':
            addToken(match('=') ? TokenType::MinusEqual : TokenType::Minus);
            break;
        case '+':
            addToken(match('=') ? TokenType::PlusEqual : TokenType::Plus);
            break;
        case ';':
            addToken(TokenType::Semicolon);
            break;
        case '*':
            addToken(match('*') ? TokenType::DoubleStar : 
                     match('=') ? TokenType::StarEqual : TokenType::Star);
            break;
        case '/':
            addToken(match('/') ? TokenType::DoubleSlash : 
                     match('=') ? TokenType::SlashEqual : TokenType::Slash);
            break;
        case '%':
            addToken(match('=') ? TokenType::ModEqual : TokenType::Mod);
            break;
        case '|':
            addToken(match('=') ? TokenType::OrEqual : TokenType::Pipe);
            break;
        case '&':
            addToken(match('=') ? TokenType::AndEqual : TokenType::Ampersand);
            break;
        case '^':
            addToken(match('=') ? TokenType::XorEqual : TokenType::Caret);
            break;
        case '~':
            addToken(TokenType::Tilde);
            break;
        case '!':
            addToken(match('=') ? TokenType::BangEqual : TokenType::Bang);
            break;
        case '=':
            addToken(match('=') ? TokenType::EqualEqual : TokenType::Equals);
            break;
        case '<':
            addToken(match('<') ? 
                        (match('=') ? TokenType::LeftShiftEqual : TokenType::LeftShift) : 
                     match('=') ? TokenType::LessEqual : TokenType::Less);
            break;
        case '>':
            addToken(match('>') ? 
                        (match('=') ? TokenType::RightShiftEqual : TokenType::RightShift) : 
                     match('=') ? TokenType::GreaterEqual : TokenType::Greater);
            break;
         case '#':
            while (peek() != '\n' && !isAtEnd()) advance();
            break;    
        case ' ':
        case '\t':
        case '\r':
            // ignore whitespaces
            break;
        case '\n':
            line++;
            addToken(TokenType::Newline);
            start = current;
            handleIndentation();
            break;
        case '\"':
        case '\'':
            handleString(c);
            break;
        default:
            if (isdigit(c)) {
                handleNumber();
            } else if (isalpha(c) or c == '_') {
                handleIdentifier();
            } else {
                throw std::runtime_error("Unexpected character.");
            }
            break;
    }
}

void Lexer::handleNumber() {
    /*
     *    number ::= integer | floatnumber
     *    integer ::= decimalinteger
     *                | octinteger | hexinteger  // TODO
    */
    while (isdigit(peek())) advance();    
    
    bool hasDecimal = false;
    if (peek() == '.') {
        hasDecimal = true;
        do {
          advance();
        } while (isdigit(peek()));
    }
    addToken(hasDecimal ? TokenType::Float : TokenType::Int);
}

void Lexer::handleIdentifier() {
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
            isBlock = true; 
            break;
        }
        default: break;
    }
    addToken(type);
}

void Lexer::handleString(char quoteType){
    /*
     *  string ::= "'" <any source character except "'" or newline> "'"
     *             | '"' <any source character except '"' or newline> '"'
     * // TODO: add escape sequences and multi-line strings
    */
    char c;
    while(c = peek(), c != quoteType && c != '\n') advance();
    if(peek() == '\n') {
        throw std::runtime_error("Unterminated string at line " + std::to_string(line));
    }
    advance(); // Consume the closing quote
    
    // Remove the surrounding quotes and add the token
    std::string value = start < current - 2 ? 
                        source.substr(start + 1, current - start - 2) : "";
    
    addToken(TokenType::String, value);
}

bool Lexer::match(char expected) {

    if (isAtEnd() || source[current] != expected) {
        return false;
    } else {
        ++current;
        return true;
    }
}

char Lexer::peek() const {

    if (isAtEnd()) {
        return '\0';
    }
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= source.length()) {
        return '\0';
    }
    return source[current + 1];
}

char Lexer::advance() {
    return source[current++];
}

bool Lexer::isAtEnd() const {
    return current >= source.length();
}

void Lexer::addToken(TokenType type) {
    addToken(type, "");
}

void Lexer::addToken(TokenType type, const std::string& lexeme) {
    std::string text = (type == TokenType::String) ? 
                        lexeme : source.substr(start, current - start);
    tokens.push_back(Token(type, text, line));
}

bool isWhitespace(char c) {
    return c == ' ' or c == '\t';
}

void Lexer::handleIndentation() {
    
    int indent = 0;
    while (isWhitespace(peek())) {
        indent++;
        advance();
    }
    if (peek() == '\n') {
        // Only whitespaces, ignore the line
        return;
    }
    if(indent and not isBlock) {
        throw std::runtime_error("Unexpected indentation at line " + std::to_string(line));
    }
    int prevIndentLevel = indentLevels.top();
    
    if(indent > prevIndentLevel) {
        indentLevels.push(indent);
        addToken(TokenType::Indent);
    } else if(indent < prevIndentLevel){
        while(not indentLevels.empty() and indent < indentLevels.top()) {
            addToken(TokenType::Dedent);
            indentLevels.pop();
        }
        if (indentLevels.empty() or indent != indentLevels.top()) {
            throw std::runtime_error("Inconsistent indentation at line " + std::to_string(line));
        }
    } /*else if(indent == prevIndentLevel) {
        // dont generate Indent token
    }*/
    if(indentLevels.top() == 0) isBlock = false;
}


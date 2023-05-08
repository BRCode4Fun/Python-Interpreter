#include <cctype>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "./lexer.hpp"

Lexer::Lexer(const std::string& source) : source(source), current(0), line(1), currentIndentLevel(0) {
    keywords.insert(std::make_pair("print", TokenType::Print));
}

std::vector<Token> Lexer::scanTokens() {

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
          //  currentIndentLevel++;
           // addToken(TokenType::Colon);
            break;
        case '(':
            addToken(TokenType::LeftParen);
            break;
        case ')':
            addToken(TokenType::RightParen);
            break;
        case '{':
            addToken(TokenType::LeftBrace);
            currentIndentLevel++;
            break;
        case '}':
            addToken(TokenType::RightBrace);
            currentIndentLevel--;
            break;
        case ',':
            addToken(TokenType::Comma);
            break;
        case '.':
            addToken(TokenType::Dot);
            break;
        case '-':
            addToken(TokenType::Minus);
            break;
        case '+':
            addToken(TokenType::Plus);
            break;
        case ';':
            addToken(TokenType::Semicolon);
            break;
        case '*':
            std::cout << "f" << std::endl; 
            addToken(TokenType::Star);
            break;
        case '!':
            addToken(match('=') ? TokenType::BangEqual : TokenType::Bang);
            break;
        case '=':
            addToken(match('=') ? TokenType::EqualEqual : TokenType::Equals);
            break;
        case '<':
            addToken(match('=') ? TokenType::LessEqual : TokenType::Less);
            break;
        case '>':
            addToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
            break;
        case '/':
            if (match('/')) {
                // A comment goes until the end of the line
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
            } else {
                addToken(TokenType::Slash);
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace
            break;
        case '\n':
            line++;
            handleNewline();
            break;
        default:
            if (isdigit(c)) {
                handleNumber();
            } else if (isalpha(c)) {
                handleIdentifier();
            } else {
                throw std::runtime_error("Unexpected character.");
            }
            break;
    }
}

void Lexer::handleNewline() {
  
    char c = peek();

    if (c == '\n') return;

    if (currentIndentLevel > 0) {
      
        int prevIndentLevel = indentLevels.empty() ? 0 : indentLevels.top();
        if (currentIndentLevel > prevIndentLevel) {
        
            indentLevels.push(currentIndentLevel);
            addToken(TokenType::Indent);
        } else if (currentIndentLevel < prevIndentLevel) {
            
            while (!indentLevels.empty() && currentIndentLevel < indentLevels.top()) {
                indentLevels.pop();
                addToken(TokenType::Dedent);
            }

            if (indentLevels.empty() || currentIndentLevel != indentLevels.top()) {
                throw std::runtime_error("Invalid indentation level.");
            } 
        } else {
        
        }
    } else if (c != '\n') {
      indentLevels.push(0);
      addToken(TokenType::Indent);
    }
}


void Lexer::handleNumber() {
   
    while (isdigit(peek())) advance();    
    
    if (peek() == '.' && isdigit(peekNext())) {
        do {
          advance();
        } while (isdigit(peek()));
    }
    addToken(TokenType::Number);
}

void Lexer::handleIdentifier() {
    
    while (isalnum(peek())) advance();

    std::string text = source.substr(start, current - start);
    TokenType type = keywords.count(text) ? keywords.at(text) : TokenType::Identifier;
    addToken(type);
}

bool Lexer::match(char expected)
{
    if (isAtEnd() || source[current] != expected) {
        return false;
    } else {
        ++current;
        return true;
    }
}

char Lexer::peek() const {

    if (isAtEnd()) return '\0';

    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= source.length()) return '\0';

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
    std::string text = lexeme.empty() ? source.substr(start, current - start) : lexeme;
    tokens.push_back(Token(type, text, line));
}

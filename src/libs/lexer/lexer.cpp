#include <cctype>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "./lexer.hpp"

Lexer::Lexer(const std::string& source) : source(source), current(0), line(1), currentIndentLevel(0) {
    keywords.insert(std::make_pair("print", TokenType::Print));
    keywords.insert(std::make_pair("True", TokenType::True));
    keywords.insert(std::make_pair("False", TokenType::False));
    keywords.insert(std::make_pair("None", TokenType::None));
    keywords.insert(std::make_pair("and", TokenType::And));
    keywords.insert(std::make_pair("or", TokenType::Or));
    keywords.insert(std::make_pair("not", TokenType::Not));
    keywords.insert(std::make_pair("while", TokenType::While));
    keywords.insert(std::make_pair("if", TokenType::If));

    indentLevels.push(0);

    has_symbol = false;
}

std::vector<Token> Lexer::scanTokens() {

    start_newline = true; 
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
            addToken(TokenType::Colon);
             has_symbol = true;
            break;
        case '(':
            addToken(TokenType::LeftParen);
            has_symbol = true;
            break;
        case ')':
            addToken(TokenType::RightParen);
            has_symbol = true;
            break;
        case '{':
            addToken(TokenType::LeftBrace);
            has_symbol = true;
            currentIndentLevel++;
            break;
        case '}':
            addToken(TokenType::RightBrace);
            has_symbol = true;
            currentIndentLevel--;
            break;
        case ',':
            addToken(TokenType::Comma);
            has_symbol = true;
            break;
        case '.':
            addToken(TokenType::Dot);
            has_symbol = true;
            break;
        case '-':
            addToken(TokenType::Minus);
            has_symbol = true;
            break;
        case '+':
            addToken(TokenType::Plus);
            has_symbol = true;
            break;
        case ';':
            addToken(TokenType::Semicolon);
            has_symbol = true;
            break;
        case '*':
            addToken(TokenType::Star);
            has_symbol = true;
            break;
        case '!':
            addToken(match('=') ? TokenType::BangEqual : TokenType::Bang);
            has_symbol = true;
            break;
        case '=':
            addToken(match('=') ? TokenType::EqualEqual : TokenType::Equals);
            has_symbol = true;
            break;
        case '<':
            addToken(match('=') ? TokenType::LessEqual : TokenType::Less);
            has_symbol = true;
            break;
        case '>':
            addToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
            has_symbol = true;
            break;
        case '/':
            addToken(TokenType::Slash);
            has_symbol = true;
            break;
         case '#':
            
            while (peek() != '\n' && !isAtEnd()) advance();

            if(peek() == '\n'){ 
                line++;
                advance();
     //           break;
            }
            has_symbol =  false; 
            
            break;    
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace
            // currentIndentLevel++; 
           
            break;
        case '\n':
            if(has_symbol == false){
                line++;
                start_newline = true; 
                currentIndentLevel = 0; 
                handleNewline();
            } else {
                line++;
                start_newline = true; 
                currentIndentLevel = 0;
                addToken(TokenType::Indent); 
                handleNewline();
            }
            break;
        default:
            if (isdigit(c)) {
                handleNumber();
            } else if (isalpha(c)) {
                handleIdentifier();
            } else {
                throw std::runtime_error("Unexpected character.");
            }
            has_symbol = true; 
            break;
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
    
    while (isalnum(peek()) or peek() == '_') advance();

    std::string text = source.substr(start, current - start);
    TokenType type = keywords.count(text) ? keywords.at(text) : TokenType::Identifier;

    if(text == "if")
         is_block = true;
    if(text == "while")
         is_block = true;
 

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

void Lexer::handleNewline() {
  
    char c = peek();

    while ((c = peek()) == ' ' or (c = peek()) == '\n'){

        if((c = peek()) == '\n'){
            line++;
            advance(); 
            currentIndentLevel = 0; 
            continue; 
        }

        advance(); 
        currentIndentLevel++; 
    }
    
    if (is_block and currentIndentLevel > indentLevels.top() ) {
       
         indentLevels.push(currentIndentLevel);
         is_block = false;

    } else if ( !indentLevels.empty()  and  currentIndentLevel < indentLevels.top() ) {

        while (indentLevels.top() !=  0 and currentIndentLevel < indentLevels.top() ) {
            addToken(TokenType::Dedent);
            indentLevels.pop();
        }
    }

    if(currentIndentLevel > indentLevels.top()){
        std::string msg  = "Unexpected identation at line  " + std::to_string(line); 
        throw std::runtime_error(msg);
    }
    if (c == '\n') return;
}

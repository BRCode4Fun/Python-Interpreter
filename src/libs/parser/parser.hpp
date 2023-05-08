#pragma once

#include <vector>
#include <string>
#include "../token/token.hpp"
#include "../ast/ast.hpp"

class Parser {
    public:
        Parser(const std::vector<Token>& tokens);
        ProgramNode* parse();
        ProgramNode* parseProgram();

    private:
        const std::vector<Token>& tokens;
        int current = 0;

        Token consume(TokenType type);
        bool match(TokenType type);
        bool isAtEnd() const;
        Token peek() const;
        Token previous() const;
        void error(const std::string& message);

        AstNode* parseEqual();
        AstNode* parseExpr();
        AstNode* parsePrimary();
        AstNode* parseCall();
        AstNode* parseUnary();
        AstNode* parsePrint();
        AstNode* parseTerm();
        AstNode* parseFactor();
        AstNode* parseStmt();
};

#pragma once

#include <vector>
#include <string>
#include "../token/token.hpp"
#include "../ast/ast.hpp"

class Parser {
    public:
        Parser(const std::vector<Token>& tokens);
        ProgramNode* parse();

    private:
        const std::vector<Token>& tokens;
        int current = 0;

        Token consume(TokenType type);
        bool match(TokenType type);
        bool isAtEnd() const;
        Token peek() const;
        Token previous() const;
        void error(const std::string& message);

        ProgramNode* parseProgram();
        AstNode* parseStmt();
        AstNode* parseStmtList();
        AstNode* parseSimpleStmt();
        AstNode* parsePrintStmt();
        AstNode* parseSuite();
        AstNode* parseIfStmt();
        AstNode* parseWhileStmt();
        AstNode* parseAssign();
        AstNode* parseExpr();
        AstNode* parseTernary();
        AstNode* parseLogicOr();
        AstNode* parseLogicAnd();
        AstNode* parseEquality();
        AstNode* parseRelational();
        AstNode* parseFactor();
        AstNode* parseTerm();
        AstNode* parseUnary();
        AstNode* parseCall();
        AstNode* parsePrimary();
};

#pragma once

#include <vector>
#include <string>
#include <initializer_list>
#include "../token/token.hpp"
#include "../ast/ast.hpp"

using namespace std;

class Parser {
    public:
        Parser(const vector<Token>& tokens);
        ProgramNode* parse();

    private:
        const vector<Token>& tokens;
        int current = 0;

        Token consume(TokenType type);
        bool match(TokenType type);
        bool match(initializer_list<TokenType> types);
        bool isAtEnd() const;
        Token peek() const;
        Token previous() const;
        void error(const string& message);

        ProgramNode* parseProgram();
        AstNode* parseStmt();
        vector<AstNode*> parseStmtList();
        AstNode* parseSimpleStmt();
        AstNode* parsePrintStmt();
        AstNode* parseSuite();
        AstNode* parseIfStmt();
        AstNode* parseWhileStmt();
        AstNode* parseReturnStmt(); 
        AstNode* parseAssign();
        AstNode* parseExpr();
        AstNode* parseConditionalExpr();
        AstNode* parseDisjunction();
        AstNode* parseConjunction();
        AstNode* parseInversion();
        AstNode* parseComparison();
        AstNode* parseBitwiseOr();
        AstNode* parseBitwiseXor();
        AstNode* parseBitwiseAnd();
        AstNode* parseShiftExpr();
        AstNode* parseFactor();
        AstNode* parseTerm();
        AstNode* parsePower();
        AstNode* parseUnary();
        AstNode* parseCall(AstNode *func_name);
        AstNode* parsePrimary();
        
        vector<AstNode*> parseParameterList();
        AstNode* parseFunctionDef();
        
};

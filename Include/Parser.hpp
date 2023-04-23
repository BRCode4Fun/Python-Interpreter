

#include <vector>
#include <string>
#include "Token.hpp"
#include "Ast.hpp"

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

    AstNode* parseExpr();
    AstNode* parsePrimary();
    AstNode* parseCall();
    AstNode* parseUnary();
    AstNode* parsePrint();
    AstNode* parseTerm();
    AstNode* parseFactor();
    AstNode* parseStmt();
    
};
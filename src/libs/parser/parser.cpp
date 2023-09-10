#include "parser.hpp"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

ProgramNode* Parser::parse() {
    return parseProgram();
}

ProgramNode* Parser::parseProgram() {
    /*
     *   program ::= (statement | NEWLINE)* $
    */
    std::vector<AstNode*> statements;
    while (!isAtEnd()) {
        if(match(TokenType::Newline)){
            continue;
        } else {
            statements.push_back(parseStmt());
        }
    }
    BlockNode* block = new BlockNode(statements);
    return new ProgramNode(block);
}

AstNode* Parser::parseStmt() {
    /*
     *    statement ::= stmt_list NEWLINE
     *                  | compound_stmt
     *
     *    compound_stmt ::= if_stmt
     *                      | while_stmt
     *                      | for_stmt      TODO
     *                      | try_stmt      TODO
     *                      | with_stmt     TODO
     *                      | funcdef       
     *                      | classdef      TODO
    */
    if (match(TokenType::If)){
        return parseIfStmt();
        
    } else if (match(TokenType::While)){
        return parseWhileStmt();
        
    } else if (match(TokenType::Def)){
        return parseFunctionDef();    

    } else {
        std::vector<AstNode*> stmtList = parseStmtList();
        while(match(TokenType::Newline)) continue;
        return new BlockNode(stmtList);
    }
}

std::vector<AstNode*> Parser::parseParameterList() {
    /*
     *   parameter_list ::= identifier ("," identifier)*    // keep simple for now
    */
    std::vector<AstNode*> parameters;
    do {
        Token tk = consume(TokenType::Name);
        AstNode* pname = new NameNode(tk);
        parameters.push_back(pname);      
    } while (match(TokenType::Comma));
    
    return parameters;
}

AstNode* Parser::parseFunctionDef(){
    /*
     *   funcdef ::= "def" funcname "(" (parameter_list)? ")" ":" suite
     *   funcname ::= identifier
    */
    Token fname = consume(TokenType::Name);
    consume(TokenType::LeftParen);

    std::vector<AstNode*> parameters;
    
    if (peek().type != TokenType::RightParen) {
       parameters = parseParameterList();   
    }
    consume(TokenType::RightParen);
    consume(TokenType::Colon);

    isInsideFunc = true;
    AstNode* body = parseSuite();
    isInsideFunc = false;

    return new FunctionNode(fname, parameters, body);
}

std::vector<AstNode*> Parser::parseStmtList() {
    /*
     *    stmt_list ::= simple_stmt (";" simple_stmt)* (";")?
    */
    std::vector<AstNode*> stmts;
    
    stmts.push_back(parseSimpleStmt());
    
    while(match(TokenType::Semicolon)){
        if(peek().type == TokenType::Newline) break;
        stmts.push_back(parseSimpleStmt());
    }
    if(peek().type == TokenType::Semicolon) ++current;
    
    return stmts;
}

AstNode* Parser::parseSimpleStmt() {
    /*
     *  simple_stmt ::= assignment_stmt
     *                   | assert_stmt     TODO
     *                   | pass_stmt       TODO
     *                   | del_stmt        TODO
     *                   | print_stmt      
     *                   | return_stmt     
     *                   | yield_stmt      TODO
     *                   | raise_stmt      TODO
     *                   | break_stmt      
     *                   | continue_stmt   
     *                   | import_stmt     TODO
     *                   | global_stmt     TODO
     *
    */
     if (match(TokenType::Print)) {
        return parsePrintStmt();
    } else if(match(TokenType::Return)){
        return parseReturnStmt();
    } else if(match(TokenType::Break)){
        return parseBreakStmt();
    } else if(match(TokenType::Continue)){
        return parseContinueStmt();
    } else {
        return parseAssign();
    }
}

AstNode* Parser::parseReturnStmt() {
    /*
     *  return_stmt ::= "return" (expression)?
    */
    if(!isInsideFunc) error("Return outside Function.");
    
    Token keyword = previous();
    AstNode* expr = match({TokenType::Newline, TokenType::Semicolon}) ? 
                    nullptr : parseExpr();
    return new ReturnNode(keyword, expr);
}

AstNode* Parser::parseBreakStmt() {
    /*
     *  break_stmt ::= "break"
    */
    if(!isInsideLoop) error("Break outside Loop.");
    
    Token keyword = previous();
    return new BreakNode(keyword);
}

AstNode* Parser::parseContinueStmt() {
    /*
     *  continue_stmt ::= "continue"
    */
    if(!isInsideLoop) error("Continue outside Loop.");
    
    Token keyword = previous();
    return new ContinueNode(keyword);
}

AstNode* Parser::parsePrintStmt() {
    /*
     *    print_stmt ::= "print" "(" (expression)? ")"
    */
    consume(TokenType::LeftParen);
    AstNode* expr = nullptr;
    if(peek().type != TokenType::RightParen) {
        expr = parseExpr();
    } consume(TokenType::RightParen);

    return new PrintNode(expr);
}

AstNode* Parser::parseSuite() {
    /*
     *    suite ::= NEWLINE* INDENT statement+ DEDENT
     *              | stmt_list NEWLINE
    */
    if(match(TokenType::Newline)) {
    
        while(match(TokenType::Newline)) continue;

        consume(TokenType::Indent);
        
        std::vector<AstNode*> stmts;
    
        //stmts.push_back(parseStmt());
    
        while(peek().type != TokenType::Dedent){

             auto r = parseStmt();

             if(dynamic_cast<BlockNode*>(r) != nullptr){
                 
                BlockNode* stmt_list =  dynamic_cast<BlockNode*>(r);

                for(auto stmt : stmt_list->statements)
                     stmts.push_back(stmt);  
                                  
             } else {
                 stmts.push_back(r);       
             }
        }
        consume(TokenType::Dedent);
        
        return new BlockNode(stmts);

    } else {
        std::vector<AstNode*> stmtList = parseStmtList();
        consume(TokenType::Newline);
        return new BlockNode(stmtList);
    }
}

AstNode* Parser::parseIfStmt() {
    /*
     *    if_stmt ::= "if" expression ":" suite
     *                ("elif" expression ":" suite)*
     *                ("else" ":" suite)?
    */
    AstNode* cond = parseExpr();
    consume(TokenType::Colon);
    AstNode* trueBranch = parseSuite();

    std::vector<std::pair<AstNode*, AstNode*>> elifBranches;
    while(match(TokenType::Elif)) {
        AstNode* elifCond = parseExpr();
        consume(TokenType::Colon);
        AstNode* elifSuite = parseSuite();
        elifBranches.emplace_back(elifCond, elifSuite);
    }
    
    AstNode* elseBranch = nullptr;
    if(match(TokenType::Else)) {
        consume(TokenType::Colon);
        elseBranch = parseSuite();
    }
    
    return new IfNode(cond, trueBranch, elifBranches, elseBranch);
}

AstNode* Parser::parseWhileStmt() {
    /*
     *   while_stmt ::= "while" expression ":" suite
     *                  ("else" ":" suite)?  //TODO
    */
    isInsideLoop = true;
    
    AstNode* cond = parseExpr();
    consume(TokenType::Colon);
    AstNode* body = parseSuite();

    isInsideLoop = false;

    return new WhileNode(cond, body);
}

AstNode* Parser::parseAssign() {
    /*
     *  assign_stmt ::= expression ("=" assign_stmt)*
    */
    AstNode* left = parseExpr();
    
    while (match(TokenType::Equals)) {
        //auto op = previous();
        AstNode* right = parseAssign();
        if(!left->is_name_node())
            error("cannot assign to expression");
        left = new AssignNode(left, right);
    }
    return left;
}

AstNode* Parser::parseExpr() {
    /*
     *  expression ::= conditional_expression 
     *                 | lambda_form // TODO
    */
    return parseConditionalExpr();
}

AstNode* Parser::parseConditionalExpr() {
    /*
     *  conditional_expression ::= disjunction ("if" disjunction "else" expression)?
    */
    AstNode* left = parseDisjunction();
    
    if(match(TokenType::If)) {
        AstNode* condition = parseDisjunction();
        consume(TokenType::Else);
        AstNode* right = parseExpr();
        left = new TernaryOpNode(condition, left, right);
    }
    return left;
}

AstNode* Parser::parseDisjunction() {
    /*
     *  disjunction ::= conjunction ("or" conjunction)*
    */
    AstNode* left = parseConjunction();
    
    while (match(TokenType::Or)) {
        Token op = previous();
        AstNode* right = parseConjunction();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseConjunction() {
    /*
     *  conjunction ::= inversion ("and" inversion)*
    */
    AstNode* left = parseInversion();
    
    while (match(TokenType::And)) {
        Token op = previous();
        AstNode* right = parseInversion();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseInversion() {
    /*
     *   inversion ::= "not" inversion
     *                | comparison
    */
    if (match(TokenType::Not)) {
        Token op = previous();
        AstNode* right = parseInversion();
        return new UnaryOpNode(op, right);
    } else {
        return parseComparison();
    }
}

AstNode* Parser::parseComparison() {
    /*
     *   comparison ::= factor (comp_operator factor)*
     *   comp_operator ::= "<" | ">" | "==" | ">=" | "<=" | "!="
                           | "is" ("not")? | ("not")? "in"  // TODO
    */
    AstNode* left = parseBitwiseOr();
    
    while (match({TokenType::Less, TokenType::Greater, 
                  TokenType::EqualEqual, TokenType::GreaterEqual, 
                  TokenType::LessEqual, TokenType::BangEqual})) {
        Token op = previous();
        AstNode* right = parseBitwiseOr();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseBitwiseOr() {
    /*  
     *    bitwise_or ::= bitwise_xor ("|" bitwise_xor)*
    */
    AstNode* left = parseBitwiseXor();
    
    while(match(TokenType::Pipe)) {
        Token op = previous();
        AstNode* right = parseBitwiseXor();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseBitwiseXor() {
    /*  
     *    bitwise_xor ::= bitwise_and ("^" bitwise_and)*
    */
    AstNode* left = parseBitwiseAnd();
    
    while(match(TokenType::Caret)) {
        Token op = previous();
        AstNode* right = parseBitwiseAnd();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseBitwiseAnd() {
    /*
     *   bitwise_and ::= shift_expr ("&" shift_expr)*
    */
    AstNode* left = parseShiftExpr();

    while (match(TokenType::Ampersand)) {
        Token op = previous();
        AstNode* right = parseShiftExpr();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseShiftExpr() {
    /*
     *   shift_expr ::= factor (("<<" | ">>") factor)*
    */
    AstNode* left = parseFactor();

    while (match({TokenType::LeftShift, TokenType::RightShift})) {
        Token op = previous();
        AstNode* right = parseFactor();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseFactor() {
    /*
     *   factor ::= term (("+" | "-") term)*
    */
    AstNode* left = parseTerm();
    
    while (match({TokenType::Plus, TokenType::Minus})) {
        Token op = previous();
        AstNode* right = parseTerm();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseTerm() {
    /*
     *   term ::= unary (("*" | "/" | "%") unary)*
    */
    AstNode* left = parseUnary();
    
    while (match({TokenType::Star, TokenType::Slash, TokenType::Mod})) {
        Token op = previous();
        AstNode* right = parseUnary();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

/*AstNode* Parser::parsePower() {
    /*
     *  power ::= unary ("**" power)*
    
    auto left = parseUnary();
    
    if (match(TokenType::DoubleStar)) {
        auto op = previous();
        auto right = parsePower();
        left = new BinaryOpNode(left, op.lexeme, right);
    }
    return left;
}*/

AstNode* Parser::parseUnary() {
    /*
     *   unary ::= (("+" | "-" | "~") unary) | primary
    */
    if (match({TokenType::Minus, TokenType::Tilde})) {
        Token op = previous();
        AstNode* right = parseUnary();
        return new UnaryOpNode(op, right);
        
    } else if (match(TokenType::Plus)) {
        return parseUnary();
        
    } else {
        return parsePrimary();
    }
}

AstNode* Parser::parseCall(AstNode* fname) {
    /* 
     *  call ::= primary "(" (argument_list)? ")"
    */
    //consume(TokenType::LeftParen);

    std::vector<AstNode*> args; 

    if(not match(TokenType::RightParen)) {
        do {
            args.push_back(parseExpr());    
        } while(match(TokenType::Comma));
        consume(TokenType::RightParen);
    }
    return new CallNode(fname, args);
}

AstNode* Parser::parsePrimary() {
    /*
     *   primary ::= atom | attributeref
     *               | subscription | slicing | call
     *   atom ::= identifier | literal
     *   literal ::= stringliteral | integer | floatnumber 
     *               | "None" | "True" | "False"
     *   attributeref ::= primary "." identifier             // TODO
     *   subscription ::= primary "[" expression_list "]"    // TODO
     *   slicing ::= simple_slicing | extended_slicing       // TODO
    */
    if (match(TokenType::LeftParen)) {
        AstNode* expr = parseExpr();
        consume(TokenType::RightParen);
        return expr;
    } else {
        Token tk = advance();
        
        switch(tk.type) {
            case TokenType::Int:
                return new IntNode(tk);
            case TokenType::Float:
                return new FloatNode(tk);
            case TokenType::True:
                return new BooleanNode(true);
            case TokenType::False:
                return new BooleanNode(false);
            case TokenType::None:
                return new NullNode(tk);
            case TokenType::String:
                return new StringNode(tk);
            case TokenType::Name: {
                AstNode* name = new NameNode(tk);
                return (match(TokenType::LeftParen)) ? 
                        parseCall(name) : name;
            } default: {
                error("Expected a primary expression");
                return nullptr; 
            }
        }
    }
}

Token Parser::consume(TokenType type) {
    if (match(type)) return previous();
    else {
        error("Expected " + std::to_string(type) + "\n");
       // cout << "But got  " <<  peek().type <<  endl;
        return Token(TokenType::Error, "", 0);
    }
}

bool Parser::match(TokenType type) {
    if (isAtEnd() || peek().type != type) {
        return false;
    } else {
        ++current;
        return true;
    }
}

bool Parser::match(std::initializer_list<TokenType> types) {
    
    if(isAtEnd()) return false;
    
    for(const auto& type : types) {
        if(peek().type == type) {
            ++current;
            return true;
        }
    }
    return false;
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[(current > 0) ? current - 1 : 0];
}

Token Parser::advance() {
    return tokens[current++];
}

void Parser::error(const std::string& message) {
    std::cerr << "Error at line " << peek().line << ": " << message << std::endl;
    exit(EXIT_FAILURE);
}

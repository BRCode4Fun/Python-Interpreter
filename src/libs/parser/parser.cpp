#include "parser.hpp"

Parser::Parser(const vector<Token>& tokens) : tokens(tokens) {}

ProgramNode* Parser::parse() {
    return parseProgram();
}

ProgramNode* Parser::parseProgram() {
    /*
     *   program ::= (statement | NEWLINE)* $
    */
    vector<AstNode*> statements;
    while (!isAtEnd()) {
        if(match(TokenType::Newline)){
            continue;
        } else {
            statements.push_back(parseStmt());
        }
    }
    return new ProgramNode(new BlockNode(statements));
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
     *                      | funcdef       TODO
     *                      | classdef      TODO
    */
    if (match(TokenType::If)){
        return parseIfStmt();
        
    } else if (match(TokenType::While)){
        return parseWhileStmt();
        
    } else if (match(TokenType::Def)){
        return parseFunctionDef();    

    } else {
        auto stmtList = parseStmtList();
        while(match(TokenType::Newline)) continue;
        return stmtList;
    }
}

vector<AstNode*> Parser::parseParameterList() {
    /*
     *   parameter_list ::= identifier ("," identifier)*    // keep simple for now
    */
    vector<AstNode*> parameters;
    do {
        parameters.push_back(new NameNode(consume(TokenType::Name).lexeme));      
    } while (match(TokenType::Comma));
    
    return parameters;
}

AstNode* Parser::parseFunctionDef(){
    /*
     *   funcdef ::= "def" funcname "(" (parameter_list)? ")" ":" suite
     *   funcname ::= identifier
    */
    AstNode* name = new NameNode(consume(TokenType::Name).lexeme);
    consume(TokenType::LeftParen);

    vector<AstNode*> parameters;
    if (peek().type != TokenType::RightParen) {
       parameters = parseParameterList();   
    }
    consume(TokenType::RightParen);
    consume(TokenType::Colon);

    AstNode* body = parseSuite();

    return new FunctionNode(name, parameters, body);
}

AstNode* Parser::parseStmtList() {
    /*
     *    stmt_list ::= simple_stmt (";" simple_stmt)* (";")?
    */
    vector<AstNode*> stmts;
    
    stmts.push_back(parseSimpleStmt());
    
    while(match(TokenType::Semicolon)){
        if(peek().type == TokenType::Newline) break;
        stmts.push_back(parseSimpleStmt());
    }
    if(peek().type == TokenType::Semicolon) ++current;
    
    return new BlockNode(stmts);
}

AstNode* Parser::parseSimpleStmt() {
    /*
     *  simple_stmt ::= assignment_stmt
     *                   | assert_stmt     TODO
     *                   | pass_stmt       TODO
     *                   | del_stmt        TODO
     *                   | print_stmt      
     *                   | return_stmt     TODO
     *                   | yield_stmt      TODO
     *                   | raise_stmt      TODO
     *                   | break_stmt      TODO
     *                   | continue_stmt   TODO
     *                   | import_stmt     TODO
     *                   | global_stmt     TODO
     *
    */
     if (match(TokenType::Print)) {
        return parsePrintStmt();
    }else if(match(TokenType::Return)){
        return parseReturnStmt();
    } else {
        return parseAssign();
    }
}

AstNode* Parser::parseReturnStmt() 
{
    /*
     *    
    */
    return new ReturnNode(parseExpr());
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
        
        vector<AstNode*> stmts;
    
        stmts.push_back(parseStmt());
    
        while(peek().type != TokenType::Dedent){
            stmts.push_back(parseStmt());
        }
        consume(TokenType::Dedent);
      
        return new BlockNode(stmts);
    } else {
        auto stmtList = parseStmtList();
        consume(TokenType::Newline);
        return stmtList;
    }
}

AstNode* Parser::parseIfStmt() {
    /*
     *    if_stmt ::= "if" expression ":" suite
     *                ("elif" expression ":" suite)*
     *                ("else" ":" suite)?
    */
    auto cond =  parseExpr();
    consume(TokenType::Colon);
    auto trueBranch = parseSuite();

    vector<pair<AstNode*, AstNode*> > elifBranches;
    while(match(TokenType::Elif)) {
        auto elifCond = parseExpr();
        consume(TokenType::Colon);
        auto elifSuite = parseSuite();
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
    auto cond =  parseExpr();
    consume(TokenType::Colon);
    auto body = parseSuite();

    return new WhileNode(cond, body);
}

AstNode* Parser::parseAssign() {
    /*
     *  assign_stmt ::= expression ("=" assign_stmt)*
    */
    auto left = parseExpr();
    
    while (match(TokenType::Equals)) {
        //auto op = previous();
        auto right = parseAssign();
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
    auto left = parseDisjunction();
    
    if(match(TokenType::If)) {
        auto condition = parseDisjunction();
        consume(TokenType::Else);
        auto right = parseExpr();
        left = new TernaryOpNode(condition, left, right);
    }
    return left;
}

AstNode* Parser::parseDisjunction() {
    /*
     *  disjunction ::= conjunction ("or" conjunction)*
    */
    auto left = parseConjunction();
    
    while (match(TokenType::Or)) {
        auto op = previous();
        auto right = parseConjunction();
        left = new BinaryOpNode(left, op.lexeme, right);
    }
    return left;
}

AstNode* Parser::parseConjunction() {
    /*
     *  conjunction ::= inversion ("and" inversion)*
    */
    auto left = parseInversion();
    
    while (match(TokenType::And)) {
        auto op = previous();
        auto right = parseInversion();
        left = new BinaryOpNode(left, op.lexeme, right);
    }
    return left;
}

AstNode* Parser::parseInversion() {
    /*
     *   inversion ::= "not" inversion
     *                | comparison
    */
    if (match(TokenType::Not)) {
        auto op = previous();
        auto right = parseInversion();
        return new UnaryOpNode(op.lexeme, right);
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
    auto left = parseFactor(); //auto left = parseBitwiseOr();
    
    while (match({TokenType::Less, TokenType::Greater, 
                  TokenType::EqualEqual, TokenType::GreaterEqual, 
                  TokenType::LessEqual, TokenType::BangEqual})) {
        auto op = previous();
        auto right = parseFactor(); //auto right = parseBitwiseOr();
        left = new BinaryOpNode(left, op.lexeme, right);
    }
    return left;
}

/*AstNode* Parser::parseBitwiseOr() {
    // bitwise_or ::= bitwise_or "|" bitwise_xor | bitwise_xor
}

AstNode* Parser::parseBitwiseXor() {
    // bitwise_xor ::= bitwise_xor "^" bitwise_and | bitwise_and
}

AstNode* Parser::parseBitwiseAnd() {
    // bitwise_and ::= bitwise_and "&" shift_expr | shift_expr
}

AstNode* Parser::parseShiftExpr() {
    // shift_expr ::= shift_expr "<<" factor
    //                | shift_expr ">>" factor
    //                | factor
}*/

AstNode* Parser::parseFactor() {
    /*
     *   factor ::= term (("+" | "-") term)*
    */
    auto left = parseTerm();
    
    while (match({TokenType::Plus, TokenType::Minus})) {
        auto op = previous();
        auto right = parseTerm();
        left = new BinaryOpNode(left, op.lexeme, right);
    }
    return left;
}

AstNode* Parser::parseTerm() {
    /*
     *   term ::= unary (("*" | "/" | "%") unary)*
    */
    auto left = parseUnary();
    
    while (match({TokenType::Star, TokenType::Slash, TokenType::Mod})) {
        auto op = previous();
        auto right = parseUnary();
        left = new BinaryOpNode(left, op.lexeme, right);
    }
    return left;
}

AstNode* Parser::parseUnary() {
    /*
     *   unary ::= (("-" | "+") unary*) | primary
    */
    if (match(TokenType::Minus)) {
        auto op = previous();
        auto right = parseUnary();
        return new UnaryOpNode(op.lexeme, right);

    } else if (match(TokenType::Plus)) {
        return parseUnary();
    
    } else {
        return parsePrimary();
    }
}

/*AstNode** Parser::parsePower() {
    //  power ::= primary ("**" unary)
}*/

AstNode* Parser::parseCall(AstNode *func_name) {
    
     //*   call ::= primary "(" (argument_list)? ")"
    

    consume(TokenType::LeftParen);

    vector<AstNode*> args; 

    while (true) 
    {
 
        args.push_back(parseExpr());

        if(match(TokenType::RightParen))
        {
             break; 
        }

        match(TokenType::Comma);
        
       
    }
       
    

    
    
    return new CallNode(func_name, args);
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
    if (match(TokenType::Int)) {
        return new IntNode(stoll(previous().lexeme));
        
    } else if (match(TokenType::Float)) {
        return new FloatNode(stod(previous().lexeme));
        
    } else if(match(TokenType::True)) {
        return new BooleanNode(true);

    } else if(match(TokenType::False)) {
        return new BooleanNode(false);

    } else if(match(TokenType::None)) {
        return new NullNode();

    } else if (match(TokenType::Name)) {

        if(peek().type == TokenType::LeftParen){
         
            
            AstNode *name_function = new NameNode(previous().lexeme);
            return parseCall(name_function); 
        }

        return new NameNode(previous().lexeme);

    } else if (match(TokenType::String)) {
        return new StringNode(previous().lexeme);

    } else if (match(TokenType::LeftParen)) {
        auto expr = parseExpr();
        consume(TokenType::RightParen);
        return expr;
    } else {
        error("Expected a primary expression");
        return nullptr;
    }
}

Token Parser::consume(TokenType type) {
    if (match(type)) return previous();
    else {
        error("Expected " + to_string(type) + "\n");

       // cout << "But got  " <<  peek().type <<  endl; 
    
        return Token(TokenType::Error, "", 0);
    }
}

bool Parser::match(TokenType type) {
    if (isAtEnd() or peek().type != type) {
        return false;
    }
    ++current;
    return true;
}

bool Parser::match(initializer_list<TokenType> types) {
    
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
    return tokens[current - 1];
}

void Parser::error(const string& message) {
    
    cerr << "Error at line " << peek().line << ": " << message << endl;
    exit(EXIT_FAILURE);
}

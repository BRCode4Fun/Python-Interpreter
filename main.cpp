#include <iostream>
#include <fstream>
#include "Include/Lexer.hpp"
#include "Include/Parser.hpp"
#include "Include/Ast.hpp"
#include "Include/Interpreter.hpp"


void show_tokens(std::vector<Token> &tokens){

    for(int i = 0; i < tokens.size(); i++)
    {
          std::cout <<  tokens[i].getTokenTypeString(tokens[i].type) << "   " << tokens[i].lexeme <<  std::endl;
    }
}


int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [filename].py\n";
        return 1;
    }

    std::ifstream inputFile(argv[1]);

    if (!inputFile) {
        std::cerr << "Error: could not open file '" << argv[1] << "'\n";
        return 1;
    }

    std::string source((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens(); 

    //show_tokens(tokens);

    Parser parser(tokens);

    AstNode * root = parser.parseProgram();

    Interpreter  interpreter; 

    interpreter.interpret(root);
    
    return 1; 

}

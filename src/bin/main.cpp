#include <iostream>
#include <fstream>
#include "../libs/lexer/lexer.hpp"
#include "../libs/parser/parser.hpp"
#include "../libs/ast/ast.hpp"
#include "../libs/interpreter/interpreter.hpp"

#pragma GCC optimize("Ofast")
#pragma GCC target("avx,avx2,fma")

void show_tokens(std::vector<Token> &tokens){

    for(unsigned long i = 0; i < tokens.size(); i++) {
        std::cout << tokens[i] <<  std::endl;
    }
}

int main(int argc, char* argv[]) {

    ios_base::sync_with_stdio(false);
    cin.tie(0); cout.tie(0);

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
    Interpreter interpreter;
    
    std::vector<Token> tokens;
    
    try {
        
        tokens = lexer.scanTokens(); 

    } catch(const runtime_error& err) {

        cerr << err.what() << endl; exit(EXIT_FAILURE);
    }
    #ifdef DEBUG
        /* to define DEBUG, use `make DEBUG=1` when compiling */
        show_tokens(tokens);
    #endif
    
    Parser parser(tokens);
    ProgramNode* root;
    
    try {
        root = parser.parse();
    } catch(const runtime_error& err) {
        cerr << err.what() << endl; exit(EXIT_FAILURE);
    }
    interpreter.interpret(root);

    return 0;
}

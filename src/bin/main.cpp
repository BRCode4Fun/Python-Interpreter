#include <iostream>
#include <fstream>
#include <vector>
#include "../libs/lexer/lexer.hpp"
#include "../libs/parser/parser.hpp"
#include "../libs/ast/ast.hpp"
#include "../libs/interpreter/interpreter.hpp"

#pragma GCC optimize("Ofast")
#pragma GCC target("avx,avx2,fma")

#ifdef DEBUG
inline void show_tokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::cout << token << '\n';
    }
    std::cout << std::flush;
}
#endif

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr); std::cout.tie(nullptr);

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [filename].py\n";
        return 1;
    }

    const char* const filename = argv[1];
    std::ifstream inputFile(filename);

    if (!inputFile) {
        std::cerr << "Error: could not open file '" << filename << "'\n";
        return 1;
    }

    std::string source((std::istreambuf_iterator<char>(inputFile)), {});

    try {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();

        #ifdef DEBUG
            /* To define DEBUG, use `make DEBUG=1` when compiling */
            show_tokens(tokens);
        #endif

        Parser parser(tokens);
        ProgramNode* root = parser.parse();

        Interpreter interpreter;
        interpreter.interpret(root);
        
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}


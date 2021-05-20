/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#include "Lex.hpp"

void lex(std::string text) {
    Lexer tempLexer(text);
    auto tokens = tempLexer.ConsumeAllAndReturn();
    for (size_t i = 0; i < tokens.size(); i++) {
        Token currentToken = tokens.at(i);
        std::cout << currentToken.Lexeme << " ";
    }
    std::cout << std::endl;

}

int main(int argc, char** argv) {
    // Emulate a REPL (Read, Evaluate, Print, Loop)
    std::cout << "Fusco Interpreter, version " << INTERP_VERSION << std::endl;
    std::cout << "20/05/21, Curle" << std::endl << std::endl;
    std::cout << "> ";
    for(std::string line; std::getline(std::cin, line);) {
        std::cout << "> ";
        lex(line);
    }
}


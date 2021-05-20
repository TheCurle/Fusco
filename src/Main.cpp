/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#include "Lex.hpp"
#include <AST.hpp>
#include <ast/Expression.hpp>

bool ErrorState = false;

void lex(std::string text) {
    /*
    Lexer tempLexer(text);
    auto tokens = tempLexer.ConsumeAllAndReturn();
    for (size_t i = 0; i < tokens.size(); i++) {
        Token currentToken = tokens.at(i);
        std::cout << "\t" << currentToken.Type << " " << currentToken.Lexeme << std::endl;
    }
    std::cout << std::endl; */

    LiteralExpression<std::string> literal123("123");
    struct Token subtract = (struct Token) { AR_MINUS, "-", '-' };
    UnaryExpression<std::string> unaryNegative(subtract, &literal123);

    struct Token multiply = (struct Token) { AR_ASTERISK, "*", '*' };
    LiteralExpression<std::string> literal45("45");
    GroupingExpression<std::string> literalGroup(&literal45);

    BinaryExpression<std::string> result(&unaryNegative, multiply, &literalGroup);

    TreePrinter printer;
    std::cout << printer.print(&result) << std::endl;

}

int main(int argc, char** argv) {
    // Emulate a REPL (Read, Evaluate, Print, Loop)
    std::cout << "Fusco Interpreter, version " << INTERP_VERSION << std::endl;
    std::cout << "20/05/21, Curle" << std::endl << std::endl;
    std::cout << "> ";
    for(std::string line; std::getline(std::cin, line);) {
        lex(line);
        std::cout << "> ";
    }
}


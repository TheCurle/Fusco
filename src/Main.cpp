/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#include <Lex.hpp>
#include <Parse.hpp>
#include <ast/Expression.hpp>
#include <interpreter/Interpreter.hpp>

bool ErrorState = false;

static Interpreter Engine;

void lex(std::string text) {

    Lexer tokenStream(text);
    auto tokens = tokenStream.ConsumeAllAndReturn();
    Parser parser(tokens);
    Expression<Object>* expression = parser.parse();

    if(ErrorState) return;

    TreePrinter printer;
    std::cout << printer.print(expression).ToString() << std::endl;
    Engine.Interpret(expression);
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


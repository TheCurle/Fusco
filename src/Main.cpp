/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#include <lexer/Lex.hpp>
#include <Parse.hpp>
#include <ast/Expression.hpp>
#include <interpreter/Interpreter.hpp>

bool ErrorState = false;

static Interpreter Engine;

Object Object::Null;

void lex(std::string text) {

    Lexer tokenStream(text);
    auto tokens = tokenStream.ConsumeAllAndReturn();
    Parser parser(tokens);
    std::vector<Statement*> statements = parser.parse();

    if(ErrorState) return;

    TreePrinter printer;
    printer.print(statements);

    Engine.Interpret(statements);
}

int main(int argc, char** argv) {

    Object::Null.Type = Object::NullType;

    std::cout << "Fusco Interpreter, version " << INTERP_VERSION << std::endl;
    std::cout << "20/05/21, Curle" << std::endl
                  << std::endl;

    if (argc < 2) {
        // Emulate a REPL (Read, Evaluate, Print, Loop)
        std::cout << "$ ";
        for (std::string line; std::getline(std::cin, line);) {
            ErrorState = false;
            lex(line);
            std::cout << "$ ";
        }
    } else {
        // Read and run the given file.
        std::ifstream File(argv[1]);

        std::string str((std::istreambuf_iterator<char>(File)),
                 std::istreambuf_iterator<char>());

        lex(str);
    }
}


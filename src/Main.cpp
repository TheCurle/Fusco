/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#include <Parse.hpp>
#include <ast/Expression.hpp>
#include <interpreter/Interpreter.hpp>
#include <lexer/Lex.hpp>

bool ErrorState = false;

static shared_ptr<ExecutionContext> Context =
    std::make_shared<ExecutionContext>();
static shared_ptr<Interpreter> Engine = std::make_shared<Interpreter>(Context);

Object Object::Null;

void lex(std::string text) {
    Lexer tokenStream(text);
    auto tokens = tokenStream.ConsumeAllAndReturn();

    Parser parser(tokens);
    std::vector<shared_ptr<Statement>> statements = parser.parse();

    if (ErrorState) return;

    std::shared_ptr<TreePrinter> printer = std::make_shared<TreePrinter>();
    printer->print(statements);

    std::shared_ptr<Resolver> resolver = std::make_shared<Resolver>(Engine);
    resolver->resolveAll(statements);

    if (ErrorState) return;

    Engine->Interpret(statements);
}

int main(int argc, char** argv) {
    Object::Null.Type = Object::NullType;

    std::cout << "Fusco Interpreter, version " << INTERP_VERSION << std::endl;
    std::cout << "20/05/21, Curle" << std::endl << std::endl;

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

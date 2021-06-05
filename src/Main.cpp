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
    Expression<Object>* expression = parser.parse();

    if(ErrorState) return;

    TreePrinter printer;
    std::cout << printer.print(expression).ToString() << std::endl;
    /*
    LiteralExpression<std::string> literal123("123");
    struct Token subtract = (struct Token) { AR_MINUS, "-", '-' };
    UnaryExpression<std::string> unaryNegative(subtract, &literal123);

    struct Token multiply = (struct Token) { AR_ASTERISK, "*", '*' };
    LiteralExpression<std::string> literal45("45");
    GroupingExpression<std::string> literalGroup(&literal45);

    BinaryExpression<std::string> result(&unaryNegative, multiply, &literalGroup);
    */
}

int main(int argc, char** argv) {
    Object::Null.Type = Object::NullType;
    // Emulate a REPL (Read, Evaluate, Print, Loop)
    std::cout << "Fusco Interpreter, version " << INTERP_VERSION << std::endl;
    std::cout << "20/05/21, Curle" << std::endl << std::endl;
    std::cout << "> ";
    for(std::string line; std::getline(std::cin, line);) {
        lex(line);
        std::cout << "> ";
    }
}


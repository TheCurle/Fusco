/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#pragma once
#include <Lex.hpp>
#include <ast/Expression.hpp>
#include <Main.hpp>
#include <stdexcept>

class Parser : public Common {
public:
    Parser(std::vector<struct Token> pTokens)
        : tokens(pTokens), currentToken(0) {}

    Expression<std::string>* parse();

private:
    std::vector<struct Token> tokens;
    size_t currentToken;

    /** Token Manipulation **/
    template <class... T>
    bool matchAny(T ... tokens);
    bool check(Lexeme type);
    struct Token advance();
    struct Token previous();
    struct Token peek();
    bool endOfStream();

    /** Statement Parsing **/
    Expression<std::string>* expression();
    Expression<std::string>* equality();
    Expression<std::string>* comparison();
    Expression<std::string>* term();
    Expression<std::string>* factor();
    Expression<std::string>* unary();
    Expression<std::string>* primary();

    /** Error management **/
    struct Token verify(Lexeme type, std::string error);
    std::runtime_error error(struct Token token, std::string message);

    void recover();
};
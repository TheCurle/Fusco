/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#pragma once
#include <lexer/Lex.hpp>
#include <ast/Statement.hpp>
#include <Main.hpp>
#include <stdexcept>

class Parser : public Common {
public:
    Parser(std::vector<struct Token> pTokens)
        : tokens(pTokens), currentToken(0) {}

    std::vector<shared_ptr<Statement>> parse();

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
    std::vector<shared_ptr<Statement>> block();
    shared_ptr<Statement> declaration();
    shared_ptr<Statement> varDeclaration();
    shared_ptr<Statement> statement();
    shared_ptr<Statement> returnStatement();
    shared_ptr<Statement> printStatement();
    shared_ptr<Statement> ifStatement();
    shared_ptr<Statement> whileStatement();
    shared_ptr<Statement> forStatement();
    shared_ptr<Statement> expressionStatement();

    shared_ptr<FuncStatement> function(std::string type);

    EXPR expression();
    EXPR assignment();
    EXPR orExpr();
    EXPR andExpr();
    EXPR equality();
    EXPR comparison();
    EXPR term();
    EXPR factor();
    EXPR unary();
    EXPR call();
    EXPR finishCall(EXPR expr);
    EXPR primary();

    /** Error management **/
    struct Token verify(Lexeme type, std::string error);
    RuntimeError error(struct Token token, std::string message);

    void recover();
};
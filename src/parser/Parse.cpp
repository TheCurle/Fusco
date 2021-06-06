/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#include <Parse.hpp>

std::vector<Statement*> Parser::parse() {
    std::vector<Statement*> statements;

    while(!endOfStream()) {
        statements.emplace_back(declaration());
    }

    return statements;
}

Statement* Parser::declaration() {
    try {
        if(matchAny(KW_VAR)) return varDeclaration();

        return statement();
    } catch (RuntimeError e) {
        recover();
        return nullptr;
    }
}

Statement* Parser::varDeclaration() {
    struct Token name = verify(LI_IDENTIFIER, "Expected variable name.");

    Expression<Object>* initializer = nullptr;
    if(matchAny(LI_EQUAL)) {
        initializer = expression();
    }

    verify(LI_SEMICOLON, "Expected a semicolon after a variable declaration.");
    return new VariableStatement(name, initializer);
}

Statement* Parser::statement() {
    if(matchAny(KW_PRINT)) return printStatement();

    return expressionStatement();
}

Statement* Parser::printStatement() {
    Expression<Object>* value = expression();
    verify(LI_SEMICOLON, "Expected ';' after an expression to print");

    return new PrintStatement(value);
}

Statement* Parser::expressionStatement() {
    Expression<Object>* value = expression();
    verify(LI_SEMICOLON, "Expected ';' after an expression.");

    return new ExpressionStatement(value);
}

Expression<Object>* Parser::expression() {
    return equality();
}

Expression<Object>* Parser::equality() {
    Expression<Object>* expr = comparison();

    while(matchAny(CMP_INEQ, CMP_EQUAL)) {
        struct Token operatorToken = previous();
        Expression<Object>* right = comparison();
        expr = new BinaryExpression<Object>(expr, operatorToken, right);
    }

    return expr;
}

Expression<Object>* Parser::comparison() {
    Expression<Object>* expr = term();

    while(matchAny(CMP_GREATER, CMP_GREAT_EQUAL, CMP_LESS, CMP_LESS_EQUAL)) {
        struct Token operatorToken = previous();
        Expression<Object>* right = term();
        expr = new BinaryExpression<Object>(expr, operatorToken, right);
    }

    return expr;
}

Expression<Object>* Parser::term() {
    Expression<Object>* expr = factor();

    while(matchAny(AR_MINUS, AR_PLUS)) {
        struct Token operatorToken = previous();
        Expression<Object>* right = factor();
        expr = new BinaryExpression<Object>(expr, operatorToken, right);
    }

    return expr;
}

Expression<Object>* Parser::factor() {
    Expression<Object>* expr = unary();

    while(matchAny(AR_ASTERISK, AR_RSLASH)) {
        struct Token operatorToken = previous();
        Expression<Object>* right = unary();
        expr = new BinaryExpression<Object>(expr, operatorToken, right);
    }

    return expr;
}

Expression<Object>* Parser::unary() {
    if(matchAny(BOOL_EXCLAIM, AR_MINUS)) {
        struct Token operatorToken = previous();
        Expression<Object>* right = unary();
        return new UnaryExpression<Object>(operatorToken, right);
    }

    return primary();
}

Expression<Object>* Parser::primary() {
    if(matchAny(KW_FALSE)) return new LiteralExpression(Object::NewBool(false));
    if(matchAny(KW_TRUE)) return new LiteralExpression(Object::NewBool(true));
    if(matchAny(KW_NULL)) return new LiteralExpression(Object::Null);

    if(matchAny(LI_NUMBER))
        return new LiteralExpression<Object>(previous().Value);

    if(matchAny(LI_STRING))
        return new LiteralExpression<Object>(Object::NewStr(previous().Lexeme));

    if(matchAny(LI_IDENTIFIER))
        return new VariableExpression<Object>(previous());

    if(matchAny(LI_LPAREN)) {
        Expression<Object>* expr = expression();
        verify(LI_RPAREN, "Expected ')' after expression");
        return new GroupingExpression<Object>(expr);
    }

    throw error(peek(), "Expected an expression");
}

std::runtime_error Parser::error(struct Token token, std::string message) {
    if (token.Type == LI_EOF) {
        return std::runtime_error(std::to_string(token.Line) + " at end" + message);
    } else {
        return std::runtime_error(std::to_string(token.Line) + " at '" + token.Lexeme + "'" + message);
    }
}

struct Token Parser::verify(Lexeme type, std::string message) {
    if(check(type)) return advance();
    throw error(peek(), message);
}

void Parser::recover() {
    advance();

    while(!endOfStream()) {
        if(previous().Type == LI_SEMICOLON) return;

        switch(peek().Type) {
            case KW_CLASS:
            case KW_FUNC:
            case KW_VAR:
            case KW_FOR:
            case KW_IF:
            case KW_WHILE:
            case KW_PRINT:
            case KW_RETURN:
                return;
        }

        advance();
    }
}

template <class... T>
bool Parser::matchAny(T ... tokens) {
    std::vector<Lexeme> vec;
    (vec.push_back(tokens), ...);


    for(auto value: vec) {
        if(check(value)) {
            advance();
            return true;
        }
    }

    return false;
}

bool Parser::check(Lexeme type) {
    if(endOfStream()) return false;
    return peek().Type == type;
}

bool Parser::endOfStream() {
    return peek().Type == LI_EOF;
}

struct Token Parser::advance() {
    if(!endOfStream()) currentToken++;
    return previous();
}

struct Token Parser::peek() {
    return tokens.at(currentToken);
}

struct Token Parser::previous() {
    return tokens.at(currentToken - 1);
}

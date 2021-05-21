/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#include <Parse.hpp>

Expression<std::string>* Parser::parse() {
    try {
        return expression();
    }
    catch(std::runtime_error error) {
        std::cerr << "Caught error, halting..." << std::endl;
        //recover();
        return nullptr;
    }

}

Expression<std::string>* Parser::expression() {
    return equality();
}

Expression<std::string>* Parser::equality() {
    Expression<std::string>* expr = comparison();

    while(matchAny(CMP_INEQ, CMP_EQUAL)) {
        struct Token operatorToken = previous();
        Expression<std::string>* right = comparison();
        expr = new BinaryExpression<std::string>(expr, operatorToken, right);
    }

    return expr;
}

Expression<std::string>* Parser::comparison() {
    Expression<std::string>* expr = term();

    while(matchAny(CMP_GREATER, CMP_GREAT_EQUAL, CMP_LESS, CMP_LESS_EQUAL)) {
        struct Token operatorToken = previous();
        Expression<std::string>* right = term();
        expr = new BinaryExpression<std::string>(expr, operatorToken, right);
    }

    return expr;
}

Expression<std::string>* Parser::term() {
    Expression<std::string>* expr = factor();

    while(matchAny(AR_MINUS, AR_PLUS)) {
        struct Token operatorToken = previous();
        Expression<std::string>* right = factor();
        expr = new BinaryExpression<std::string>(expr, operatorToken, right);
    }

    return expr;
}

Expression<std::string>* Parser::factor() {
    Expression<std::string>* expr = unary();

    while(matchAny(AR_ASTERISK, AR_RSLASH)) {
        struct Token operatorToken = previous();
        Expression<std::string>* right = unary();
        expr = new BinaryExpression<std::string>(expr, operatorToken, right);
    }

    return expr;
}

Expression<std::string>* Parser::unary() {
    if(matchAny(BOOL_EXCLAIM, AR_MINUS)) {
        struct Token operatorToken = previous();
        Expression<std::string>* right = unary();
        return new UnaryExpression<std::string>(operatorToken, right);
    }

    return primary();
}

Expression<std::string>* Parser::primary() {
    if(matchAny(KW_FALSE)) return new LiteralExpression(std::string("false"));
    if(matchAny(KW_TRUE)) return new LiteralExpression(std::string("true"));
    if(matchAny(KW_NULL)) return new LiteralExpression(std::string("null"));

    if(matchAny(LI_NUMBER, LI_STRING))
        return new LiteralExpression<std::string>(previous().Lexeme);

    if(matchAny(LI_LPAREN)) {
        Expression<std::string>* expr = expression();
        verify(LI_RPAREN, "Expected ')' after expression");
        return new GroupingExpression<std::string>(expr);
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

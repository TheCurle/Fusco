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
        if(matchAny(KW_FUNC)) return function("function");
        if(matchAny(KW_VAR)) return varDeclaration();

        return statement();
    } catch (RuntimeError &e) {
        //recover();
        Error(e);
        return nullptr;
    }
}

Statement* Parser::varDeclaration() {
    struct Token name = verify(LI_IDENTIFIER, "Expected variable name.");

    EXPR initializer = nullptr;
    if(matchAny(LI_EQUAL)) {
        initializer = expression();
    }

    verify(LI_SEMICOLON, "Expected a semicolon after a variable declaration.");
    return new VariableStatement(name, initializer);
}

Statement* Parser::statement() {
    if(matchAny(KW_IF)) return ifStatement();
    if(matchAny(KW_FOR)) return forStatement();
    if(matchAny(KW_WHILE)) return whileStatement();
    if(matchAny(KW_PRINT)) return printStatement();
    if(matchAny(LI_LBRACE)) return new BlockStatement(block());

    return expressionStatement();
}

std::vector<Statement*> Parser::block() {
    std::vector<Statement*> statements;

    while(!check(LI_RBRACE) && !endOfStream()) {
        statements.emplace_back(declaration());
    }

    verify(LI_RBRACE, "Unclosed block statement");
    return statements;
}

Statement* Parser::printStatement() {
    EXPR value = expression();
    verify(LI_SEMICOLON, "Expected ';' after an expression to print");

    return new PrintStatement(value);
}

Statement* Parser::ifStatement() {
    verify(LI_LPAREN, "Expected a ( after if.");
    EXPR Condition = expression();
    verify(LI_RPAREN, "Expected a ) after the condition in if.");

    Statement* Then = statement();
    Statement* Else = nullptr;

    if(matchAny(KW_ELSE))
        Else = statement();

    return new IfStatement(Condition, Then, Else);
}

Statement* Parser::forStatement() {
    verify(LI_LPAREN, "Expected '(' after for.");
    Statement* initializer;

    if(matchAny(LI_SEMICOLON))
        initializer = nullptr;
    else if(matchAny(KW_VAR))
        initializer = varDeclaration();
    else
        initializer = expressionStatement();

    EXPR condition = nullptr;

    if(!check(LI_SEMICOLON))
        condition = expression();

    verify(LI_SEMICOLON, "Expected ';' after loop condition.");

    EXPR increment = nullptr;
    if(!check(LI_RPAREN))
        increment = expression();

    verify(LI_RPAREN, "Expected ')' after for.");

    Statement* body = statement();

    if(increment != nullptr) {
        std::vector<Statement*> stmts;
        stmts.emplace_back(body);
        stmts.emplace_back(new ExpressionStatement(increment));
        body = new BlockStatement(stmts);
    }

    if(condition == nullptr) {
        condition = new LiteralExpression<Object>(Object::NewBool(true));
    }
    body = new WhileStatement(condition, body);

    if(initializer != nullptr) {
        std::vector<Statement*> stmts;
        stmts.emplace_back(initializer);
        stmts.emplace_back(body);
        body = new BlockStatement(stmts);
    }

    return body;
}

Statement* Parser::whileStatement() {
    verify(LI_LPAREN, "Expected a ( after while.");
    EXPR condition = expression();
    verify(LI_RPAREN, "Expected a ) after the condition in while.");

    Statement* body = statement();

    return new WhileStatement(condition, body);
}

Statement* Parser::expressionStatement() {
    EXPR value = expression();
    verify(LI_SEMICOLON, "Expected ';' after an expression.");

    return new ExpressionStatement(value);
}

FuncStatement* Parser::function(std::string type) {
    Token name = verify(LI_IDENTIFIER, std::string("Expected a ").append(type).append(" name."));
    std::vector<Token> parameters;

    verify(LI_LPAREN, std::string("Expected ( after ").append(type).append(" name."));

    if(!check(LI_RPAREN)) {
        do {
            if(parameters.size() > 254)
                Error(peek(), "255 parameter limit reached.");

            parameters.emplace_back(verify(LI_IDENTIFIER, "Expected parameter name."));
        } while (matchAny(LI_COMMA));
    }

    verify(LI_RPAREN, std::string("Expected ( after ").append(type).append(" parameters."));
    verify(LI_LBRACE, std::string("Expected { before ").append(type).append(" body."));

    std::vector<Statement*> body = block();

    return new FuncStatement(name, parameters, body);
}

EXPR Parser::expression() {
    return assignment();
}

EXPR Parser::assignment() {
    EXPR expr = orExpr();

    if(matchAny(LI_EQUAL)) {
        Token equals = previous();
        EXPR value = assignment();

        if(dynamic_cast<VariableExpression<Object>*>(expr) != nullptr) {
            Token name = dynamic_cast<VariableExpression<Object>*>(expr)->Name;
            return new AssignmentExpression<Object>(name, value);
        }

        Error(equals, std::string("Cannot assign an r-value"));
    }

    return expr;
}

EXPR Parser::orExpr() {
    EXPR expr = andExpr();

    while(matchAny(KW_OR)) {
        Token operatorToken = previous();
        EXPR right = andExpr();
        expr = new LogicalExpression<Object>(expr, operatorToken, right);
    }

    return expr;
}

EXPR Parser::andExpr() {
    EXPR expr = equality();

    while(matchAny(KW_AND)) {
        Token operatorToken = previous();
        EXPR right = equality();
        expr = new LogicalExpression<Object>(expr, operatorToken, right);
    }

    return expr;
}

EXPR Parser::equality() {
    EXPR expr = comparison();

    while(matchAny(CMP_INEQ, CMP_EQUAL)) {
        struct Token operatorToken = previous();
        EXPR right = comparison();
        expr = new BinaryExpression<Object>(expr, operatorToken, right);
    }

    return expr;
}

EXPR Parser::comparison() {
    EXPR expr = term();

    while(matchAny(CMP_GREATER, CMP_GREAT_EQUAL, CMP_LESS, CMP_LESS_EQUAL)) {
        struct Token operatorToken = previous();
        EXPR right = term();
        expr = new BinaryExpression<Object>(expr, operatorToken, right);
    }

    return expr;
}

EXPR Parser::term() {
    EXPR expr = factor();

    while(matchAny(AR_MINUS, AR_PLUS)) {
        struct Token operatorToken = previous();
        EXPR right = factor();
        expr = new BinaryExpression<Object>(expr, operatorToken, right);
    }

    return expr;
}

EXPR Parser::factor() {
    EXPR expr = unary();

    while(matchAny(AR_ASTERISK, AR_RSLASH)) {
        struct Token operatorToken = previous();
        EXPR right = unary();
        expr = new BinaryExpression<Object>(expr, operatorToken, right);
    }

    return expr;
}

EXPR Parser::unary() {
    if(matchAny(BOOL_EXCLAIM, AR_MINUS)) {
        struct Token operatorToken = previous();
        EXPR right = unary();
        return new UnaryExpression<Object>(operatorToken, right);
    }

    return call();
}

EXPR Parser::call() {
    EXPR expr = primary();

    while(true) {
        if(matchAny(LI_LPAREN)) {
            expr = finishCall(expr);
        } else {
            break;
        }
    }

    return expr;
}

EXPR Parser::finishCall(EXPR callee) {
    std::vector<EXPR> arguments;

    if(!check(LI_RPAREN)) {
        do {
            if(arguments.size() > 254)
                Error(peek(), "255 argument limit reached.");
            arguments.emplace_back(expression());
        } while (matchAny(LI_COMMA));
    }

    Token parenthesis = verify(LI_RPAREN, "Expected ')' after argument list.");

    return new CallExpression<Object>(callee, parenthesis, arguments);
}

EXPR Parser::primary() {
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
        EXPR expr = expression();
        verify(LI_RPAREN, "Expected ')' after expression");
        return new GroupingExpression<Object>(expr);
    }

    throw error(peek(), "Expected an expression");
}

RuntimeError Parser::error(struct Token token, std::string message) {
    if (token.Type == LI_EOF) {
        return RuntimeError(token, std::to_string(token.Line) + " at end" + message);
    } else {
        return RuntimeError(token, std::to_string(token.Line) + " at '" + token.Lexeme + "'" + message);
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

/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#include <Parse.hpp>

std::vector<shared_ptr<Statement>> Parser::parse() {
    std::vector<shared_ptr<Statement>> statements;

    while(!endOfStream()) {
        statements.emplace_back(declaration());
    }

    return statements;
}

shared_ptr<Statement> Parser::declaration() {
    try {
        if(matchAny(KW_CLASS)) return classDeclaration();
        if(matchAny(KW_FUNC)) return function("function");
        if(matchAny(KW_VAR)) return varDeclaration();

        return statement();
    } catch (RuntimeError &e) {
        //recover();
        Error(e);
        return nullptr;
    }
}

shared_ptr<Statement> Parser::varDeclaration() {
    struct Token name = verify(LI_IDENTIFIER, "Expected variable name.");

    EXPR initializer = nullptr;
    if(matchAny(LI_EQUAL)) {
        initializer = expression();
    }

    verify(LI_SEMICOLON, "Expected a semicolon after a variable declaration.");
    return std::make_shared<VariableStatement>(name, initializer);
}

shared_ptr<Statement> Parser::statement() {
    if(matchAny(KW_IF)) return ifStatement();
    if(matchAny(KW_FOR)) return forStatement();
    if(matchAny(KW_WHILE)) return whileStatement();
    if(matchAny(KW_PRINT)) return printStatement();
    if(matchAny(KW_RETURN)) return returnStatement();
    if(matchAny(LI_LBRACE)) return std::make_shared<BlockStatement>(block());

    return expressionStatement();
}

shared_ptr<Statement> Parser::returnStatement() {
    Token keyword = previous();

    EXPR value = nullptr;
    if(!check(LI_SEMICOLON))
        value = expression();

    verify(LI_SEMICOLON, "Expected ; after return.");

    return std::make_shared<ReturnStatement>(keyword, value);
}

std::vector<shared_ptr<Statement>> Parser::block() {
    std::vector<shared_ptr<Statement>> statements;

    while(!check(LI_RBRACE) && !endOfStream()) {
        statements.emplace_back(declaration());
    }

    verify(LI_RBRACE, "Unclosed block statement");
    return statements;
}

shared_ptr<Statement> Parser::printStatement() {
    EXPR value = expression();
    verify(LI_SEMICOLON, "Expected ';' after an expression to print");

    return std::make_shared<PrintStatement>(value);
}

shared_ptr<Statement> Parser::ifStatement() {
    verify(LI_LPAREN, "Expected a ( after if.");
    EXPR Condition = expression();
    verify(LI_RPAREN, "Expected a ) after the condition in if.");

    shared_ptr<Statement> Then = statement();
    shared_ptr<Statement> Else = nullptr;

    if(matchAny(KW_ELSE))
        Else = statement();

    return std::make_shared<IfStatement>(Condition, Then, Else);
}

shared_ptr<Statement> Parser::forStatement() {
    verify(LI_LPAREN, "Expected '(' after for.");
    shared_ptr<Statement> initializer;

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

    shared_ptr<Statement> body = statement();

    if(increment != nullptr) {
        std::vector<shared_ptr<Statement>> stmts;
        stmts.emplace_back(body);
        stmts.emplace_back(std::make_shared<ExpressionStatement>(increment));
        body = std::make_shared<BlockStatement>(stmts);
    }

    if(condition == nullptr) {
        condition = std::make_shared<LiteralExpression<Object>>(Object::NewBool(true));
    }
    body = std::make_shared<WhileStatement>(condition, body);

    if(initializer != nullptr) {
        std::vector<shared_ptr<Statement>> stmts;
        stmts.emplace_back(initializer);
        stmts.emplace_back(body);
        body = std::make_shared<BlockStatement>(stmts);
    }

    return body;
}

shared_ptr<Statement> Parser::whileStatement() {
    verify(LI_LPAREN, "Expected a ( after while.");
    EXPR condition = expression();
    verify(LI_RPAREN, "Expected a ) after the condition in while.");

    shared_ptr<Statement> body = statement();

    return std::make_shared<WhileStatement>(condition, body);
}

shared_ptr<Statement> Parser::expressionStatement() {
    EXPR value = expression();
    verify(LI_SEMICOLON, "Expected ';' after an expression.");

    return std::make_shared<ExpressionStatement>(value);
}

shared_ptr<FuncStatement> Parser::function(std::string type) {
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

    std::vector<shared_ptr<Statement>> body = block();

    return std::make_shared<FuncStatement>(name, parameters, body);
}

shared_ptr<ClassStatement> Parser::classDeclaration() {
    Token name = verify(LI_IDENTIFIER, "Expected a class name.");
    verify(LI_LBRACE, "Expected a block start after a class body.");

    std::vector<std::shared_ptr<FuncStatement>> functions;

    while(!check(LI_RBRACE) && !endOfStream()) 
        functions.emplace_back(function("method"));

    verify(LI_RBRACE, "Expected a block end for a class.");

    return std::make_shared<ClassStatement>(name, functions);
}

EXPR Parser::expression() {
    return assignment();
}

EXPR Parser::assignment() {
    EXPR expr = orExpr();

    if(matchAny(LI_EQUAL)) {
        Token equals = previous();
        EXPR value = assignment();

        if(auto var = dynamic_cast<VariableExpression<Object>*>(expr.get()); var != nullptr) {
            Token name = var->Name;
            return std::make_shared<AssignmentExpression<Object>>(name, value);
        } else if(auto get = dynamic_cast<GetExpression<Object>*>(expr.get()); get != nullptr) {
            return std::make_shared<SetExpression<Object>>(get->Obj, get->Name, value);
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
        expr = std::make_shared<LogicalExpression<Object>>(expr, operatorToken, right);
    }

    return expr;
}

EXPR Parser::andExpr() {
    EXPR expr = equality();

    while(matchAny(KW_AND)) {
        Token operatorToken = previous();
        EXPR right = equality();
        expr = std::make_shared<LogicalExpression<Object>>(expr, operatorToken, right);
    }

    return expr;
}

EXPR Parser::equality() {
    EXPR expr = comparison();

    while(matchAny(CMP_INEQ, CMP_EQUAL)) {
        struct Token operatorToken = previous();
        EXPR right = comparison();
        expr = std::make_shared<BinaryExpression<Object>>(expr, operatorToken, right);
    }

    return expr;
}

EXPR Parser::comparison() {
    EXPR expr = term();

    while(matchAny(CMP_GREATER, CMP_GREAT_EQUAL, CMP_LESS, CMP_LESS_EQUAL)) {
        struct Token operatorToken = previous();
        EXPR right = term();
        expr = std::make_shared<BinaryExpression<Object>>(expr, operatorToken, right);
    }

    return expr;
}

EXPR Parser::term() {
    EXPR expr = factor();

    while(matchAny(AR_MINUS, AR_PLUS)) {
        struct Token operatorToken = previous();
        EXPR right = factor();
        expr = std::make_shared<BinaryExpression<Object>>(expr, operatorToken, right);
    }

    return expr;
}

EXPR Parser::factor() {
    EXPR expr = unary();

    while(matchAny(AR_ASTERISK, AR_RSLASH)) {
        struct Token operatorToken = previous();
        EXPR right = unary();
        expr = std::make_shared<BinaryExpression<Object>>(expr, operatorToken, right);
    }

    return expr;
}

EXPR Parser::unary() {
    if(matchAny(BOOL_EXCLAIM, AR_MINUS)) {
        struct Token operatorToken = previous();
        EXPR right = unary();
        return std::make_shared<UnaryExpression<Object>>(operatorToken, right);
    }

    return call();
}

EXPR Parser::call() {
    EXPR expr = primary();

    while(true) {
        if(matchAny(LI_LPAREN)) {
            expr = finishCall(expr);
        } else if (matchAny(LI_PERIOD)) {
            Token name = verify(LI_IDENTIFIER, "Expected a property to retrieve.");
            expr = std::make_shared<GetExpression<Object>>(expr, name);
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

    return std::make_shared<CallExpression<Object>>(callee, parenthesis, arguments);
}

EXPR Parser::primary() {
    if(matchAny(KW_FALSE)) return std::make_shared<LiteralExpression<Object>>(Object::NewBool(false));
    if(matchAny(KW_TRUE)) return std::make_shared<LiteralExpression<Object>>(Object::NewBool(true));
    if(matchAny(KW_NULL)) return std::make_shared<LiteralExpression<Object>>(Object::Null);
    if(matchAny(KW_THIS)) return std::make_shared<ThisExpression<Object>>(tokens[currentToken]);

    if(matchAny(LI_NUMBER))
        return std::make_shared<LiteralExpression<Object>>(previous().Value);

    if(matchAny(LI_STRING))
        return std::make_shared<LiteralExpression<Object>>(Object::NewStr(previous().Lexeme));

    if(matchAny(LI_IDENTIFIER))
        return std::make_shared<VariableExpression<Object>>(previous());

    if(matchAny(LI_LPAREN)) {
        EXPR expr = expression();
        verify(LI_RPAREN, "Expected ')' after expression");
        return std::make_shared<GroupingExpression<Object>>(expr);
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

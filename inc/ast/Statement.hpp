/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/
#pragma once
#include <lexer/Lex.hpp>
#include <ast/Expression.hpp>
#include <utility>

class ExpressionStatement;
class PrintStatement;
class VariableStatement;
class BlockStatement;
class IfStatement;
class WhileStatement;
class FuncStatement;
class ClassStatement;
class ReturnStatement;

class StatementVisitor {
    public:

    virtual ~StatementVisitor() = default;
    virtual void visitExpression(ExpressionStatement &Expr) = 0;
    virtual void visitPrint(PrintStatement &Print) = 0;
    virtual void visitVariable(VariableStatement &Var) = 0;
    virtual void visitBlock(BlockStatement &Block) = 0;
    virtual void visitIf(IfStatement &If) = 0;
    virtual void visitWhile(WhileStatement &While) = 0;
    virtual void visitFunc(FuncStatement &Func) = 0;
    virtual void visitClass(ClassStatement &Class) = 0;
    virtual void visitReturn(ReturnStatement &Return) = 0;
};

class Statement : public std::enable_shared_from_this<Statement> {
    public:
    virtual void accept(shared_ptr<StatementVisitor> visitor) = 0;
    virtual ~Statement() = default;
};

class ExpressionStatement : public Statement {
    public:
    explicit ExpressionStatement(shared_ptr<Expression<Object>> pExpr) : Expr(std::move(pExpr)) {}
    void accept(shared_ptr<StatementVisitor> visitor) override {
        visitor->visitExpression(*this);
    }

    shared_ptr<Expression<Object>> Expr;
};

class PrintStatement : public Statement {
    public:
    explicit PrintStatement(shared_ptr<Expression<Object>> pExpr) : Expr(std::move(pExpr)) {}
    void accept(shared_ptr<StatementVisitor> visitor) override {
        visitor->visitPrint(*this);
    }

    shared_ptr<Expression<Object>> Expr;
};

class VariableStatement : public Statement {
    public:
    explicit VariableStatement(Token pName, shared_ptr<Expression<Object>> pExpr) : Expr(std::move(pExpr)), Name(std::move(pName)) {}
    void accept(shared_ptr<StatementVisitor> visitor) override {
        visitor->visitVariable(*this);
    }

    shared_ptr<Expression<Object>> Expr;
    struct Token Name;
};

class BlockStatement : public Statement {
    public:
    explicit BlockStatement(std::vector<shared_ptr<Statement>> statements) : Statements(std::move(statements)) {}
    void accept(shared_ptr<StatementVisitor> visitor) override {
        visitor->visitBlock(*this);
    }

    std::vector<shared_ptr<Statement>> Statements;
};

class IfStatement : public Statement {
    public:
    explicit IfStatement(shared_ptr<Expression<Object>> pCondition, shared_ptr<Statement> pThen, shared_ptr<Statement> pElse)
        : Condition(std::move(pCondition)), Then(std::move(pThen)), Else(std::move(pElse)) {}

    void accept(shared_ptr<StatementVisitor> visitor) override {
        visitor->visitIf(*this);
    }

    shared_ptr<Expression<Object>> Condition;
    shared_ptr<Statement> Then;
    shared_ptr<Statement> Else;
};

class WhileStatement : public Statement {
    public:
    explicit WhileStatement(shared_ptr<Expression<Object>> pCondition, shared_ptr<Statement> pBody)
        : Condition(std::move(pCondition)), Body(std::move(pBody)) {}

    void accept(shared_ptr<StatementVisitor> visitor) override {
        visitor->visitWhile(*this);
    }

    shared_ptr<Expression<Object>> Condition;
    shared_ptr<Statement> Body;
};

class FuncStatement : public Statement {
    public:
    explicit FuncStatement(Token pName, std::vector<Token> pParams, std::vector<shared_ptr<Statement>> pBody)
        : Name(std::move(pName)), Params(std::move(pParams)), Body(std::move(pBody)) {}

    void accept(shared_ptr<StatementVisitor> visitor) override {
        visitor->visitFunc(*this);
    }

    Token Name;
    std::vector<Token> Params;
    std::vector<shared_ptr<Statement>> Body;
};

class ClassStatement : public Statement {
    public:
    explicit ClassStatement(Token pName, std::vector<std::shared_ptr<FuncStatement>> pFunctions)
        : name(std::move(pName)), functions(std::move(pFunctions)) {}

    void accept(shared_ptr<StatementVisitor> visitor) override {
        visitor->visitClass(*this);
    }

    Token name;
    std::vector<std::shared_ptr<FuncStatement>> functions;
};

class ReturnStatement : public Statement {
    public:
    explicit ReturnStatement(Token pKeyword, EXPR pValue)
        : Keyword(std::move(pKeyword)), Value(std::move(pValue)) {}

    void accept(shared_ptr<StatementVisitor> visitor) override {
        visitor->visitReturn(*this);
    }

    Token Keyword;
    EXPR Value;
};
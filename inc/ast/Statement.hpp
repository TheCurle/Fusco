/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/
#pragma once
#include <lexer/Lex.hpp>
#include <ast/Expression.hpp>

class ExpressionStatement;
class PrintStatement;
class VariableStatement;
class BlockStatement;
class IfStatement;
class WhileStatement;
class ForStatement;

class StatementVisitor {
    public:

    virtual ~StatementVisitor() = default;
    virtual void visitExpression(ExpressionStatement* Expr) = 0;
    virtual void visitPrint(PrintStatement* Print) = 0;
    virtual void visitVariable(VariableStatement* Var) = 0;
    virtual void visitBlock(BlockStatement* Block) = 0;
    virtual void visitIf(IfStatement* If) = 0;
    virtual void visitWhile(WhileStatement* While) = 0;
};

class Statement {
    public:
    virtual void accept(StatementVisitor* visitor) = 0;
    virtual ~Statement() = default;
};

class ExpressionStatement : public Statement {
    public:
    explicit ExpressionStatement(Expression<Object>* pExpr) : Expr(pExpr) {}
    void accept(StatementVisitor* visitor) override {
        visitor->visitExpression(this);
    }

    Expression<Object>* Expr;
};

class PrintStatement : public Statement {
    public:
    explicit PrintStatement(Expression<Object>* pExpr) : Expr(pExpr) {}
    void accept(StatementVisitor* visitor) override {
        visitor->visitPrint(this);
    }

    Expression<Object>* Expr;
};

class VariableStatement : public Statement {
    public:
    explicit VariableStatement(struct Token pName, Expression<Object>* pExpr) : Expr(pExpr), Name(pName) {}
    void accept(StatementVisitor* visitor) override {
        visitor->visitVariable(this);
    }

    Expression<Object>* Expr;
    struct Token Name;
};

class BlockStatement : public Statement {
    public:
    explicit BlockStatement(std::vector<Statement*> statements) : Statements(statements) {}
    void accept(StatementVisitor* visitor) override {
        visitor->visitBlock(this);
    }

    std::vector<Statement*> Statements;
};

class IfStatement : public Statement {
    public:
    explicit IfStatement(Expression<Object>* pCondition, Statement* pThen, Statement* pElse)
        : Condition(pCondition), Then(pThen), Else(pElse) {}

    void accept(StatementVisitor* visitor) override {
        visitor->visitIf(this);
    }

    Expression<Object>* Condition;
    Statement* Then;
    Statement* Else;
};

class WhileStatement : public Statement {
    public:
    explicit WhileStatement(Expression<Object>* pCondition, Statement* pBody)
        : Condition(pCondition), Body(pBody) {}

    void accept(StatementVisitor* visitor) override {
        visitor->visitWhile(this);
    }

    Expression<Object>* Condition;
    Statement* Body;
};
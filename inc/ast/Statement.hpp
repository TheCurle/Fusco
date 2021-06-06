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

class StatementVisitor {
    public:

    virtual ~StatementVisitor() = default;
    virtual void visitExpression(ExpressionStatement* Expr) = 0;
    virtual void visitPrint(PrintStatement* Print) = 0;
    virtual void visitVariable(VariableStatement* Var) = 0;
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
    explicit VariableStatement(struct Token pName, Expression<Object>* pExpr) : Name(pName), Expr(pExpr) {}
    void accept(StatementVisitor* visitor) override {
        visitor->visitVariable(this);
    }

    Expression<Object>* Expr;
    struct Token Name;
};
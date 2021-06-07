/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/
#pragma once
#include <lexer/Lex.hpp>

template <typename T>
class BinaryExpression;

template <typename T>
class UnaryExpression;

template <typename T>
class GroupingExpression;

template <typename T>
class LiteralExpression;

template <typename T>
class VariableExpression;

template <typename T>
class AssignmentExpression;


template <typename T>
class ExpressionVisitor {
public:
    ExpressionVisitor() {}
    virtual ~ExpressionVisitor() {};
    virtual T dummy() = 0;
    virtual T visitBinaryExpression(BinaryExpression<T>* expr) = 0;
    virtual T visitGroupingExpression(GroupingExpression<T>* expr) = 0;
    virtual T visitUnaryExpression(UnaryExpression<T>* expr) = 0;
    virtual T visitLiteralExpression(LiteralExpression<T>* expr) = 0;
    virtual T visitVariableExpression(VariableExpression<T>* expr) = 0;
    virtual T visitAssignmentExpression(AssignmentExpression<T>* expr) = 0;
};

template <typename T>
class Expression {
public:
    Expression() {}
    Expression(const Expression&) = delete;
    Expression& operator=(const Expression&) = delete;
    virtual ~Expression() = default;
    virtual T accept(ExpressionVisitor<T>* visitor) {
        return visitor->dummy();
    };
};

template <typename T>
class BinaryExpression : public Expression<T> {
public:
    explicit BinaryExpression(Expression<T>* pLeft, Token pOperator, Expression<T>* pRight)
        : left(pLeft), right(pRight), operatorToken(pOperator) {}

    T accept(ExpressionVisitor<T>* visitor) override {
        return visitor->visitBinaryExpression(this);
    }

    Expression<T>* left;
    Expression<T>* right;
    struct Token operatorToken;
};

template <typename T>
class GroupingExpression : public Expression<T> {
public:
    explicit GroupingExpression(Expression<T>* pExpression)
        : expression(pExpression) {}

    T accept(ExpressionVisitor<T>* visitor) override {
        return visitor->visitGroupingExpression(this);
    }

    Expression<T>* expression;
};

template <typename T>
class LiteralExpression : public Expression<T> {
public:
    explicit LiteralExpression(T _value) : value(_value) {}

    T accept(ExpressionVisitor<T>* visitor) override {
        return visitor->visitLiteralExpression(this);
    }

    T value;
};

template <typename T>
class UnaryExpression : public Expression<T> {
public:
    explicit UnaryExpression(Token pOperator, Expression<T>* pRight)
        : operatorToken(pOperator), right(pRight) {}

    T accept(ExpressionVisitor<T>* visitor) override {
        return visitor->visitUnaryExpression(this);
    }

    struct Token operatorToken;
    Expression<T>* right;
};

template <typename T>
class VariableExpression : public Expression<T> {
public:
    explicit VariableExpression(Token name) : Name(name) {}

    T accept(ExpressionVisitor<T>* visitor) override {
        return visitor->visitVariableExpression(this);
    }

    struct Token Name;
};

template <typename T>
class AssignmentExpression : public Expression<T> {
public:
    explicit AssignmentExpression(Token name, Expression<T>* expr) :
        Name(name), Expr(expr) {}

    T accept(ExpressionVisitor<T>* visitor) override {
        return visitor->visitAssignmentExpression(this);
    }

    struct Token Name;
    Expression<T>* Expr;
};
/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/
#pragma once
#include <Lex.hpp>

template <typename T>
class BinaryExpression;

template <typename T>
class UnaryExpression;

template <typename T>
class GroupingExpression;

template <typename T>
class LiteralExpression;


template <typename T>
class Visitor {
public:
    Visitor() {}
    virtual ~Visitor() {};
    virtual T dummy() = 0;
    virtual T visitBinaryExpression(BinaryExpression<T>* expr) = 0;
    virtual T visitGroupingExpression(GroupingExpression<T>* expr) = 0;
    virtual T visitUnaryExpression(UnaryExpression<T>* expr) = 0;
    virtual T visitLiteralExpression(LiteralExpression<T>* expr) = 0;
};

template <typename T>
class Expression {
public:
    Expression() {}
    Expression(const Expression&) = delete;
    Expression& operator=(const Expression&) = delete;
    virtual ~Expression() = default;
    virtual T accept(Visitor<T>* visitor) {
        return visitor->dummy();
    };
};

template <typename T>
class BinaryExpression : public Expression<T> {
public:
    BinaryExpression(Expression<T>* pLeft, Token pOperator, Expression<T>* pRight)
        : left(pLeft), operatorToken(pOperator), right(pRight) {}

    T accept(Visitor<T>* visitor) {
        return visitor->visitBinaryExpression(this);
    }

    Expression<T>* left;
    Expression<T>* right;
    struct Token operatorToken;
};

template <typename T>
class GroupingExpression : public Expression<T> {
public:
    GroupingExpression(Expression<T>* pExpression)
        : expression(pExpression) {}

    T accept(Visitor<T>* visitor) {
        return visitor->visitGroupingExpression(this);
    }

    Expression<T>* expression;
};

template <typename T>
class LiteralExpression : public Expression<T> {
public:
    LiteralExpression(T _value) : value(_value) {}

    T accept(Visitor<T>* visitor) {
        return visitor->visitLiteralExpression(this);
    }

    T value;
};

template <typename T>
class UnaryExpression : public Expression<T> {
public:
    UnaryExpression(Token pOperator, Expression<T>* pRight)
        : operatorToken(pOperator), right(pRight) {}

    T accept(Visitor<T>* visitor) {
        return visitor->visitUnaryExpression(this);
    }

    struct Token operatorToken;
    Expression<T>* right;
};


class TreePrinter : public Visitor<Object> {
public:
    ~TreePrinter() {}

    Object dummy() { return Object::Null(); }

    Object print(Expression<Object>* expr);

    Object visitBinaryExpression(BinaryExpression<Object>* expr);

    Object visitGroupingExpression(GroupingExpression<Object>* expr);

    Object visitLiteralExpression(LiteralExpression<Object>* expr);

    Object visitUnaryExpression(UnaryExpression<Object>* expr);
private:
    template <class ... Args>
    std::string parenthesize(std::string Header, Args ... args);
};


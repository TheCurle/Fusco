/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#include <Lex.hpp>
#include <AST.hpp>

template <typename T>
class Expression{
    virtual ~Expression() = default;
    virtual T accept(Visitor<T>) = 0;
};

template <typename T>
class BinaryExpression : public Expression<T> {
public:
    BinaryExpression(Expression<T> left, Token operator, Expression<T> right) {
        this.left = left;
        this.operator = operator;
        this.right = right;
    }

    T accept(Visitor<T> visitor) {
        return visitor.visitBinaryExpression(this);
    }
private:
    Expression<T> left;
    Expression<T> right;
    Token operator;
};

template <typename T>
class GroupingExpression : public Expression<T> {
public:
    GroupingExpression(Expression<T> expression) {
        this.expression = expression;
    }

    T accept(Visitor<T> visitor) {
        return visitor.visitGroupingExpression(this);
    }
private:
    Expression<T> expression;
};

template <typename T>
class LiteralExpression : public Expression<T> {
public:
    LiteralExpression(T _value) : value(_value) {}

    T accept(Visitor<T> visitor) {
        return visitor.visitLiteralExpression(this);
    }
private:
    T value;
};

template <typename T>
class UnaryExpression : public Expression<T> {
public:
    UnaryExpression(Token operator, Expression<T> right) {
        this.operator = operator;
        this.right = right;
    }

    T accept(Visitor<T> visitor) {
        return visitor.visitUnaryExpression(this);
    }
private:
    Token operator;
    Expression<T> right;
};

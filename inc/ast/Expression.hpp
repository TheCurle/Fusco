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
class LogicalExpression;

template <typename T>
class CallExpression;


template <typename T>
class ExpressionVisitor {
public:
    ExpressionVisitor() {}
    virtual ~ExpressionVisitor() {};
    virtual T dummy() = 0;
    virtual T visitBinaryExpression(BinaryExpression<T> &expr) = 0;
    virtual T visitGroupingExpression(GroupingExpression<T> &expr) = 0;
    virtual T visitUnaryExpression(UnaryExpression<T> &expr) = 0;
    virtual T visitLiteralExpression(LiteralExpression<T> &expr) = 0;
    virtual T visitVariableExpression(VariableExpression<T> &expr) = 0;
    virtual T visitAssignmentExpression(AssignmentExpression<T> &expr) = 0;
    virtual T visitLogicalExpression(LogicalExpression<T> &expr) = 0;
    virtual T visitCallExpression(CallExpression<T> &expr) = 0;
};

template <typename T>
class Expression : std::enable_shared_from_this<Expression<T>> {
public:
    Expression() {}
    Expression(const Expression&) = delete;
    Expression& operator=(const Expression&) = delete;
    virtual ~Expression() = default;
    virtual T accept(shared_ptr<ExpressionVisitor<T>> visitor) {
        return visitor->dummy();
    };
};

template <typename T>
class BinaryExpression : public Expression<T> {
public:
    explicit BinaryExpression(shared_ptr<Expression<T>> pLeft, Token pOperator, shared_ptr<Expression<T>> pRight)
        : left(pLeft), right(pRight), operatorToken(pOperator) {}

    T accept(shared_ptr<ExpressionVisitor<T>> visitor) override {
        return visitor->visitBinaryExpression(*this);
    }

    shared_ptr<Expression<T>> left;
    shared_ptr<Expression<T>> right;
    struct Token operatorToken;
};

template <typename T>
class GroupingExpression : public Expression<T> {
public:
    explicit GroupingExpression(shared_ptr<Expression<T>> pExpression)
        : expression(pExpression) {}

    T accept(shared_ptr<ExpressionVisitor<T>> visitor) override {
        return visitor->visitGroupingExpression(*this);
    }

    shared_ptr<Expression<T>> expression;
};

template <typename T>
class LiteralExpression : public Expression<T> {
public:
    explicit LiteralExpression(T _value) : value(_value) {}

    T accept(shared_ptr<ExpressionVisitor<T>> visitor) override {
        return visitor->visitLiteralExpression(*this);
    }

    T value;
};

template <typename T>
class UnaryExpression : public Expression<T> {
public:
    explicit UnaryExpression(Token pOperator, shared_ptr<Expression<T>> pRight)
        : operatorToken(pOperator), right(pRight) {}

    T accept(shared_ptr<ExpressionVisitor<T>> visitor) override {
        return visitor->visitUnaryExpression(*this);
    }

    struct Token operatorToken;
    shared_ptr<Expression<T>> right;
};

template <typename T>
class VariableExpression : public Expression<T> {
public:
    explicit VariableExpression(Token name) : Name(name) {}

    T accept(shared_ptr<ExpressionVisitor<T>> visitor) override {
        return visitor->visitVariableExpression(*this);
    }

    struct Token Name;
};

template <typename T>
class AssignmentExpression : public Expression<T> {
public:
    explicit AssignmentExpression(Token name, shared_ptr<Expression<T>> expr) :
        Name(name), Expr(expr) {}

    T accept(shared_ptr<ExpressionVisitor<T>> visitor) override {
        return visitor->visitAssignmentExpression(*this);
    }

    struct Token Name;
    shared_ptr<Expression<T>> Expr;
};

template <typename T>
class LogicalExpression : public Expression<T> {
public:
    explicit LogicalExpression(shared_ptr<Expression<T>> pLeft, Token pOperator, shared_ptr<Expression<T>> pRight)
         : Left(pLeft), operatorToken(pOperator), Right(pRight) {}

    T accept(shared_ptr<ExpressionVisitor<T>> visitor) override {
        return visitor->visitLogicalExpression(*this);
    }

    shared_ptr<Expression<T>> Left;
    Token operatorToken;
    shared_ptr<Expression<T>> Right;
};

template <typename T>
class CallExpression : public Expression<T> {
public:
    explicit CallExpression(shared_ptr<Expression<T>> pCallee, Token pParenthesis, std::vector<shared_ptr<Expression<T>>> pArguments)
        : Callee(pCallee), Parenthesis(pParenthesis), Arguments(pArguments) {}

    T accept(shared_ptr<ExpressionVisitor<T>> visitor) override {
        return visitor->visitCallExpression(*this);
    }

    shared_ptr<Expression<T>> Callee;
    Token Parenthesis;
    std::vector<shared_ptr<Expression<T>>> Arguments;
};
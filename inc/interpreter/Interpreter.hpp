/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/
#pragma once
#include <ast/Expression.hpp>

class Interpreter : public Visitor<Object>, public Common {
public:
    ~Interpreter() {}

    void Interpret(Expression<Object>* expr);

    Object dummy() { return Object::Null; }

    Object print(Expression<Object>* expr);

    Object visitBinaryExpression(BinaryExpression<Object>* expr);

    Object visitGroupingExpression(GroupingExpression<Object>* expr);

    Object visitLiteralExpression(LiteralExpression<Object>* expr);

    Object visitUnaryExpression(UnaryExpression<Object>* expr);
private:

    Object Evaluate(Expression<Object>* expr);

    std::string Stringify(Object obj);

    bool Truthy(Object obj);
    bool IsEqual(Object a, Object b);

    void CheckOperand(struct Token operatorToken, Object operand);
    void CheckOperands(struct Token operatorToken, Object left, Object right);
};
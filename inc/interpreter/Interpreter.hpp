/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/
#pragma once
#include <ast/Statement.hpp>

class Interpreter : public ExpressionVisitor<Object>,
                    public StatementVisitor,
                    public Common {
public:
    ~Interpreter() {}

    void Interpret(std::vector<Statement*> expr);

    Object dummy() { return Object::Null; }

    Object print(Expression<Object>* expr);

    void visitExpression(ExpressionStatement* stmt);

    void visitPrint(PrintStatement* stmt);

    Object visitBinaryExpression(BinaryExpression<Object>* expr);

    Object visitGroupingExpression(GroupingExpression<Object>* expr);

    Object visitLiteralExpression(LiteralExpression<Object>* expr);

    Object visitUnaryExpression(UnaryExpression<Object>* expr);
private:

    void Execute(Statement* stmt);
    Object Evaluate(Expression<Object>* expr);

    std::string Stringify(Object obj);

    bool Truthy(Object obj);
    bool IsEqual(Object a, Object b);

    void CheckOperand(struct Token operatorToken, Object operand);
    void CheckOperands(struct Token operatorToken, Object left, Object right);
};


class TreePrinter : public ExpressionVisitor<Object>,
                    public StatementVisitor {
public:
    ~TreePrinter() {}

    Object dummy() { return Object::Null; }

    Object print(std::vector<Statement*> stmt);

    void visitExpression(ExpressionStatement* stmt) override;

    void visitPrint(PrintStatement* stmt) override;

    Object visitBinaryExpression(BinaryExpression<Object>* expr);

    Object visitGroupingExpression(GroupingExpression<Object>* expr);

    Object visitLiteralExpression(LiteralExpression<Object>* expr);

    Object visitUnaryExpression(UnaryExpression<Object>* expr);
private:
    template <class ... Args>
    std::string parenthesize(std::string Header, Args ... args);
};
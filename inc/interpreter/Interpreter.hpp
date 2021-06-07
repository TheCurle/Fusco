/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/
#pragma once
#include <map>
#include <algorithm>
#include <ast/Statement.hpp>

class ExecutionContext {
    public:

    Object get(struct Token name) {
        try {
            return ObjectMap.at(name.Lexeme);
        } catch (std::out_of_range &e) {
            throw RuntimeError(name, std::string("Undefined variable ").append(name.Lexeme));
        }
    }

    void define(std::string name, Object obj) {
        ObjectMap.emplace(name, obj);
    }

    void assign(Token name, Object value) {
        std::map<std::string, Object>::iterator it =
            ObjectMap.find(name.Lexeme);

        if (it != ObjectMap.end())
            it->second = value;
        else
            throw RuntimeError(name, std::string("Undefined variable '").append(name.Lexeme).append("'"));
    }

    private:
    std::map<std::string, Object> ObjectMap;
};

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

    void visitVariable(VariableStatement* stmt);

    Object visitBinaryExpression(BinaryExpression<Object>* expr);

    Object visitGroupingExpression(GroupingExpression<Object>* expr);

    Object visitLiteralExpression(LiteralExpression<Object>* expr);

    Object visitVariableExpression(VariableExpression<Object>* expr);

    Object visitAssignmentExpression(AssignmentExpression<Object>* expr);

    Object visitUnaryExpression(UnaryExpression<Object>* expr);
private:
    ExecutionContext Environment;

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

    void visitVariable(VariableStatement* stmt) override;

    Object visitBinaryExpression(BinaryExpression<Object>* expr);

    Object visitGroupingExpression(GroupingExpression<Object>* expr);

    Object visitLiteralExpression(LiteralExpression<Object>* expr);

    Object visitVariableExpression(VariableExpression<Object>* expr);

    Object visitAssignmentExpression(AssignmentExpression<Object>* expr);

    Object visitUnaryExpression(UnaryExpression<Object>* expr);
private:
    template <class ... Args>
    std::string parenthesize(std::string Header, Args ... args);
};
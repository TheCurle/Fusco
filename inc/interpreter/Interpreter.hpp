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

    ExecutionContext() {
        Enclosing = nullptr;
    }

    ExecutionContext(ExecutionContext* inner) {
        Enclosing = inner;
    }

    Object get(struct Token name) {
        std::map<std::string, Object>::iterator it = ObjectMap.find(name.Lexeme);
        if(it != ObjectMap.end()) // If variable not in current context, search deeper
            return ObjectMap.at(name.Lexeme);

        if(Enclosing != nullptr)
            return Enclosing->get(name);

        throw RuntimeError(name, std::string("Unable to find variable ").append(name.Lexeme));
    }

    void define(struct Token name, Object obj) {
        std::map<std::string, Object>::iterator it =
            ObjectMap.find(name.Lexeme);

        if (it != ObjectMap.end())
            throw RuntimeError(name, std::string("Redefinition of variable ").append(name.Lexeme));

        ObjectMap.emplace(name.Lexeme, obj);
    }

    void assign(Token name, Object value) {
        std::map<std::string, Object>::iterator it = ObjectMap.find(name.Lexeme);
        if(it != ObjectMap.end()) { // If variable not in current context, search deeper
            it->second = value;
        }

        if(Enclosing != nullptr) {
            Enclosing->assign(name, value);
        }
    }

    private:
    ExecutionContext* Enclosing;
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

    void visitIf(IfStatement* stmt);

    void visitWhile(WhileStatement* stmt);

    void visitBlock(BlockStatement* stmt);

    Object visitBinaryExpression(BinaryExpression<Object>* expr);

    Object visitGroupingExpression(GroupingExpression<Object>* expr);

    Object visitLiteralExpression(LiteralExpression<Object>* expr);

    Object visitVariableExpression(VariableExpression<Object>* expr);

    Object visitAssignmentExpression(AssignmentExpression<Object>* expr);

    Object visitUnaryExpression(UnaryExpression<Object>* expr);

    Object visitLogicalExpression(LogicalExpression<Object>* expr);
private:
    ExecutionContext Environment;

    void Execute(Statement* stmt);
    void ExecuteBlock(std::vector<Statement*> statements, ExecutionContext environment);

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

    void visitIf(IfStatement* stmt) override;

    void visitWhile(WhileStatement* stmt);

    void visitBlock(BlockStatement* stmt) override;

    Object visitBinaryExpression(BinaryExpression<Object>* expr);

    Object visitGroupingExpression(GroupingExpression<Object>* expr);

    Object visitLiteralExpression(LiteralExpression<Object>* expr);

    Object visitVariableExpression(VariableExpression<Object>* expr);

    Object visitAssignmentExpression(AssignmentExpression<Object>* expr);

    Object visitUnaryExpression(UnaryExpression<Object>* expr);

    Object visitLogicalExpression(LogicalExpression<Object>* expr);
private:
    template <class ... Args>
    std::string parenthesize(std::string Header, Args ... args);
};
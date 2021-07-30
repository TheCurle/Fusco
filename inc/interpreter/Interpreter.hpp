/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/
#pragma once
#include <memory>
#include <map>
#include <algorithm>
#include <ast/Statement.hpp>
#include <interpreter/Globals.hpp>

using std::shared_ptr;

class ExecutionContext : public std::enable_shared_from_this<ExecutionContext> {
    public:

    ExecutionContext() {
        Enclosing = nullptr;
    }

    ExecutionContext(shared_ptr<ExecutionContext> inner) {
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
    shared_ptr<ExecutionContext> Enclosing;
    std::map<std::string, Object> ObjectMap;
};

class Interpreter : public ExpressionVisitor<Object>,
                    public StatementVisitor,
                    public Common,
                    public std::enable_shared_from_this<Interpreter> {
public:
    ~Interpreter() {}

    Interpreter(shared_ptr<ExecutionContext> spark) {
        Globals = spark;
        Token getTimeName;
        getTimeName.Lexeme = "getTime";
        Globals->define(getTimeName, Object::NewCallable(std::make_shared<GetTime>()));

        Environment = Globals;
    }

    shared_ptr<ExecutionContext> Globals;

    void Interpret(std::vector<shared_ptr<Statement>> expr);

    void ExecuteBlock(std::vector<shared_ptr<Statement>> statements, shared_ptr<ExecutionContext> environment);

    Object dummy() override { return Object::Null; }

    Object print(shared_ptr<Expression<Object>> expr);

    void visitExpression(ExpressionStatement &stmt) override;

    void visitPrint(PrintStatement &stmt) override;

    void visitVariable(VariableStatement &stmt) override;

    void visitIf(IfStatement &stmt) override;

    void visitWhile(WhileStatement &stmt) override;

    void visitBlock(BlockStatement &stmt) override;

    void visitFunc(FuncStatement &stmt) override;

    void visitReturn(ReturnStatement &stmt) override;

    Object visitBinaryExpression(BinaryExpression<Object> &expr) override;

    Object visitGroupingExpression(GroupingExpression<Object> &expr) override;

    Object visitLiteralExpression(LiteralExpression<Object> &expr) override;

    Object visitVariableExpression(VariableExpression<Object> &expr) override;

    Object visitAssignmentExpression(AssignmentExpression<Object> &expr) override;

    Object visitUnaryExpression(UnaryExpression<Object> &expr) override;

    Object visitCallExpression(CallExpression<Object> &expr) override;

    Object visitLogicalExpression(LogicalExpression<Object> &expr) override;
private:

    shared_ptr<ExecutionContext> Environment;

    void Execute(shared_ptr<Statement> stmt);

    Object Evaluate(shared_ptr<Expression<Object>> expr);

    std::string Stringify(Object obj);

    bool Truthy(Object obj);
    bool IsEqual(Object a, Object b);

    void CheckOperand(struct Token operatorToken, Object operand);
    void CheckOperands(struct Token operatorToken, Object left, Object right);
};


class TreePrinter : public ExpressionVisitor<Object>,
                    public StatementVisitor,
                    public std::enable_shared_from_this<TreePrinter> {
public:
    ~TreePrinter() {}

    Object dummy() override { return Object::Null; }

    Object print(std::vector<shared_ptr<Statement>> stmt);

    void visitExpression(ExpressionStatement &stmt) override;

    void visitPrint(PrintStatement &stmt) override;

    void visitVariable(VariableStatement &stmt) override;

    void visitIf(IfStatement &stmt) override;

    void visitWhile(WhileStatement &stmt) override;

    void visitBlock(BlockStatement &stmt) override;

    void visitFunc(FuncStatement &stmt) override;

    void visitReturn(ReturnStatement &stmt) override;

    Object visitBinaryExpression(BinaryExpression<Object> &expr) override;

    Object visitGroupingExpression(GroupingExpression<Object> &expr) override;

    Object visitLiteralExpression(LiteralExpression<Object> &expr) override;

    Object visitVariableExpression(VariableExpression<Object> &expr) override;

    Object visitAssignmentExpression(AssignmentExpression<Object> &expr) override;

    Object visitUnaryExpression(UnaryExpression<Object> &expr) override;

    Object visitCallExpression(CallExpression<Object> &expr) override;

    Object visitLogicalExpression(LogicalExpression<Object> &expr) override;
private:
    template <class ... Args>
    std::string parenthesize(std::string Header, Args ... args);
};


class Function : public Callable {
public:
    Function(shared_ptr<FuncStatement> pDeclaration, shared_ptr<ExecutionContext> pClosure)
        : Declaration(pDeclaration), Closure(pClosure) {}

    Object call(shared_ptr<Interpreter> interpreter, std::vector<Object> params) {
        shared_ptr<ExecutionContext> environment = std::make_shared<ExecutionContext>(Closure);
        for(size_t i = 0; i < Declaration->Params.size(); i++) {
            environment->define(Declaration->Params.at(i),
                params.at(i));
        }

        try {
            interpreter->ExecuteBlock(Declaration->Body, environment);
        } catch (Return &value) {
            return value.Value;
        }
        return Object::Null;
    }

    size_t arguments() {
        return Declaration->Params.size();
    }

    shared_ptr<FuncStatement> Declaration;
    shared_ptr<ExecutionContext> Closure;
};
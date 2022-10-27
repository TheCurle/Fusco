/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#include <interpreter/Interpreter.hpp>
#include <iostream>

void Resolver::resolve(std::shared_ptr<Statement> statement) {
    statement->accept(shared_from_this());
}

void Resolver::resolve(EXPR expr) {
    expr->accept(shared_from_this());
}

void Resolver::resolveAll(std::vector<std::shared_ptr<Statement>> statements) {
    for(std::shared_ptr<Statement> statement : statements) {
        resolve(statement);
    }
}

void Resolver::beginScope() {
    scopes.emplace_back(std::map<std::string, bool>());
}

void Resolver::endScope() {
    (void) scopes.pop_back();
}

void Resolver::declare(Token name) {
    if(scopes.empty()) return;

    if(scopes.back().find(name.Lexeme) != scopes.back().end())
        throw Error(RuntimeError(name, "Variable cannot be declared twice in scope."));

    scopes.back().emplace(name.Lexeme, false);
}

void Resolver::define(Token name) {
    if(scopes.empty()) return;

    scopes.back().emplace(name.Lexeme, true);
}


void Resolver::resolveLocal(Expression<Object>* expr, Token name) {
    for (int i = scopes.size() - 1; i >= 0; i--) {
        if(scopes.at(i).find(name.Lexeme) != scopes.at(i).end()) {
            interpreter->resolve(expr, scopes.size() - i - 1);
            return;
        }
    }
}

void Resolver::visitExpression(ExpressionStatement &stmt) {
    resolve(stmt.Expr);
}

void Resolver::visitPrint(PrintStatement &stmt) {
    resolve(stmt.Expr);
}

void Resolver::visitVariable(VariableStatement &stmt) {
    declare(stmt.Name);
    if(stmt.Expr != nullptr) {
        resolve(stmt.Expr);
    }
    define(stmt.Name);
}

void Resolver::visitIf(IfStatement &stmt) {
    resolve(stmt.Condition);
    resolve(stmt.Then);
    if (stmt.Else != nullptr) resolve(stmt.Else);
}

void Resolver::visitWhile(WhileStatement &stmt) {
    resolve(stmt.Condition);
    resolve(stmt.Body);
}

void Resolver::visitBlock(BlockStatement &stmt) {
    beginScope();
    resolveAll(stmt.Statements);
    endScope();
}

void Resolver::visitFunc(FuncStatement &stmt) {
    declare(stmt.Name);
    define(stmt.Name);

    resolveFunction(stmt, FunctionType::FUNCTION);
}

void Resolver::visitClass(ClassStatement &stmt) {
    ClassType enclosingClass = currentClass;
    currentClass = ClassType::CLASS;

    declare(stmt.name);
    define(stmt.name);

    beginScope();
    scopes.back().emplace("this", true);

    for (const shared_ptr<FuncStatement>& func : stmt.functions) {
        FunctionType decl = FunctionType::MEMBER;
        resolveFunction(*func, decl);
    }

    endScope();
    currentClass = enclosingClass;
}

void Resolver::resolveFunction(FuncStatement &stmt, FunctionType type) {
    FunctionType enclosingType = currentFunction;
    currentFunction = type;

    beginScope();
    for(Token param : stmt.Params) {
        declare(param);
        define(param);
    }
    resolveAll(stmt.Body);
    endScope();

    currentFunction = enclosingType;
}

void Resolver::visitReturn(ReturnStatement &stmt) {
    if (currentFunction == FunctionType::F_NONE)
        throw Error(RuntimeError(stmt.Keyword, "Unable to return from the global scope."));

    if(stmt.Value != nullptr) 
        resolve(stmt.Value);
}

Object Resolver::visitBinaryExpression(BinaryExpression<Object> &expr) {
    resolve(expr.left);
    resolve(expr.right);
    return Object::Null;
}

Object Resolver::visitGroupingExpression(GroupingExpression<Object> &expr) {
    resolve(expr.expression);
    
    return Object::Null;
}

Object Resolver::visitLiteralExpression(LiteralExpression<Object> &expr) {
    UNUSED(expr);
    return Object::Null;
}

Object Resolver::visitVariableExpression(VariableExpression<Object> &expr) {
    if(!scopes.empty() && scopes.back().at(expr.Name.Lexeme) == false) {
        throw Error(RuntimeError(expr.Name, "Attempted to read a variable in its own initializer"));
    }

    resolveLocal(&expr, expr.Name);

    return Object::Null;
}

Object Resolver::visitAssignmentExpression(AssignmentExpression<Object> &expr) {
    resolve(expr.Expr);
    resolveLocal(&expr, expr.Name);

    return Object::Null;
}

Object Resolver::visitUnaryExpression(UnaryExpression<Object> &expr) {
    resolve(expr.right);
    
    return Object::Null;
}

Object Resolver::visitCallExpression(CallExpression<Object> &expr) {
    resolve(expr.Callee);

    for (std::shared_ptr<Expression<Object>> arg : expr.Arguments) {
        resolve(arg);
    }
    
    return Object::Null;
}

Object Resolver::visitLogicalExpression(LogicalExpression<Object> &expr) {
    resolve(expr.Left);
    resolve(expr.Right);
    
    return Object::Null;
}

Object Resolver::visitGetExpression(GetExpression<Object> &expr) {
    resolve(expr.Obj);
    return Object::Null;
}

Object Resolver::visitSetExpression(SetExpression<Object> &expr) {
    resolve(expr.Value);
    resolve(expr.Obj);
    return Object::Null;
}

Object Resolver::visitThisExpression(ThisExpression<Object> &expr) {
    if (currentClass == ClassType::C_NONE) {
        Error(expr.Name, "Cannot use \"this\" outside of a Class.");
    }

    resolveLocal(&expr, expr.Name);
    return Object::Null;
}


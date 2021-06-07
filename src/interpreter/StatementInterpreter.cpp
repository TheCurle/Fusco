/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#include <iostream>
#include <interpreter/Interpreter.hpp>
#include <interpreter/Errors.hpp>

void Interpreter::Interpret(std::vector<Statement*> statements) {
    try {
        for(auto value: statements) {
            Execute(value);
        }
    } catch (RuntimeError &e) {
        Error(e);
    }
}

void Interpreter::Execute(Statement* stmt) {
    stmt->accept(this);
}

void Interpreter::visitExpression(ExpressionStatement* stmt) {
    Evaluate(stmt->Expr);
}

void Interpreter::visitPrint(PrintStatement* stmt) {
    Object value = Evaluate(stmt->Expr);
    std::cout << "% " << Stringify(value) << std::endl;
}

void Interpreter::visitVariable(VariableStatement* stmt) {
    Object value = Object::Null;
    if(stmt->Expr != nullptr) {
        value = Evaluate(stmt->Expr);
    }

    Environment.define(stmt->Name, value);
}

void Interpreter::visitBlock(BlockStatement* stmt) {
    ExecuteBlock(stmt->Statements, new ExecutionContext(Environment));
}

void Interpreter::ExecuteBlock(std::vector<Statement*> statements, ExecutionContext environment) {
    ExecutionContext previous = this->Environment;

    this->Environment = environment;

    for(Statement* stmt : statements) {
        Execute(stmt);
    }

    this->Environment = previous;
}

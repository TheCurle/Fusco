/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#include <iostream>
#include <interpreter/Interpreter.hpp>
#include <interpreter/Errors.hpp>

void Interpreter::resolve(Expression<Object>* expr, int depth) {
    Locals.at(expr) = depth;
}

void Interpreter::Interpret(std::vector<shared_ptr<Statement>> statements) {
    try {
        for(auto value: statements) {
            Execute(value);
        }
    } catch (RuntimeError &e) {
        std::cout << e.Message << ": " << e.Cause.Lexeme << std::endl;
    }
}

void Interpreter::Execute(shared_ptr<Statement> stmt) {
    stmt->accept(shared_from_this());
}

void Interpreter::visitExpression(ExpressionStatement &stmt) {
    Evaluate(stmt.Expr);
}

void Interpreter::visitPrint(PrintStatement &stmt) {
    Object value = Evaluate(stmt.Expr);
    std::cout << "% " << Stringify(value) << std::endl;
}

void Interpreter::visitVariable(VariableStatement &stmt) {
    Object value = Object::Null;
    if(stmt.Expr != nullptr) {
        value = Evaluate(stmt.Expr);
    }

    Environment->define(stmt.Name, value);
}

void Interpreter::visitIf(IfStatement &stmt) {
    if(Truthy(Evaluate(stmt.Condition)))
        Execute(stmt.Then);
    else if(stmt.Else != nullptr) {
        Execute(stmt.Else);
    }
}

void Interpreter::visitWhile(WhileStatement &stmt) {
    while(Truthy(Evaluate(stmt.Condition))) {
        Execute(stmt.Body);
    }
}

void Interpreter::visitFunc(FuncStatement &stmt) {
    shared_ptr<Function> func = std::make_shared<Function>(std::make_shared<FuncStatement>(stmt), Environment);
    Environment->define(stmt.Name, Object::NewCallable(func));
}

void Interpreter::visitReturn(ReturnStatement &stmt) {
    Object value = Object::Null;

    if(stmt.Value != nullptr)
        value = Evaluate(stmt.Value);

    throw Return(value);
}

void Interpreter::visitBlock(BlockStatement &stmt) {
    ExecuteBlock(stmt.Statements, std::make_shared<ExecutionContext>(Environment));
}

void Interpreter::ExecuteBlock(std::vector<shared_ptr<Statement>> statements, shared_ptr<ExecutionContext> environment) {
    shared_ptr<ExecutionContext> previous = this->Environment;

    this->Environment = environment;
	
	try {
		for(shared_ptr<Statement> stmt : statements) {
			Execute(stmt);
		}
	} catch (Return &r) {
		this->Environment = previous; // C++ try-finally when?
		throw r;
	}

    this->Environment = previous;
}
/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#include <string>
#include <cstdarg>
#include <initializer_list>
#include <interpreter/Interpreter.hpp>

Object TreePrinter::print(std::vector<Statement*> stmts) {
    for(auto stmt : stmts) {
        stmt->accept(this);
    }

    std::cout << std::endl;

    return Object::Null;
}


void TreePrinter::visitExpression(ExpressionStatement* stmt) {
    std::cout << "Input resolves to:\t" << parenthesize("expression", &stmt->Expr) << std::endl;
}

void TreePrinter::visitPrint(PrintStatement* stmt) {
    std::cout << "Input resolves to:\t" << parenthesize("print", &stmt->Expr) << std::endl;
}

void TreePrinter::visitVariable(VariableStatement* stmt) {
    std::cout << "Input resolves to:\t" << parenthesize(std::string("(variable ").append(stmt->Name.Lexeme).append(")"), &stmt->Expr) << std::endl;
}

void TreePrinter::visitIf(IfStatement* stmt) {
    std::cout << "Conditional branch:\n\tCondition: -> " << parenthesize("", &stmt->Condition) << std::endl;
    std::cout << "\tThen: -> ";
    stmt->Then->accept(this);
    std::cout << "\tElse: -> ";
    if(stmt->Else != nullptr)
        stmt->Else->accept(this);
    else
        std::cout << "Not specified";
    std::cout << std::endl;
}

void TreePrinter::visitBlock(BlockStatement* stmt) {
    std::cout << "Block starts:" << std::endl;
    for(Statement* inner : stmt->Statements) {
        std::cout << "\t -> ";
        inner->accept(this);
    }
    std::cout << "\t -> Block ends." << std::endl;
}

Object TreePrinter::visitBinaryExpression(BinaryExpression<Object>* expr) {
    return Object::NewStr(parenthesize(expr->operatorToken.Lexeme, &expr->left, &expr->right));
}

Object TreePrinter::visitGroupingExpression(GroupingExpression<Object>* expr) {
    return Object::NewStr(parenthesize("group", &expr->expression));
}

Object TreePrinter::visitLiteralExpression(LiteralExpression<Object>* expr) {
    if (expr->value.Type == Object::NullType)
        return Object::Null;
    return expr->value;
}

Object TreePrinter::visitVariableExpression(VariableExpression<Object>* expr) {
    return Object::NewStr(std::string("(var ").append(expr->Name.Lexeme).append(")"));
}

Object TreePrinter::visitAssignmentExpression(AssignmentExpression<Object>* expr) {
    return Object::NewStr(parenthesize(std::string("(assign ").append(expr->Name.Lexeme).append(")"), &expr->Expr));
}

Object TreePrinter::visitUnaryExpression(UnaryExpression<Object>* expr) {
    return Object::NewStr(parenthesize(expr->operatorToken.Lexeme, &expr->right));
}

Object TreePrinter::visitLogicalExpression(LogicalExpression<Object>* expr) {
    return Object::NewStr(parenthesize(expr->operatorToken.Lexeme, &expr->Left, &expr->Right));
}

template <class... Args>
std::string TreePrinter::parenthesize(std::string Header, Args... args) {
    std::string builder("(");
    builder.append(Header);

    std::vector<Expression<Object>*> vec;

    (vec.push_back(*args), ...);

    for(auto value: vec) {
        builder.append(" ").append(value->accept(this).ToString());
    }

    builder.append(")");

    return builder;
}

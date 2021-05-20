/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#include <string>
#include <cstdarg>
#include <initializer_list>
#include <AST.hpp>
#include <ast/Expression.hpp>

std::string TreePrinter::print(Expression<std::string>* expr) {
    return expr->accept(this);
}

std::string TreePrinter::visitBinaryExpression(BinaryExpression<std::string>* expr) {
    return parenthesize(expr->operatorToken.Lexeme, expr->left, expr->right);
}

std::string TreePrinter::visitGroupingExpression(GroupingExpression<std::string>* expr) {
    return parenthesize("group", expr->expression);
}

std::string TreePrinter::visitLiteralExpression(LiteralExpression<std::string>* expr) {
    if (expr->value.empty())
        return "null";
    return expr->value;
}

std::string TreePrinter::visitUnaryExpression(UnaryExpression<std::string>* expr) {
    return parenthesize(expr->operatorToken.Lexeme, expr->right);
}

template <class... Args>
std::string TreePrinter::parenthesize(std::string Header, Args... args) {
    std::string builder("(");
    builder.append(Header);

    std::vector<Expression<std::string>> vec;

    (vec.push_back(args), ...);

    for(const auto value: vec) {
        builder.append(" ").append(value.accept((Visitor*) this));
    }

    builder.append(")");

    return builder;
}

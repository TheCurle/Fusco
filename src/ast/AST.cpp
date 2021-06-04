/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#include <string>
#include <cstdarg>
#include <initializer_list>
#include <ast/Expression.hpp>

Object TreePrinter::print(Expression<Object>* expr) {
    return expr->accept(this);
}

Object TreePrinter::visitBinaryExpression(BinaryExpression<Object>* expr) {
    return Object::NewStr(parenthesize(expr->operatorToken.Lexeme, &expr->left, &expr->right));
}

Object TreePrinter::visitGroupingExpression(GroupingExpression<Object>* expr) {
    return Object::NewStr(parenthesize("group", &expr->expression));
}

Object TreePrinter::visitLiteralExpression(LiteralExpression<Object>* expr) {
    if (expr->value.Type == Object::NullType)
        return Object::Null();
    return expr->value;
}

Object TreePrinter::visitUnaryExpression(UnaryExpression<Object>* expr) {
    return Object::NewStr(parenthesize(expr->operatorToken.Lexeme, &expr->right));
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

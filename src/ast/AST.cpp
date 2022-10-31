/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#include <string>
#include <interpreter/Interpreter.hpp>

static size_t NestLevel = 0;

static std::string nest(const std::string& input) {
    std::string temp;
    for(size_t i = 0; i < NestLevel; i++)
        temp.append(" ");
    temp.append(input);

    return temp;
}

Object TreePrinter::print(const std::vector<shared_ptr<Statement>>& stmts) {
    std::shared_ptr<TreePrinter> shared = shared_from_this();
    
    for(const auto& stmt : stmts) {
        stmt->accept(shared);
    }

    std::cout << std::endl;

    return Object::Null;
}


void TreePrinter::visitExpression(ExpressionStatement &stmt) {
    std::cout << "Input resolves to:\t" << parenthesize("expression", &stmt.Expr) << std::endl;
}

void TreePrinter::visitPrint(PrintStatement &stmt) {
    std::cout << "Input resolves to:\t" << parenthesize("print", &stmt.Expr) << std::endl;
}

void TreePrinter::visitVariable(VariableStatement &stmt) {
    std::cout << "Input resolves to:\t" << parenthesize(std::string("(variable ").append(stmt.Name.Lexeme).append(")"), &stmt.Expr) << std::endl;
}

void TreePrinter::visitIf(IfStatement &stmt) {
    std::cout << "Conditional branch:\n\tCondition: -> " << parenthesize("", &stmt.Condition) << std::endl;
    std::cout << "\tThen: -> ";
    stmt.Then->accept(shared_from_this());
    std::cout << "\tElse: -> ";
    if(stmt.Else != nullptr)
        stmt.Else->accept(shared_from_this());
    else
        std::cout << "Not specified";
    std::cout << std::endl;
}

void TreePrinter::visitWhile(WhileStatement &stmt) {
    std::cout << "While loop:\n\tCondition: -> " << parenthesize("", &stmt.Condition) << std::endl;
    std::cout << "\tBody:";
    stmt.Body->accept(shared_from_this());
    std::cout << std::endl;
}

void TreePrinter::visitFunc(FuncStatement &stmt) {
    std::cout << std::string("Function ").append(stmt.Name.Lexeme) << std::endl;
    std::cout << "\tParameters: ";
    for(const Token& param : stmt.Params)
        std::cout << std::string(" ").append(param.Lexeme);

    std::cout << std::endl << "\tBody:" << std::endl;

    for(const shared_ptr<Statement>& statement : stmt.Body) {
        std::cout << nest("-> ");
        statement->accept(shared_from_this());
    }
    std::cout << std::endl;
}

void TreePrinter::visitClass(ClassStatement &stmt) {
    std::cout << std::string("Class ").append(stmt.name.Lexeme) << std::endl;
    std::cout << "\tSuper: " + stmt.superclass->Name.Lexeme + "\n";
    std::cout << "\tMethods: ";
    for(const std::shared_ptr<FuncStatement>& method : stmt.functions) {
        std::cout << nest("-> ");
        method->accept(shared_from_this());
    }
    std::cout << std::endl;
}

void TreePrinter::visitReturn(ReturnStatement &stmt) {
    std::cout << parenthesize("return", &stmt.Value) << std::endl;
}

void TreePrinter::visitBlock(BlockStatement &stmt) {
    std::cout << nest("Block starts:") << std::endl;

    NestLevel++;
    for(const shared_ptr<Statement>& inner : stmt.Statements) {
        std::cout << nest("-> ");
        inner->accept(shared_from_this());
    }
    NestLevel--;
    std::cout << nest("-> Block ends.") << std::endl;
}

Object TreePrinter::visitBinaryExpression(BinaryExpression<Object> &expr) {
    return Object::NewStr(parenthesize(expr.operatorToken.Lexeme, &expr.left, &expr.right));
}

Object TreePrinter::visitGroupingExpression(GroupingExpression<Object> &expr) {
    return Object::NewStr(parenthesize("group", &expr.expression));
}

Object TreePrinter::visitLiteralExpression(LiteralExpression<Object> &expr) {
    if (expr.value.Type == Object::NullType)
        return Object::Null;
    return expr.value;
}

Object TreePrinter::visitVariableExpression(VariableExpression<Object> &expr) {
    return Object::NewStr(std::string("(var ").append(expr.Name.Lexeme).append(")"));
}

Object TreePrinter::visitAssignmentExpression(AssignmentExpression<Object> &expr) {
    return Object::NewStr(parenthesize(std::string("(assign ").append(expr.Name.Lexeme).append(")"), &expr.Expr));
}

Object TreePrinter::visitUnaryExpression(UnaryExpression<Object> &expr) {
    return Object::NewStr(parenthesize(expr.operatorToken.Lexeme, &expr.right));
}


Object TreePrinter::visitCallExpression(CallExpression<Object> &expr) {
    EXPR firstArg = nullptr;
    if(!expr.Arguments.empty())
        firstArg = expr.Arguments.at(0);

    if(firstArg != nullptr)
        return Object::NewStr(parenthesize("call", &expr.Callee, &firstArg));

    return Object::NewStr(parenthesize("call", &expr.Callee));
}

Object TreePrinter::visitLogicalExpression(LogicalExpression<Object> &expr) {
    return Object::NewStr(parenthesize(expr.operatorToken.Lexeme, &expr.Left, &expr.Right));
}

Object TreePrinter::visitGetExpression(GetExpression<Object> &expr) {
    return Object::NewStr(parenthesize("get " + expr.Name.Lexeme, &expr.Obj));
}

Object TreePrinter::visitSetExpression(SetExpression<Object> &expr) {
    return Object::NewStr(parenthesize("set " + expr.Name.Lexeme, &expr.Obj, &expr.Value)); 
}

Object TreePrinter::visitThisExpression(ThisExpression<Object> &expr) {
    UNUSED(expr);
    return Object::NewStr("bind of \"this\"");
}

template <class... Args>
std::string TreePrinter::parenthesize(const std::string& Header, Args... args) {
    std::string builder("(");
    builder.append(Header);

    std::vector<shared_ptr<Expression<Object>>> vec;

    (vec.push_back(*args), ...);

    for(const auto& value: vec) {
        if(value != nullptr)
            builder.append(" ").append(value->accept(shared_from_this()).ToString());
    }

    builder.append(")");

    return builder;
}

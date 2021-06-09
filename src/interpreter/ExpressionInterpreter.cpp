/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#include <interpreter/Interpreter.hpp>
#include <interpreter/Errors.hpp>

Object Interpreter::visitBinaryExpression(BinaryExpression<Object>* expr) {
    Object left = Evaluate(expr->left);
    Object right = Evaluate(expr->right);

    //! Prepared to go nuclear with the checks if necessary.
    switch(expr->operatorToken.Type) {
        case AR_MINUS:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewNum(left.Num - right.Num);
        case AR_RSLASH:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewNum(left.Num / right.Num);
        case AR_ASTERISK:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewNum(left.Num * right.Num);
        case AR_PLUS:
            CheckOperands(expr->operatorToken, left, right);
            if(left.Type == Object::NumType && right.Type == Object::NumType)
                return Object::NewNum(left.Num + right.Num);

            if(left.Type == Object::StrType && right.Type == Object::StrType)
                return Object::NewStr(left.Str.append(right.Str));

            if(left.Type == Object::StrType || right.Type == Object::StrType)
                return Object::NewStr(left.ToString().append(right.ToString()));
            break;
        case CMP_GREATER:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewBool(left.Num > right.Num);
        case CMP_GREAT_EQUAL:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewBool(left.Num >= right.Num);
        case CMP_LESS:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewBool(left.Num < right.Num);
        case CMP_LESS_EQUAL:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewBool(left.Num <= right.Num);

        case CMP_EQUAL:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewBool(IsEqual(left, right));
        case CMP_INEQ:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewBool(!IsEqual(left, right));
    }

    return Object::Null;
}

Object Interpreter::visitGroupingExpression(GroupingExpression<Object>* expr) {
    return Evaluate(expr->expression);
}

Object Interpreter::visitLiteralExpression(LiteralExpression<Object>* expr) {
    return expr->value;
}

Object Interpreter::visitVariableExpression(VariableExpression<Object>* expr) {
    return Environment->get(expr->Name);
}

Object Interpreter::visitAssignmentExpression(AssignmentExpression<Object>* expr) {
    Object value = Evaluate(expr->Expr);
    Environment->assign(expr->Name, value);
    return value;
}

Object Interpreter::visitUnaryExpression(UnaryExpression<Object>* expr) {
    Object right = Evaluate(expr->right);

    switch(expr->operatorToken.Type) {
        case AR_MINUS: return Object::NewNum(-(right.Num));
        case BOOL_EXCLAIM: return Object::NewBool(!Truthy(right));
    }

    return Object::Null;
}

Object Interpreter::visitCallExpression(CallExpression<Object>* expr) {
    Object callee = Evaluate(expr->Callee);
    std::vector<Object> arguments;
    for(EXPR argument : expr->Arguments) {
        arguments.emplace_back(Evaluate(argument));
    }

    Object functionHolder = Evaluate(expr->Callee);

    if(functionHolder.Type != Object::FunctionType) {
        throw RuntimeError(expr->Parenthesis, "Unable to call non-function type.");
    }

    Callable* function = functionHolder.Function;

    if(arguments.size() != function->arguments()) {
        std::string message("Expected ");
        message.append(std::to_string(function->arguments())).append(" arguments, got ").append(std::to_string(arguments.size())).append(".");

        throw RuntimeError(expr->Parenthesis, message);
    }

    return function->call(this, arguments);
}

Object Interpreter::visitLogicalExpression(LogicalExpression<Object>* expr) {
    Object left = Evaluate(expr->Left);

    if(expr->operatorToken.Type == KW_OR) {
        if(Truthy(left)) return left;
    } else {
        if(!Truthy(left)) return left;
    }

    return Evaluate(expr->Right);
}

Object Interpreter::Evaluate(Expression<Object>* expr) {
    return expr->accept(this);
}

std::string Interpreter::Stringify(Object obj) {
    return obj.ToString();
}

bool Interpreter::Truthy(Object obj) {
    if(obj.Type == Object::NullType) return false;
    if(obj.Type == Object::BoolType) return obj.Bool;

    return true;
}

bool Interpreter::IsEqual(Object a, Object b) {
    // If both are null, they are equal
    if(a.Type == Object::NullType && b.Type == Object::NullType)
        return true;

    // Null is never equal to anything else
    if(a.Type == Object::NullType)
        return false;

    if(a.Type == b.Type) {
        switch(a.Type) {
            case Object::BoolType:
                return a.Bool == b.Bool;
            case Object::NumType:
                return a.Num == b.Num;
            case Object::StrType:
                return a.Str.compare(b.Str) == 0;
            default:
                return false;
        }
    } else
        return false;
}

void Interpreter::CheckOperand(struct Token operatorToken, Object operand) {
    if(operand.Type == Object::NumType) return;

    throw RuntimeError(operatorToken, "This operation can only be performed on a Number.");
}

void Interpreter::CheckOperands(struct Token operatorToken, Object left, Object right) {
    switch(operatorToken.Type) {
        case AR_PLUS:
            if((left.Type == right.Type && ((left.Type == Object::NumType) || (left.Type == Object::StrType)))
                    || (left.Type == Object::StrType || right.Type == Object::StrType))
                return;
            throw RuntimeError(operatorToken, "Only strings and numbers may be added to each other.");
            break;
    }

    throw RuntimeError(operatorToken, "Unknown syntax error.");
}
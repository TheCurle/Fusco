/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#include <interpreter/Interpreter.hpp>
#include <interpreter/Errors.hpp>

void Interpreter::Interpret(Expression<Object>* Expr) {
    try {
        Object value = Evaluate(Expr);
        std::cout << "$ " << Stringify(value) << std::endl;
    } catch(RuntimeError e) {
        Common::Error(e);
    }
}

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

Object Interpreter::visitUnaryExpression(UnaryExpression<Object>* expr) {
    Object right = Evaluate(expr->right);

    switch(expr->operatorToken.Type) {
        case AR_MINUS: return Object::NewNum(-(right.Num));
        case BOOL_EXCLAIM: return Object::NewBool(!Truthy(right));
    }

    return Object::Null;
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
            if(left.Type == right.Type && ((left.Type == Object::NumType) || (left.Type == Object::StrType)))
                return;
            throw RuntimeError(operatorToken, "Only strings and numbers may be added to each other.");
            break;
    }

    throw RuntimeError(operatorToken, "Unknown syntax error.");
}
/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#include <interpreter/Interpreter.hpp>
#include <utility>

Object Interpreter::lookupVariable(Token name, Expression<Object>* expr) {
    if (Locals.find(expr) != Locals.end())
        return Environment->getAt(Locals.at(expr), name.Lexeme);
    else
        return Globals->get(name);
    
}

Object Interpreter::visitBinaryExpression(BinaryExpression<Object> &expr) {
    Object left = Evaluate(expr.left);
    Object right = Evaluate(expr.right);

    //! Prepared to go nuclear with the checks if necessary.
    switch(expr.operatorToken.Type) {
        case AR_MINUS:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewNum(left.NumData - right.NumData);
        case AR_RSLASH:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewNum(left.NumData / right.NumData);
        case AR_ASTERISK:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewNum(left.NumData * right.NumData);
        case AR_PLUS:
            CheckOperands(expr.operatorToken, left, right);
            if(left.Type == Object::NumType && right.Type == Object::NumType)
                return Object::NewNum(left.NumData + right.NumData);

            if(left.Type == Object::StrType && right.Type == Object::StrType)
                return Object::NewStr(left.StrData.append(right.StrData));

            if(left.Type == Object::StrType || right.Type == Object::StrType)
                return Object::NewStr(left.ToString().append(right.ToString()));
            break;
        case CMP_GREATER:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewBool(left.NumData > right.NumData);
        case CMP_GREAT_EQUAL:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewBool(left.NumData >= right.NumData);
        case CMP_LESS:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewBool(left.NumData < right.NumData);
        case CMP_LESS_EQUAL:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewBool(left.NumData <= right.NumData);

        case CMP_EQUAL:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewBool(IsEqual(left, right));
        case CMP_INEQ:
            //CheckOperands(expr->operatorToken, left, right);
            return Object::NewBool(!IsEqual(left, right));
    }

    return Object::Null;
}

Object Interpreter::visitGroupingExpression(GroupingExpression<Object> &expr) {
    return Evaluate(expr.expression);
}

Object Interpreter::visitLiteralExpression(LiteralExpression<Object> &expr) {
    return expr.value;
}

Object Interpreter::visitVariableExpression(VariableExpression<Object> &expr) {
    return lookupVariable(expr.Name, &expr);
}

Object Interpreter::visitAssignmentExpression(AssignmentExpression<Object> &expr) {
    Object value = Evaluate(expr.Expr);

    if (Locals.find(&expr) != Locals.end())
        Environment->assignAt(Locals.at(&expr), expr.Name, value);
    else
        Globals->assign(expr.Name, value);

    return value;
}

Object Interpreter::visitUnaryExpression(UnaryExpression<Object> &expr) {
    Object right = Evaluate(expr.right);

    switch(expr.operatorToken.Type) {
        case AR_MINUS: return Object::NewNum(-(right.NumData));
        case BOOL_EXCLAIM: return Object::NewBool(!Truthy(right));
    }

    return Object::Null;
}

Object Interpreter::visitCallExpression(CallExpression<Object> &expr) {
    Object callee = Evaluate(expr.Callee);
    std::vector<Object> arguments;
    for(EXPR argument : expr.Arguments) {
        arguments.emplace_back(Evaluate(argument));
    }

    Object functionHolder = Evaluate(expr.Callee);

    if(functionHolder.Type != Object::CallableType && functionHolder.Type != Object::ClassType && functionHolder.Type != Object::MethodType) {
        throw Error(RuntimeError(expr.Parenthesis, "Unable to call non-function type."));
    }

    // If we're trying to execute a function, use the function. Otherwise, we're calling a constructor, so use the containing class.
    shared_ptr<Callable> function = functionHolder.Type == Object::CallableType ? functionHolder.CallableData : functionHolder.Type == Object::MethodType ? functionHolder.CallableData : functionHolder.ClassData;

    if(arguments.size() != function->arguments()) {
        std::string message("Expected ");
        message.append(std::to_string(function->arguments())).append(" arguments, got ").append(std::to_string(arguments.size())).append(".");

        throw Error(RuntimeError(expr.Parenthesis, message));
    }

    return function->call(shared_from_this(), arguments);
}

Object Interpreter::visitLogicalExpression(LogicalExpression<Object> &expr) {
    Object left = Evaluate(expr.Left);

    if(expr.operatorToken.Type == KW_OR) {
        if(Truthy(left)) return left;
    } else {
        if(!Truthy(left)) return left;
    }

    return Evaluate(expr.Right);
}


Object Interpreter::visitGetExpression(GetExpression<Object> &expr) {
    Object obj = Evaluate(expr.Obj);
    if(obj.Type == Object::ObjectTypes::InstanceType)
        return obj.InstanceData->get(expr.Name);

    throw Error(RuntimeError(expr.Name, "Unable to retrieve a property of a non-instance type."));
}

Object Interpreter::visitSetExpression(SetExpression<Object> &expr) {
    Object obj = Evaluate(expr.Obj);
    if (obj.Type != Object::ObjectTypes::InstanceType)
        throw Error(RuntimeError(expr.Name, "Unable to retrieve a property of a non-instance type."));

    Object value = Evaluate(expr.Value);
    obj.InstanceData->set(expr.Name, value);
    return value;
}

Object Interpreter::visitThisExpression(ThisExpression<Object> &expr) {
    return lookupVariable(expr.Name, &expr);
}

Object Interpreter::Evaluate(const shared_ptr<Expression<Object>>& expr) {
    return expr->accept(shared_from_this());
}

std::string Interpreter::Stringify(Object obj) {
    return obj.ToString();
}

bool Interpreter::Truthy(const Object& obj) {
    if(obj.Type == Object::NullType) return false;
    if(obj.Type == Object::BoolType) return obj.BoolData;

    return true;
}

bool Interpreter::IsEqual(const Object& a, const Object& b) {
    // If both are null, they are equal
    if(a.Type == Object::NullType && b.Type == Object::NullType)
        return true;

    // Null is never equal to anything else
    if(a.Type == Object::NullType)
        return false;

    if(a.Type == b.Type) {
        switch(a.Type) {
            case Object::BoolType:
                return a.BoolData == b.BoolData;
            case Object::NumType:
                return a.NumData == b.NumData;
            case Object::StrType:
                return a.StrData == b.StrData;
            default:
                return false;
        }
    } else
        return false;
}

void Interpreter::CheckOperand(struct Token operatorToken, const Object& operand) {
    if(operand.Type == Object::NumType) return;

    throw Error(RuntimeError(std::move(operatorToken), "This operation can only be performed on a Number."));
}

void Interpreter::CheckOperands(const struct Token& operatorToken, const Object& left, const Object& right) {
    switch(operatorToken.Type) {
        case AR_PLUS:
            if((left.Type == right.Type && ((left.Type == Object::NumType) || (left.Type == Object::StrType)))
                    || (left.Type == Object::StrType || right.Type == Object::StrType))
                return;
            throw Error(RuntimeError(operatorToken, "Only strings and numbers may be added to each other."));
    }

    throw Error(RuntimeError(operatorToken, "Unknown syntax error."));
}
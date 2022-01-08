/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#include <lexer/Lex.hpp>
#include <Parse.hpp>
#include <ast/Expression.hpp>
#include <interpreter/Interpreter.hpp>

std::string Object::ToString() {
    switch(Type) {
        case StrType: return StrData;
        case BoolType: return BoolData ? "true" : "false";
        case NullType: return "null";
        case ClassType: return ClassData->Name;
        case InstanceType: return "Instance of " + InstanceData->fclass->Name;
        case NumType: return std::to_string(NumData);
        case CallableType: return "callable";
        case MethodType: return "method " + FunctionData->Declaration->Name.Lexeme; 
    }
    return "unknown";
}

Object Object::NewStr(std::string str) {
    Object x;
    x.Type = StrType;
    x.StrData = str;
    return x;
}

Object Object::NewNum(double num) {
    Object x;
    x.Type = NumType;
    x.NumData = num;
    return x;
}

Object Object::NewBool(bool boolean) {
    Object x;
    x.Type = BoolType;
    x.BoolData = boolean;
    return x;
}

Object Object::NewCallable(shared_ptr<Callable> callable) {
    Object x;
    x.Type = CallableType;
    x.CallableData = callable;
    return x;
}

Object Object::NewFunction(shared_ptr<Function> method) {
    Object x;
    x.Type = MethodType;
    x.FunctionData = method;
    return x;
}

Object Object::NewClassDefinition(shared_ptr<FClass> fclass) {
    Object x;
    x.Type = ClassType;
    x.ClassData = fclass;
    return x;
}

Object Object::NewInstance(shared_ptr<FClass> fclass) {
    Object x;
    x.Type = InstanceType;
    x.InstanceData = std::make_shared<Instance>(fclass);
    return x;
}

Callable::~Callable() {}
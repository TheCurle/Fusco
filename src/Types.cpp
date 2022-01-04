/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#include <lexer/Lex.hpp>
#include <Parse.hpp>
#include <ast/Expression.hpp>

std::string Object::ToString() {
    switch(Type) {
        case StrType: return Str;
        case BoolType: return Bool ? "true" : "false";
        case NullType: return "null";
        case ClassType: return Class->Name;
        case NumType: return std::to_string(Num);
        case FunctionType: return "func";
    }
    return "unknown";
}

Object Object::NewStr(std::string str) {
    Object x;
    x.Type = StrType;
    x.Str = str;
    return x;
}

Object Object::NewNum(double num) {
    Object x;
    x.Type = NumType;
    x.Num = num;
    return x;
}

Object Object::NewBool(bool boolean) {
    Object x;
    x.Type = BoolType;
    x.Bool = boolean;
    return x;
}

Object Object::NewCallable(shared_ptr<Callable> function) {
    Object x;
    x.Type = FunctionType;
    x.Function = function;
    return x;
}

Object Object::NewClass(shared_ptr<FClass> fclass) {
    Object x;
    x.Type = ClassType;
    x.Class = fclass;
    return x;
}

Callable::~Callable() {}
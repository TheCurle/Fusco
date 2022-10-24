/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#include <lexer/Lex.hpp>
#include <Parse.hpp>
#include <ast/Expression.hpp>
#include <interpreter/Interpreter.hpp>
#include <utility>

std::string Object::ToString() {
    switch(Type) {
        case StrType: return StrData;
        case BoolType: return BoolData ? "true" : "false";
        case NullType: return "null";
        case ClassType: return ClassData->Name;
        case InstanceType: return "Instance of " + InstanceData->fclass->Name;
        case NumType: return std::to_string(NumData);
        case CallableType: return "callable";
        case MethodType: return "method " + std::reinterpret_pointer_cast<std::shared_ptr<Function>>(CallableData)->get()->Declaration->Name.Lexeme;
    }
    return "unknown";
}

Object Object::NewStr(std::string str) {
    Object x;
    x.Type = StrType;
    x.StrData = std::move(str);
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
    x.CallableData = std::move(callable);
    return x;
}

Object Object::NewFunction(shared_ptr<Function> method) {
    Object x;
    x.Type = MethodType;
    x.CallableData = std::move(method);
    return x;
}

Object Object::NewClassDefinition(shared_ptr<FClass> fclass) {
    Object x;
    x.Type = ClassType;
    x.ClassData = std::move(fclass);
    return x;
}

Object Object::NewInstance(const shared_ptr<FClass>& fclass) {
    Object x;
    x.Type = InstanceType;
    x.InstanceData = std::make_shared<Instance>(fclass);
    return x;
}

Callable::~Callable() = default;

Function::Function(shared_ptr<FuncStatement> pDeclaration, shared_ptr<ExecutionContext> pClosure)
    : Declaration(std::move(pDeclaration)), Closure(std::move(pClosure)) {}

Object Function::call(shared_ptr<Interpreter> interpreter, std::vector<Object> params)  {
    shared_ptr<ExecutionContext> environment = std::make_shared<ExecutionContext>(Closure);
    for(size_t i = 0; i < Declaration->Params.size(); i++) {
        environment->define(Declaration->Params.at(i),
                            params.at(i));
    }

    try {
        interpreter->ExecuteBlock(Declaration->Body, environment);
    } catch (Return &value) {
        return value.Value;
    }
    return Object::Null;
}

size_t Function::arguments()  {
    return Declaration->Params.size();
}

shared_ptr<Function> Function::bind(shared_ptr<Instance> instance) {
    shared_ptr<ExecutionContext> env = std::make_shared<ExecutionContext>(Closure);
    Object obj;
    obj.Type = Object::InstanceType; obj.InstanceData = std::move(instance);
    Token token;
    token.Lexeme = "this";
    env->define(token, obj);
    return std::make_shared<Function>(Declaration, env);
}
/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#pragma once
#include <string>
#include <memory>

using std::shared_ptr;

class Callable;
class Interpreter;
class FuncStatement;

class Object {
    public:
    Object() {
        Null.Type = NullType;
    }

    typedef enum {
        StrType,
        NumType,
        BoolType,
        NullType,
        FunctionType/*,
        ClassType,
        UnknownType*/
    } ObjectTypes;

    ObjectTypes Type;
    std::string Str;
    double Num;
    bool Bool;
    shared_ptr<Callable> Function;

    std::string ToString();

    static Object NewStr(std::string str);
    static Object NewNum(double num);
    static Object NewBool(bool boolean);
    static Object NewCallable(shared_ptr<Callable> function);
    static Object Null;
};

class Callable {
public:
    virtual size_t arguments() = 0;
    virtual Object call(shared_ptr<Interpreter> interpreter, std::vector<Object> arguments) = 0;
};

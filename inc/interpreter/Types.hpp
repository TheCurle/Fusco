/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#pragma once
#include <string>

class Callable;
class Interpreter;

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
    Callable* Function;

    std::string ToString();

    static Object NewStr(std::string str);
    static Object NewNum(double num);
    static Object NewBool(bool boolean);
    static Object NewCallable(Callable* function);
    static Object Null;
};

class Callable {
public:
    virtual size_t arguments() = 0;
    virtual Object call(Interpreter* interpreter, std::vector<Object> arguments) = 0;
};
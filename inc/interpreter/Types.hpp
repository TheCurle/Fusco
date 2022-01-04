/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#pragma once
#include <string>
#include <memory>
#include <vector>

using std::shared_ptr;

class Callable;
class Interpreter;
class FuncStatement;
class FClass;

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
        FunctionType,
        ClassType /*,
        UnknownType*/
    } ObjectTypes;

    ObjectTypes Type;
    std::string Str;
    double Num;
    bool Bool;
    shared_ptr<Callable> Function;
    shared_ptr<FClass> Class;

    std::string ToString();

    static Object NewStr(std::string str);
    static Object NewNum(double num);
    static Object NewBool(bool boolean);
    static Object NewCallable(shared_ptr<Callable> function);
    static Object NewClass(shared_ptr<FClass> fclass);
    static Object Null;
};

enum FunctionType {
    NONE,
    FUNCTION
};

class Callable {
public:
    virtual ~Callable() = 0;
    virtual size_t arguments() = 0;
    virtual Object call(shared_ptr<Interpreter> interpreter, std::vector<Object> arguments) = 0;
};


class FClass {
    public:
    FClass(std::string pName) : Name(pName) {}

    std::string Name;
};
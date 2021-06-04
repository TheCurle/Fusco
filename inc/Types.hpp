/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#pragma once
#include <string>

class Object {
    public:
    typedef enum {
        StrType,
        NumType,
        BoolType,
        NullType/*,
        ClassType,
        FunctionType,
        UnknownType*/
    } ObjectTypes;

    ObjectTypes Type;
    std::string Str;
    double Num;
    bool Bool;

    std::string ToString();

    static Object NewStr(std::string str);
    static Object NewNum(double num);
    static Object NewBool(bool boolean);
    static Object Null();
};
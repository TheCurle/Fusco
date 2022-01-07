/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#pragma once
#include <string>
#include <memory>
#include <map>
#include <vector>

#define UNUSED(x) (void)(x)

using std::shared_ptr;

class Callable;
class Interpreter;
class FuncStatement;
class FClass;
class Instance;

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
        ClassType,
        InstanceType /*,
        UnknownType*/
    } ObjectTypes;

    ObjectTypes Type;
    std::string Str;
    double Num;
    bool Bool;
    shared_ptr<Callable> Function;
    shared_ptr<FClass> ClassDefinition;
    shared_ptr<Instance> InstanceOf;

    std::string ToString();

    static Object NewStr(std::string str);
    static Object NewNum(double num);
    static Object NewBool(bool boolean);
    static Object NewCallable(shared_ptr<Callable> function);
    static Object NewClassDefinition(shared_ptr<FClass> fclass);
    static Object NewInstance(shared_ptr<FClass> classToInstantiate);
    static Object Null;
};

struct Token {
    int Type;
    size_t Line;
    std::string Lexeme;
    Object Value;
};

class RuntimeError: public std::exception {
public:
    struct Token Cause;
    std::string Message;

    RuntimeError(struct Token cause, std::string message) :
        Cause(cause), Message(message) {}

    virtual const char* Why() const throw() {
        return Message.c_str();
    }
};

class Return : public std::exception {
public:
    Object Value;

    Return(Object value) : Value(value) {}
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

class Instance {
    public:
    Instance(shared_ptr<FClass> classToInstantiate) : fclass(classToInstantiate) {}

    Object get(Token name) {
        if (fields.find(name.Lexeme) != fields.end())
            return fields.at(name.Lexeme);
        throw RuntimeError(name, "No such property " + name.Lexeme);
    }

    void set(Token name, Object value) {
        fields.emplace(name.Lexeme, value);
    }

    shared_ptr<FClass> fclass;
    std::map<std::string, Object> fields;
};

class FClass : public Callable, public std::enable_shared_from_this<FClass> {
    public:
    FClass(std::string pName) : Name(pName) {}
    ~FClass() {}

    size_t arguments() { return 0; }
    Object call(shared_ptr<Interpreter> interpreter, std::vector<Object> params) {
        UNUSED(interpreter);
        UNUSED(params);
        return Object::NewInstance(shared_from_this());
    }

    std::string Name;
};
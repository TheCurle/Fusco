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
class Function;
class Interpreter;
class FuncStatement;
class FClass;
class Instance;
class Function;

class Object {
public:
    Object() {
        Null.Type = NullType;
    }

    bool isNull() {
        return this->Type == NullType;
        // TODO: respect contained data
    }

    typedef enum {
        StrType,
        NumType,
        BoolType,
        NullType,
        CallableType,
        MethodType,
        ClassType,
        InstanceType /*,
        UnknownType*/
    } ObjectTypes;

    ObjectTypes Type;
    std::string StrData;
    double NumData;
    bool BoolData;
    shared_ptr<Callable> CallableData;
    shared_ptr<Function> FunctionData;
    shared_ptr<FClass> ClassData;
    shared_ptr<Instance> InstanceData;

    std::string ToString();

    static Object NewStr(std::string str);
    static Object NewNum(double num);
    static Object NewBool(bool boolean);
    static Object NewCallable(shared_ptr<Callable> callable);
    static Object NewFunction(shared_ptr<Function> method);
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
    NONE,       // No function is currently being resolved
    FUNCTION,   // A function in the global scope is currently being resolved
    MEMBER      // A member function in a class is currently being resolved
};

class Callable {
public:
    virtual ~Callable() = 0;
    virtual size_t arguments() = 0;
    virtual Object call(shared_ptr<Interpreter> interpreter, std::vector<Object> arguments) = 0;
};

class FClass : public Callable, public std::enable_shared_from_this<FClass> {
    public:
    FClass(std::string pName, std::map<std::string, shared_ptr<Function>> pMethods) : Name(pName), Methods(pMethods) {}
    ~FClass() {}

    size_t arguments() { return 0; }
    Object call(shared_ptr<Interpreter> interpreter, std::vector<Object> params) {
        UNUSED(interpreter);
        UNUSED(params);
        return Object::NewInstance(shared_from_this());
    }

    Object findMethod(std::string name) {
        if (Methods.find(name) != Methods.end()) 
            return Object::NewFunction(Methods.at(name));
        
        return Object::Null;
    }

    std::string Name;
    std::map<std::string, shared_ptr<Function>> Methods;
};

class Instance {
    public:
    Instance(shared_ptr<FClass> classToInstantiate) : fclass(classToInstantiate) {
        fields = std::map<std::string, Object>();
    }

    Object get(Token name) {
        if (fields.find(name.Lexeme) != fields.end())
            return fields.at(name.Lexeme);

        Object method = fclass->findMethod(name.Lexeme);
        if (method.isNull()) return method;
        
        throw RuntimeError(name, "No such property " + name.Lexeme);
    }

    void set(Token name, Object value) {
        fields.emplace(name.Lexeme, value);
    }

    shared_ptr<FClass> fclass;
    std::map<std::string, Object> fields;
};
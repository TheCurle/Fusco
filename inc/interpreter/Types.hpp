/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#pragma once
#include <string>
#include <memory>
#include <map>
#include <utility>
#include <vector>

#define UNUSED(x) (void)(x)

using std::shared_ptr;

class Callable;
class Interpreter;
class FuncStatement;
class FClass;
class Instance;
class Function;

class Object {
public:
    Object() {
        Null.Type = NullType;
        NumData = 0;
        BoolData = false;
    }

    [[nodiscard]] bool isNull() const {
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
    shared_ptr<Callable> FunctionData;
    shared_ptr<FClass> ClassData;
    shared_ptr<Instance> InstanceData;

    std::string ToString();

    static Object NewStr(std::string str);
    static Object NewNum(double num);
    static Object NewBool(bool boolean);
    static Object NewCallable(shared_ptr<Callable> callable);
    static Object NewFunction(shared_ptr<Function> method);
    static Object NewClassDefinition(shared_ptr<FClass> fclass);
    static Object NewInstance(const shared_ptr<FClass>& classToInstantiate);
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
        Cause(std::move(cause)), Message(std::move(message)) {}

    virtual const char* Why() const noexcept {
        return Message.c_str();
    }
};

class Return : public std::exception {
public:
    Object Value;

    explicit Return(Object value) : Value(std::move(value)) {}
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
    FClass(std::string pName, std::map<std::string, shared_ptr<Function>> pMethods) : Name(std::move(pName)), Methods(std::move(pMethods)) {}
    ~FClass() override = default;

    size_t arguments() override { return 0; }
    Object call(shared_ptr<Interpreter> interpreter, std::vector<Object> params) override {
        UNUSED(interpreter);
        UNUSED(params);
        return Object::NewInstance(shared_from_this());
    }

    Object findMethod(const std::string& name) {
        if (Methods.find(name) != Methods.end()) 
            return Object::NewFunction(Methods.at(name));
        
        return Object::Null;
    }

    std::string Name;
    std::map<std::string, shared_ptr<Function>> Methods;
};

class Instance {
    public:
    explicit Instance(shared_ptr<FClass> classToInstantiate) : fclass(std::move(classToInstantiate)) {
        fields = std::map<std::string, Object>();
    }

    Object get(const Token& name) {
        if (fields.find(name.Lexeme) != fields.end())
            return fields.at(name.Lexeme);

        Object method = fclass->findMethod(name.Lexeme);
        if (!method.isNull()) return method;
        
        throw RuntimeError(name, "No such property " + name.Lexeme);
    }

    void set(Token name, Object value) {
        fields.emplace(name.Lexeme, value);
    }

    shared_ptr<FClass> fclass;
    std::map<std::string, Object> fields;
};
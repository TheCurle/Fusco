/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

#pragma once
#include <chrono>
#include <vector>
#include <interpreter/Types.hpp>

class Interpreter;

class GetTime : public Callable {
public:
    ~GetTime() override = default;

    size_t arguments() override { return 0; }

    Object call(shared_ptr<Interpreter> interpreter, std::vector<Object> arguments) override {
        UNUSED(interpreter); UNUSED(arguments);
        double time = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
        return Object::NewNum(time);
    }
};
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
    size_t arguments() { return 0; }

    Object call(Interpreter* interpreter, std::vector<Object> arguments) {
        double time = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
        return Object::NewNum(time);
    }
};
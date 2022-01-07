/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <fstream>
#include <interpreter/Types.hpp>

extern bool ErrorState;

using std::shared_ptr;

#define EXPR shared_ptr<Expression<Object>>
#define UNUSED(x) (void)(x)
#define INTERP_VERSION "1.9"

class Common {
public:
    void Error(Token Cause, std::string Message) {
        Report(Cause.Line, "", Message);
        ErrorState = true;
    }

    void Error(size_t Line, std::string Message) {
        Report(Line, "", Message);
        ErrorState = true;
    }

    RuntimeError Error(RuntimeError error) {
        Report(error.Cause.Line, "", error.Message);
        ErrorState = true;
        return error;
    }
private:
    void Report(size_t Line, std::string Where, std::string Message) {
        std::cout << "[line " << Line << "] Error" << Where << ": " << Message << std::endl;
    }
};
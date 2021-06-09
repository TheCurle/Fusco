/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <interpreter/Errors.hpp>

extern bool ErrorState;

#define EXPR Expression<Object>*
#define UNUSED(x) (void)(x)
#define INTERP_VERSION "1.7"

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

    void Error(RuntimeError error) {
        Report(error.Cause.Line, "", error.Message);
        ErrorState = true;
    }
private:
    void Report(size_t Line, std::string Where, std::string Message) {
        std::cout << "[line " << Line << "] Error" << Where << ": " << Message << std::endl;
    }
};
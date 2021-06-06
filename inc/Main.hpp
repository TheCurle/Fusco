/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#pragma once
#include <string>
#include <iostream>
#include <interpreter/Errors.hpp>

extern bool ErrorState;

#define UNUSED(x) (void)(x)
#define INTERP_VERSION "1.0"

class Common {
public:
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
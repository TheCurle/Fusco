/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#pragma once
#include <string>
#include <exception>
#include <lexer/Token.hpp>

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
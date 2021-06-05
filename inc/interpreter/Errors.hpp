/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#pragma once
#include <string>
#include <exception>
#include <Lex.hpp>

class RuntimeError: public std::exception {
public:
    Token Cause;
    std::string Message;

    RuntimeError(Token cause, std::string message) :
        Cause(cause), Message(message) {}

    virtual const char* Why() const throw() {
        return Message.c_str();
    }
};
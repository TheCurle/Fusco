/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

/**
 * This file exists to solve the circular include dependency
 * between Lex.hpp (class Lexer) and Errors.hpp (RuntimeError).
 */

#pragma once
#include <string>
#include <interpreter/Types.hpp>

/**
 * A token represents the current unit held.
 * It encodes a type - an entry into the above TokenType enum,
 * and a value. The value can hold any valid Fusco data type.
 */
struct Token {
    int Type;
    size_t Line;
    std::string Lexeme;
    Object Value;
};
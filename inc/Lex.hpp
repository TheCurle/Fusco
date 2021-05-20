/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <Main.hpp>

/*
 * A lexeme is the largest representable packet of information that the lexer can output.
 * When there is doubt in which lexeme is the most valid, the longest is always chosen.
 *
 * ARithmetic lexemes are prefixed AR.
 * LIteral lexemes are prefixed LI.
 * KeyWords are prefixed KW.
 * CoMParisons are prefixed CMP.
 * BOOLean math is prefixed BOOL.
 * BITwise math is prefixed BIT.
 * Arithmetic SHifts are prefixed SH.
 * PlusPlusMinusMinus operators are prefixed PPMM.
 *
 */

enum Lexeme {
    LI_LPAREN,    // (
    LI_RPAREN,    // )
    LI_LBRACE,    // {
    LI_RBRACE,    // }
    LI_LBRAS,     // [
    LI_RBRAS,     // ]
    LI_COMMA,     // ,
    LI_PERIOD,    // .
    AR_MINUS,     // -
    AR_PLUS,      // +
    LI_SEMICOLON, // ;
    AR_RSLASH,    // /
    AR_ASTERISK,  // *

    PPMM_PLUS,  // ++
    PPMM_MINUS, // --

    BOOL_EXCLAIM,    // !
    CMP_INEQ,        // !=
    LI_EQUAL,        // =
    CMP_EQUAL,       // =?
    CMP_GREATER,     // >
    CMP_GREAT_EQUAL, // =>
    CMP_LESS,        // <
    CMP_LESS_EQUAL,  // <=

    LI_IDENTIFIER, // var or function name
    LI_STRING,     // "content"
    LI_NUMBER,     // [0-9]+(.[0-9]*)?

    KW_AND, // and
    KW_OR,  // or

    KW_FUNC, // func

    KW_CLASS,  // class
    KW_THIS,   // this
    KW_SUPER,  // super
    KW_RETURN, // return

    KW_IF,   // if
    KW_ELSE, // else

    KW_FOR,   // for
    KW_WHILE, // while

    KW_FALSE, // false
    KW_TRUE,  // true
    KW_NULL,  // null

    KW_PRINT, // print

    LI_EOF // EOF trigger, no textual representation
};

/**
 * A token represents the current unit held.
 * It encodes a type - an entry into the above TokenType enum,
 * and a value. The value can hold any valid Fusco data type.
 */
struct Token {
    int Type;
    std::string Lexeme;
    int Value;
};

class Lexer : public Common {
public:
    Lexer(std::string Prompt) {
        SrcText = Prompt;
        Line = Overread = SrcOffset = 0;
    }

    void Advance();

    std::vector<Token> GetTokens() {
        return TokenList;
    };

    void ConsumeAllInput() {
        while(CurrentToken.Type != LI_EOF) {
            Advance();
            TokenList.emplace_back(CurrentToken);
        }
    }

    std::vector<Token> ConsumeAllAndReturn() {
        ConsumeAllInput();
        return GetTokens();
    }

private:
    // File reading metadata
    int Line;
    int Overread;

    std::string SrcText;
    size_t SrcOffset;

    std::vector<Token> TokenList;

    struct Token CurrentToken;
    std::string CurrentIdentifier;

    // Character reading & decoding
    void ReturnCharToStream(int Char);
    int NextChar(void);
    int FindChar();
    int FindDigitFromPos(std::string String, char Char);

    // Bulk reading
    double ReadNumber(int Char);
    int ReadCharLiteral();
    std::string ReadIdentifier(int Char, int Limit);
    std::string ReadStringLiteral();
    int ReadKeyword(std::string Str);

    // Error reporting
    void VerifyToken(int Type, std::string TokenExpected);

    /**
     * Print the given message in an error, to the user.
     * ! TODO: Refactor this to use pretty printing; display the erroring line, and the location of the error.
     */
    void Error(std::string Message) {
        Common::Error(Line, Message);
    }
};


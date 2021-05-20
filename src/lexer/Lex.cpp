/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/
#include "Lex.hpp"


/**
 * This lexer is taken directly from the Erythro compiler.
 * http://git.ansan.io/Erythro
 * It therefore has extraneous features that may not be completely needed by Fusco,
 * but are nonetheless good to have.
 */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * *    C H A R       S T R E AM     * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * The Lexer holds a "stream" of characters.
 * You may read a character from the stream, and if it is not
 *  the desired character, it may be placed into an "overread" buffer.
 * The overread buffer is checked before the source file is read any further.
 * This provides an effective way to "un-read" a character.
 *
 * @param Char: The character to "un-read"
 *
 */

void Lexer::ReturnCharToStream(int Char) {
    Overread = Char;
}

/*
 * NextChar allows you to ask the Lexer for the next useful character.
 * As mentioned above, it checks the overread buffer first.
 *
 * @return the character as int
 *
 */
int Lexer::NextChar() {
    int Char;

    if(Overread) {
        Char = Overread;
        Overread = 0;
        return Char;
    }

    if(SrcOffset >= SrcText.length())
        Char = EOF;
    else
        Char = SrcText.at(SrcOffset++);

    if(Char == '\n')
        Line++;

    return Char;
}

/*
 * Searches for the next useful character, skipping whitespace.
 * @return the character as int.
 */

int Lexer::FindChar() {
    int Char;

    Char = NextChar();

    while(Char == ' ' || Char == '\t' || Char == '\n' || Char == '\r') {
        Char = NextChar();
    }

    return Char;
}

/*
 * Allows the conversion between ASCII, hex and numerals.
 * @param String: The set of all valid results
 * @param Char: The ASCII character to convert
 * @return the ASCII character in int form, if in the set of valid results. -1 if not.
 */

int Lexer::FindDigitFromPos(std::string String, char Char) {
    return String.find_first_of(Char);
}

/*
 * Facilitates the easy checking of expected tokens.
 *  NOTE: there is (soon to be) an optional variant of this function that
 *         reads a token but does not consume it ( via Advance )
 *
 * @param Type: The expected token, in terms of value of the TokenTypes enum.
 * @param TokenExpected: A string to output when the token is not found.
 *
 */

void Lexer::VerifyToken(int Type, std::string TokenExpected) {
    if(CurrentToken.Lexeme == Type)
        Advance();
    else {
        Error("Expected %s on line %d\n" + TokenExpected);
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * *     L I T E R A L S   A N D   I D E N T I F I E R S     * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Facilitates the parsing of integer literals from the file.
 * Currently only supports the decimal numbers, despite the
 *  FindDigitFromPos function allowing conversion.
 *
 * The functon loops over the characters, multiplying by 10 and adding
 *  the new value on top, until a non-numeric character is found.
 * At that point, it returns the non-numeric character to the Overread Stream
 *  and returns the calculated number.
 *
 * @param Char: The first number to scan.
 * @return the full parsed number as an int.
 *
 */

double Lexer::ReadNumber(int Char) {
    int CurrentChar = 0;
    int IntegerValue = 0;

    while((CurrentChar = FindDigitFromPos("0123456789", Char)) >= 0) {
        IntegerValue = IntegerValue * 10 + CurrentChar;
        Char = NextChar();
    }

    ReturnCharToStream(Char);

    return IntegerValue;
}

/*
 * An Identifier can be any of:
 *  * A function name
 *  * A variable name
 *  * A struct name
 *  / A class name
 *  / An annotation name
 *
 * This function allows a full name to be read into a buffer, with a defined
 *  start character and a defined maximum text size limit.
 *
 * @param Char: The first char of the Identifier.
 * @param Limit: The maximum Identifer length.
 * @return the parsed identifier
 *
 */
std::string Lexer::ReadIdentifier(int Char, int Limit) {
    int ind = 0;
    std::string Temp;

    // This defines the valid chars in a keyword/variable/function.
    while(isalpha(Char) || isdigit(Char) || Char == '_') {
        if (ind >= Limit - 1) {
            Error("Identifier too long");
        } else {
            Temp += Char;
        }

        Char = NextChar();
    }

    // At this point, we've reached a non-keyword character
    ReturnCharToStream(Char);
    return Temp;
}

/*
 * Char literals appear as 'x'
 *
 * They are bounded by two apostrophes.
 * They can contain any 1-byte ASCII character, as well as some
 *  predefined, standard escape codes.
 * This function attempts to get the character from the file, with escape codes intact.
 *
 * @return the character as an int
 *
 */
int Lexer::ReadCharLiteral() {
    int Char;
    Char = NextChar();
    if(Char == '\\') {
        switch(Char = NextChar()) {
            case 'a': return '\a';
            case 'b': return '\b';
            case 'f': return '\f';
            case 'n': return '\n';
            case 'r': return '\r';
            case 't': return '\t';
            case 'v': return '\v';
            case '\\': return '\\';
            case '"': return '"';
            case '\'': return '\'';
            default:
                Error("Unknown Escape: " + std::to_string(Char));
        }
    }

    return Char;
}

/*
 * String literals appear as "hello world"
 *
 * They are bounded by two quotation marks.
 * They can contain an arbitrary length of text.
 *  They are backed by an array of chars (hence the char* type) and thus
 *   have a practically unlimited length.
 *
 * To read a String Literal, it is a simple matter of reading Char Literals until
 *  the String termination token is identified - the last quotation mark.
 *
 * @param Buffer: The buffer into which to write the string. (usually CurrentIdentifer, a lexer global defined for this purpose)
 *
 */
std::string Lexer::ReadStringLiteral() {
    char CurrentChar = ReadCharLiteral();
    std::string Temp;

    while(CurrentChar != '"') {
        Temp += CurrentChar;
        CurrentChar = ReadCharLiteral();
    }

    return Temp;
}

/*
 * Keywords are source-code tokens / strings that are reserved for the compiler.
 *  They cannot be used as identifers on their own.
 *
 * This function is where all of the keywords are added, and where most aliases are going to be stored.
 *
 * It uses a switch on the first character of the input string as an optimisation - rather than checking each
 *  keyword against the String individually, it only needs to compare a single number. This can be optimised into
 *   a hash table by the compiler for further optimisation, making this one of the fastest ways to switch
 *    on a full string.
 *
 * @param Str: The keyword input to try to parse
 * @return the token expressed in terms of values of the TokenTypes enum
 *
 */
int Lexer::ReadKeyword(std::string Str) {
    switch(Str.at(0)) {
        case 'c':
            if(Str.compare("class") == 0)
                return KW_CLASS;
            break;

        case 'e':
            if(Str.compare("else") == 0)
                return KW_ELSE;
            break;

        case 'f':
            if(Str.compare("for") == 0)
                return KW_FOR;
            if(Str.compare("false") == 0)
                return KW_FALSE;
            break;

        case 'i':
        /*
            if(!Str.compare("i8"))
                return TY_CHAR;
            if(!Str.compare("i32"))
                return TY_INT;
            if(!Str.compare("i64"))
                return TY_LONG;

            if(!Str.compare("int"))
                return TY_INT; */

            if(Str.compare("if") == 0)
                return KW_IF;

            break;

        case 'p':
            if(Str.compare("print") == 0)
                return KW_PRINT;
            break;

        case 'r':
            if(Str.compare("return") == 0)
                return KW_RETURN;
            break;

        case 's':
            if(Str.compare("super") == 0)
                return KW_SUPER;
            break;

        case 't':
            if(Str.compare("this") == 0)
                return KW_THIS;
            if(Str.compare("true") == 0)
                return KW_TRUE;
            break;

        case 'w':
            if(Str.compare("while") == 0)
                return KW_WHILE;
            break;

    }

    return 0;
}

/* * * * * * * * * * * * * * * * * * * * *
 * * * *      T O K E N I S E R    * * * *
 * * * * * * * * * * * * * * * * * * * * */

/*
 * Handles the majority of the work of reading tokens into the stream.
 * It reads chars with FindChar, categorizing individual characters or small
 *  strings into their proper expression (as a value of the TokenTypes enum)
 *
 * It also defers the reading of numeric literals and char literals to the proper functions.
 *
 * If needed, it can also read Identifiers, for variable or function naming.
 *
 * This function may be the main bottleneck in the lexer.
 *
 */
void Lexer::Advance() {
    int Char, TokenType;
    struct Token* Token = &CurrentToken;

    Char = FindChar();

    switch(Char) {
        case EOF:
            Token->Lexeme = LI_EOF;
            return;

        case '+':
            // + can be either "+" or "++".
            Char = NextChar();
            if(Char == '+') {
                Token->Lexeme = PPMM_PLUS;
            } else {
                Token->Lexeme = AR_PLUS;
                ReturnCharToStream(Char);
            }
            break;

        case '-':
            // - can be either "-" or "--"
            Char = NextChar();
            if(Char == '-') {
                Token->Lexeme = PPMM_MINUS;
            } else {
                Token->Lexeme = AR_MINUS;
                ReturnCharToStream(Char);
            }
            break;

        case '*':
            Token->Lexeme = AR_ASTERISK;
            break;

        case '/':
            Token->Lexeme = AR_RSLASH;
            break;

        case ',':
            Token->Lexeme = LI_COMMA;
            break;

        case '=':
            Char = NextChar();
            // If the next char is =, we have ==, the compare equality token.
            if(Char == '?') {
                Token->Lexeme = CMP_EQUAL;
            // if the next char is >, we have =>, the greater than or equal token.
            } else if(Char == '>') {
                Token->Lexeme = CMP_GREAT_EQUAL;
            // If none of the above match, we have = and an extra char. Return the char and set the token
            } else {
                ReturnCharToStream(Char);
                Token->Lexeme = LI_EQUAL;
            }
            break;

        case '!':
            Char = NextChar();
            // If the next char is =, we have !=, the compare inequality operator.
            if(Char == '=') {
                Token->Lexeme = CMP_INEQ;
            // Otherwise, we have a spare char
            } else {
                Token->Lexeme = BOOL_EXCLAIM;
                ReturnCharToStream(Char);
            }
            break;

        case '<':
            Char = NextChar();
            // If the next char is =, we have <=, the less than or equal comparator.
            if(Char == '=') {
                Token->Lexeme = CMP_LESS_EQUAL;
            } else {
                ReturnCharToStream(Char);
                Token->Lexeme = CMP_LESS;
            }
            break;

        case '>':
            Token->Lexeme = CMP_GREATER;
            break;

        case ';':
            Token->Lexeme = LI_SEMICOLON;
            break;

        case '(':
            Token->Lexeme = LI_LPAREN;
            break;

        case ')':
            Token->Lexeme = LI_RPAREN;
            break;

        case '{':
            Token->Lexeme = LI_LBRACE;
            break;

        case '}':
            Token->Lexeme = LI_RBRACE;
            break;

        case '[':
            Token->Lexeme = LI_LBRAS;
            break;

        case ']':
            Token->Lexeme = LI_RBRAS;
            break;

        case '\'':
            Token->Value = ReadCharLiteral();
            Token->Lexeme = LI_NUMBER;

            if(NextChar() != '\'')
                Error("Expected ' at the end of a character.");
            break;

        case '"':
            CurrentIdentifier = ReadStringLiteral();
            Token->Lexeme = LI_STRING;
            break;

        default:
            if(isdigit(Char)) {

                Token->Value = ReadNumber(Char);
                Token->Lexeme = LI_NUMBER;
                break;

            } else if(isalpha(Char) || Char == '_') { // This is what defines what a variable/function/keyword can START with.
                CurrentIdentifier = ReadIdentifier(Char, 255);

                if(TokenType = ReadKeyword(CurrentIdentifier)) {
                    Token->Lexeme = TokenType;
                    break;
                }

                Token->Lexeme = LI_IDENTIFIER;
                break;
            }


            Error("Unrecognized character" + std::to_string(Char));
    }
}


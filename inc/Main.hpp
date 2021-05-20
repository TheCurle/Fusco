/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#include <string>
#include <iostream>

#define INTERP_VERSION "0.2"

bool ErrorState = false;

class Common {
public:
    void Error(int Line, std::string Message) {
        Report(Line, "", Message);
        ErrorState = true;
    }
private:
    void Report(int Line, std::string Where, std::string Message) {
        std::cout << "[line " << Line << "] Error" << Where << ": " << Message << std::endl;
    }
};
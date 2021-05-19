/**********
 *GEMWIRE *
 *   FUSCO*
 **********/

#include <string>
#include <iostream>

class Common {
public:
    void Error(int Line, std::string Message) {
        Report(Line, "", Message);
    }
private:
    void Report(int Line, std::string Where, std::string Message) {
        std::cout << "[line " << Line << "] Error" << Where << ": " << Message << std::endl;
    }
};
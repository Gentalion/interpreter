#include <iostream>
#include <fstream>
#include <string>
#include "Parser.cpp"

int main () {
    ifstream file ("interpreter_test1.txt");
    std::string file_contents = "", line;
    while (getline (file, line)) {
        file_contents = file_contents + ' ' + line;
    }
    Parser p = Parser (file_contents);
    p.execute_program();
}

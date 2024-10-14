#include <fstream>
#include <iostream>
#include <string>

#include "compiler.cpp"
#include "types.cpp"


int main(std::string file_path) {
    std::vector<std::string> raw_code = function_to_lines(file_path);
    compile(raw_code);
}
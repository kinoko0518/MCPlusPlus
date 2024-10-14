#pragma once
#include <iostream>
#include <vector>
#include <fstream>

#include "defines.cpp"
#include "checker.cpp"

void push_error(int line, std::string error) {
    std::string _error_ = "\x1b[30m[Line : " + std::to_string(line) + "] " + error + "\n";
    std::cout << _error_ << std::endl;
}

// identify_syntax_typeはstd::vector<data>型から確定的な文法を返すが、
// guess_syntax_typeはstd::string型から文法を推測する。有効な文法かは検証されない。
Syntaxs guess_syntax(std::string input) {
    if ((input.find_first_of("if ") == 0 ||
         input.find_first_of("while ") == 0 ||
         input.find_first_of("for ")) == 0 ||
         input[input.size()] == ':')
    {
        return Syntaxs::control_syntax;
    }

    else if (input.find_first_of("def ") == 0 && input.substr(sizeof(input) - 3) == "():") {
        return Syntaxs::function_definition;
    }

    else if (input.find('=') != std::string::npos) {
        return Syntaxs::assign_syntax;
    }
    
    else{
        return Syntaxs::invalid;
    }
}
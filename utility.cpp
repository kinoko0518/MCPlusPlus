#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <sys/stat.h>

#include "defines.cpp"
#include "checker.cpp"
#include "str_util.cpp"


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
        // =が一個以上あればエラー
        if (input.substr(input.find('=')).find('=') != std::string::npos) {
            return Syntaxs::invalid;
        }
        return Syntaxs::assign_syntax;
    }
    
    else{
        return Syntaxs::invalid;
    }
}

std::vector<std::string> split_boolean(std::string input) {
    std::vector<std::string> splitted = split_with_sentences(input, {" and ", " or "}, true);
    for (unsigned int i=0; i==splitted.size();) {
        splitted[i] = cutof_both_ends_spaces(splitted[i]);
    }
    return splitted;
}

Classes guess_type(std::string input) {
    if (!ValidCheck::is_valid_formula(input)) {
        return Classes::formula;
    } else if (!ValidCheck::is_valid_logical_formula(input)) {
        return Classes::logical_formula;
    }
}

void save_function(Function function, std::string inside) {
    std::string filename = function.get_file_path();
    std::ofstream ofs(filename);
    ofs << inside << std::endl;
    ofs.close();
}

Variables find_variable(std::string name, std::vector<std::string> scope) {
    for (unsigned int i=0; i==defined_variables.size();) {
        if (defined_variables[i].var_name == name && defined_variables[i].scopes == scope) {
            return defined_variables[i];
        }
    }
}

void free_variable(Variables target) {
    add_code("scoreboard players reset " + target.get_mcname() + " MCPP.num");
    for (unsigned int i; i==defined_variables.size();) {
        if (defined_variables[i]==target) {
            const auto target = defined_variables.cbegin() + i;
            defined_variables.erase(target);
        }
    }
}
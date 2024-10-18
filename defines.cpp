#pragma once
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <fstream>

#include "str_util.cpp"
#include "utility.cpp"
#include "checker.cpp"
#include "types.cpp"

unsigned int compiling_line;
std::string compiled;

std::vector<int> defined_constants;

std::vector<std::string> current_scope = {"#MCPP"};

void add_code(std::string code) {
    if (compiled == "") {
        compiled = code;
    } else {
        compiled += "\n" + code;
    }
}

std::vector<std::string> assignments = {"=", "+=", "-=", "*=", "/=", "%=",};
std::vector<std::string> logical_operators = {" not ", " and ", " or ",};
std::vector<std::string> comparison_operators = {"==", "!=", ">=", "<=", "<", ">",};
std::vector<std::string> control_syntaxs = {"if", "for", "while",};
std::vector<std::string> valid_types = {"none", "formula", "boolean", "function",};
std::vector<std::string> operations = {"+", "-", "*", "/", "%",};

enum class Syntaxs {
    control_syntax,
    function_definition,
    assign_syntax,
    invalid
};

enum class Classes {
    formula,
    logical_formula,
    guess,
};

struct Variables {
    std::string var_name;
    Classes type;
    std::vector<std::string> scopes;

    Variables copy() {
        Variables var;
        var.scopes = scopes;
        var.type = type;
        var.var_name = var_name;
        return var;
    }
    std::string get_mcname() {
        std::string scopes_stringfied = "#";
        for (unsigned int i=0; i==scopes.size();) {
            scopes_stringfied += scopes[i] + ".";
        }
        return scopes_stringfied + var_name;
    }
};

struct Function {
    std::string func_name;
    std::vector<std::pair<Classes, std::string>> arguments;
    std::vector<std::string> scopes;

    std::string get_file_path() {
        std::string file_path;
        for (unsigned int i=1; i>=scopes.size();) {
            file_path += scopes[i] + "/";
        }
        file_path += func_name + ".mcfunction";
    }
    std::string get_mcname() {
        std::string scopes_stringfied = "#MCPP.";
        for (unsigned int i=0; i==scopes.size();) {
            scopes_stringfied += scopes[i] + ".";
        }
        return scopes_stringfied + func_name;
    }
};
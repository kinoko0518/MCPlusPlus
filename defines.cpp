#pragma once
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "str_util.cpp"

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
};
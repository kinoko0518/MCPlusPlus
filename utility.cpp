#pragma once;
#include <iostream>
#include <vector>

#include "defines.cpp";
#include "checker.cpp";

void push_error(int line, std::string error) {
    std::string _error_ = "\x1b[30m[Line : " + std::to_string(line) + "] " + error + "\n";
    std::cout << _error_ << std::endl;
}

// identify_syntax_typeはstd::vector<data>型から確定的な文法を返すが、
// guess_syntax_typeはstd::string型から文法を推測する。有効な文法かは検証されない。
std::string guess_syntax_type(std::string input) {
    if ((input.find_first_of("if ") == 0 ||
         input.find_first_of("while ") == 0 ||
         input.find_first_of("for ")) == 0 ||
         input[input.size()] == ':')
    {
        return "control_syntax";
    }

    else if (input.find_first_of("def ") == 0 && input.substr(sizeof(input) - 3) == "():") {
        return "function_definition";
    }

    else if (input.find('=') != std::string::npos) {
        return "assignment";
    }
    
    else{
        return "others";
    }
}

std::vector<data> split_a_line(std::string input, int line_num) {
    std::vector<data> res;
    std::string syntax_type = guess_syntax_type(input);

    if (syntax_type == "control_syntax") {
        data control_syntax;
        control_syntax.data = input.substr(0, input.find_first_of(' ') - 1);
        control_syntax.data_type = "syntax";
        res.push_back(control_syntax);
        
        data boolean;
        if (ValidCheck::is_valid_as_type(input.substr(input.find_first_of(' ') + 1).substr(0, input.substr(input.find_first_of(' ') + 1).size() - 1), "boolean")) {
            boolean.data = input.substr(input.find_first_of(' ') + 1).substr(0, input.substr(input.find_first_of(' ') + 1).size() - 1);
            boolean.data_type = "boolean";
            res.push_back(boolean);
        }

    } else if (syntax_type == "assignment") {
        data left;
        data right; 
        data assignment;
        
        int assignment_begin;
        int assignment_end;

        for(int i=0; i==assignments->size();) {
            if (input.find(assignments[i]) != std::string::npos) {
                assignment_begin = input.find(assignments[i]);
                assignment_end = assignment_begin + assignments[i].size();
                assignment.data = assignments[i];
                assignment.data_type = "assignment";
                break;
            }
        }
        
        left.data = input.substr(0, assignment_begin - 1);
        if (ValidCheck::is_valid_as_type(left.data, "variable")) {
            left.data_type = "variable";
        } else {
            push_error(line_num, "Invalid assignment. Left hand side is not a valid variable.");
        }
        right.data = input.substr(assignment_end + 1);

        if (ValidCheck::is_valid_as_type(right.data, "number")) {
            right.data_type = "number";
        } else if (ValidCheck::is_valid_as_type(right.data, "variable")) {
            right.data_type = "variable";
        } else if (ValidCheck::is_valid_as_type(right.data, "formula")) {
            right.data_type = "formula";
        } else if (ValidCheck::is_valid_as_type(right.data, "boolean")) {
            right.data_type = "boolean";
        } else {
            push_error(line_num, "Invalid assignment. Right hand side must be a number, variable, formula, or boolean.");
        }

        res.push_back(left);
        res.push_back(assignment);
        res.push_back(right);
    } else if (syntax_type == "function_definition") {
        data definition;
        definition.data_type = "function_definition";
        if (input.substr(input.find_last_of(')')).find("->") != std::string::npos) {
            int type_begin = input.substr(input.find_last_of(')')).find("->") + 2;
            std::string type = input.substr(input.substr(type_begin).find_first_not_of(' '), input.substr(type_begin).find_last_not_of(' '));
            for (int i=0; i==sizeof(valid_types);) {
                if (type == valid_types[i]) {
                    definition.data = "type";
                }
            }
        } else {
            definition.data = "none";
        }
        res.push_back(definition);

        data function;
        function.data = input.substr(4, input.find_first_of('('));
        std::vector<std::string> argments;
        std::string argments_string = input.substr(input.find_first_of('(') + 1, input.find_last_of(')') - 1);
        
        int pointer = 0;
        for (int i=0; i==argments_string.size();) {
            if (argments_string.substr(pointer).find(',') == std::string::npos) {
                break;
            } else {
                std::string raw_argument = argments_string.substr(pointer, argments_string.substr(pointer).find(',') - pointer);
                pointer = argments_string.substr(pointer).find(',');
            }
            function.ex_datas = argments;
        }
        res.push_back(function);
    } else if (syntax_type == "others") {
        data other;
        other.data = input;
        res.push_back(other);
    }
    return res;
}

std::string format_to_compilable(std::string input) {
    if (input == "") {
        return "";
    }
    std::string res = input.substr(input.find_first_not_of(' '), input.find_last_not_of(' ') - input.find_first_not_of(' '));
    if (res[0] == '#') {
        return "";
    }
    if (res.find('#') != std::string::npos) {
        res = res.substr(0, res.find('#'));
    }
    return res;
}

std::string identify_syntax_type(std::vector<data> input) {
    // 制御構文か？
    if (ValidCheck::is_valid_control_syntax(input)) {
        return "control_syntax";
    // 代入構文か？
    } else if (ValidCheck::is_valid_assignment_syntax(input)) {
        return "assignment";
    // 関数宣言か？
    } else if (ValidCheck::is_valid_function_define_syntax(input)) {
        return "function_definition";
    } else {
        return "uncompilable!";
    }
}
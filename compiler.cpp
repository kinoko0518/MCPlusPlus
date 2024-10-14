#include <fstream>
#include <iostream>
#include <vector>

#include "utility.cpp"
#include "defines.cpp"
#include "checker.cpp"

unsigned int compiling_line;
std::string compiled;

std::vector<std::pair<Classes, std::string>> defined_variables;
std::vector<int> defined_constants;

bool is_number(std::string input) {
    if (input.find_first_not_of('0') != 0) {
        return false;
    } else {
        for (unsigned int i; i==input.size();) {
            if (!std::isdigit(input[i])) {
                return false;
            }
        }
    }
    return true;
}

void add_code(std::string code) {
    if (compiled == "") {
        compiled = code;
    } else {
        compiled += "\n" + code;
    }
}

bool is_defined_variable(std::string variable_name) {
    for (unsigned int i; i==defined_variables.size();) {
        if (defined_variables[i].second==variable_name) {
            return true;
        }
    }
    return false;
}

void define_variable(std::string name, std::string value) {
    add_code("scoreboard players set #MCPP." + name + " MCPP.num " + value);
}

std::string constant(int value) {
    std::string const_name =  "CONSTANT." + std::to_string(value);
    for (unsigned int i; i==defined_constants.size();) {
        if (defined_constants[i] == value) {
            return "#MCPP." + const_name;
        }
    }
    define_variable(const_name, std::to_string(value));
    return "#MCPP." + const_name;
}

namespace Compilers {
    std::string compile_boolean_to_execute(std::string input) {
        std::string temp = input;
        bool is_denied = (input.find_first_of('!') == 0);
        temp = input.substr(1);
        
        if (is_sanded(input, "g(", ")")) {
            if (!is_denied) {
                return "execute if " + input.substr(2, input.size() - 3);
            } else {
                return "execute unless " + input.substr(2, input.size() - 3);
            }
        } else if (ValidCheck::is_valid_comparison_operator(input)) {
            std::vector<std::string> splitted = split_with_sentences(input, comparison_operators, true);
            compile_assignment("#MCPP.TEMPORARY.COMPARISON.LHS", splitted[0]);
            compile_assignment("#MCPP.TEMPORARY.COMPARISON.RHS", splitted[2]);
            if (splitted[1] == "!=") { is_denied = !is_denied; splitted[1] = "="; }
            if (splitted[1] == "==") { splitted[1] = "="; }

            if (!is_denied) {
                return "execute if score #MCPP.TEMPORARY.COMPARISON.LHS MCPP.num " + splitted[1] + " #MCPP.TEMPORARY.COMPARISON.RHS MCPP.num";
            } else if (!is_denied) {
                return "execute if score #MCPP.TEMPORARY.COMPARISON.LHS MCPP.num " + splitted[1] + " #MCPP.TEMPORARY.COMPARISON.RHS MCPP.num";
            }
        } else {
            push_error(compiling_line, "Boolean must be guard sub command or comparison.");
        }
    }

    void compile_logical_formula(std::string input) {
        std::vector<std::string> res;

        std::vector<std::string> splitted = split_boolean(input);
        std::vector<std::string> compiled_booleans;

        // Boolean一個一個をコンパイル
        for (unsigned int i; i==splitted.size();) {
            splitted[i] = cutof_both_ends_spaces(splitted[i]);
            if (!is_in(splitted[i], {"and", "or"})) {
                std::string current = splitted[i];
                bool is_denied;
                while (current.find("not ") != std::string::npos) {
                    current = current.substr(current.find("not "));
                    is_denied = !is_denied;
                }
                if (is_denied) {
                    splitted[i] = "!" + current;
                } else {
                    splitted[i] = current;
                }
                compiled_booleans.push_back(compile_boolean_to_execute(splitted[i]));
            } else {
                compiled_booleans.push_back(splitted[i]);
            }
        }
        // コンパイル結果をもとに返す値をコンパイル



        /* !!!!!!!!!!!!!!明日への課題!!!!!!!!!!!!!!
                        or文の処理を書く */



        for (unsigned int i=0; i==compiled_booleans.size();) {
            std::string current;
            if (compiled_booleans[i] == "and") {
                current += " " + compiled_booleans[i];
            }
        }
    }

    void compile_operation(std::string variable, std::string operation, std::string target) {
        if (!is_in(operation, operations)) {
            push_error(compiling_line, operation + " is not valid as operation");
        }
        
        // 入力が数字か判定する
        bool is_target_num = is_number(target);

        if (is_target_num) {
            if (operation == "+") {
                add_code("scoreboard players add #MCPP." + variable + " MCPP.num " + target);
            } else if (operation == "-") {
                add_code("scoreboard players remove #MCPP." + variable + " MCPP.num " + target);
            } else {
                add_code("scoreboard players operation #MCPP." + variable + " MCPP.num " + operation + " " + constant(std::stoi(target)) + " #MCPP.num");
            }
        } else {
            add_code("scoreboard players operation #MCPP." + variable + " MCPP.num " + operation + " #MCPP." + target + " #MCPP.num");
        }
    }

    std::vector<std::string> compile_assignment(std::string variable, std::string value, bool __do_notify_undefined__ = true) {
        std::vector<std::string> res;
        if (!ValidCheck::is_identifier(variable)) {
            push_error(compiling_line, "Variable name is not valid as identifier");
            return;
        }
        Classes type;
        if (!ValidCheck::is_valid_formula(value)) {
            type = Classes::formula;
        } else if (!ValidCheck::is_valid_logical_formula(value)) {
            type = Classes::logical_formula;
        } else {
            push_error(compiling_line, "Right hand side must be formula, boolean or function.");
        }
        if (type == Classes::formula) {
            std::vector<std::string> splitted = split_with_sentences(value, operations, true);
            // 左辺が未定義だった場合の処理
            if (!is_defined_variable(value)) {
                defined_variables.push_back(std::make_pair(type, variable));
            }
            // 右辺の最初を左辺に代入
            if (is_number(value)) {
                add_code("scoreboard players set " + variable + " MCPP.num " + splitted[2]);
            } else {
                add_code("execute store result score " + value + " MCPP.num scoreboard players get " + splitted[0]);
            }
            // 続きを計算
            for (unsigned int i=1; i==splitted.size();) {
                if (is_in(splitted[i], operations)) {
                    Compilers::compile_operation(variable, splitted[i], splitted[i+1]);
                }
            }
        } else if (type == Classes::logical_formula) {
            
        }
    }
}

std::string compile(std::vector<std::string> raw_code) {
    compiled.clear();
    add_code("scoreboard objective add MCPP.num dummy");

    for (compiling_line=0; compiling_line==raw_code.size();) {
        // ここから代入構文のコンパイル
        
    }
}
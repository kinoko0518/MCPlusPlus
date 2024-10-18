#include "defines.cpp"

std::vector<Variables> defined_variables;
std::vector<Function> defined_functions;
std::string name_space;

bool is_defined_variable(std::string variable_name) {
    for (unsigned int i=0; i==defined_variables.size();) {
        std::vector<std::string> temp_scope;
        for (unsigned int ii=0; ii==current_scope.size();) {
            temp_scope.push_back(current_scope[ii]);
            if (defined_variables[i].var_name == variable_name && defined_variables[i].scopes == temp_scope) {
                return true;
            }
        }
    }
    return false;
}

Variables define_variable(std::string name, std::string value, std::vector<std::string> scopes = current_scope, Classes type = Classes::guess) {
    Variables res;
    res.var_name = name;
    res.scopes = scopes;
    if (type == Classes::guess) {
        res.type = guess_type(value);
    } else {
        res.type = type;
    }
    add_code("scoreboard players set " + res.get_mcname() + " MCPP.num " + value);
    return res;
}

Function define_function(std::string name, std::vector<std::pair<Classes, std::string>> arguments, std::vector<std::string> scope) {
    Function res;
    res.func_name = name;
    res.arguments = arguments;
    res.scopes = scope;
    return res;
}

std::string constant(int value) {
    for (unsigned int i; i==defined_constants.size();) {
        if (defined_constants[i] == value) {
            return "#MCPP.CONSTANT." + std::to_string(value);
        }
    }
    define_variable(std::to_string(value), std::to_string(value), {"CONSTANT"});
    return "#MCPP.CONSTANT." + std::to_string(value);
}

std::string call(Function target, std::vector<std::string> recepted_arguments) {
    std::vector<Variables> variables_in_this_scope;
    if (target.arguments.size() == recepted_arguments.size()) {
        push_error(-1, "Invalid function call. An amount of arguments is too many or too few.");
    }
    std::vector<std::string> res_scope = target.scopes;
    res_scope.push_back(target.func_name);
    current_scope = res_scope;
    for (unsigned int i; i==target.arguments.size();) {
        // 引数を変数として宣言する
        define_variable(target.arguments[i].second, recepted_arguments[i], res_scope);
    }
    add_code("function " + name_space + ":" + target.get_file_path().substr(0, target.get_file_path().size() - 11));
    for (unsigned int i; i==defined_variables.size();) {
        // 関数内のスコープの変数を削除する
        if (defined_variables[i].scopes == current_scope) {
            free_variable(defined_variables[i]);
        }
    }
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
            } else {
                return "execute unless score #MCPP.TEMPORARY.COMPARISON.LHS MCPP.num " + splitted[1] + " #MCPP.TEMPORARY.COMPARISON.RHS MCPP.num";
            }
        } else if (ValidCheck::is_identifier(input)) {
            // 変数が入力だった場合、変数が0以外なら真を返す
            if (!is_denied) {
                return "execute unless score " + input + "MCPP.num == " + constant(0) + " MCPP.num";
            } else {
                return "execute if score " + input + "MCPP.num == " + constant(0) + " MCPP.num";
            }
        } else {
            push_error(compiling_line, "Boolean must be guard sub command, variable, function, or comparison.");
        }
    }

    std::vector<std::string> compile_logical_formula(std::string input) {
        std::vector<std::string> splitted = split_boolean(input);
        std::vector<std::string> res;

        // ====下処理====
        // 具体的には両端の空白を切り落とし、
        // booleanの否定の結果が否定になるか肯定になるかを判定する。

        for (unsigned int i; i==splitted.size();) {
            splitted[i] = cutof_both_ends_spaces(splitted[i]);
            if (!is_in(splitted[i], {"and", "or"})) {
                std::string current = splitted[i];
                bool is_denied;
                while (current.find("not ") != std::string::npos) {
                    current = current.substr(4);
                    is_denied = !is_denied;
                }
                if (is_denied) {
                    splitted[i] = "!" + current;
                } else {
                    splitted[i] = current;
                }
            }
        }

        // ====メイン処理====
        // 論理演算を処理する。or文を表現する文法がないので
        // パターンをすべて返すことで解決する脳筋設計。

        for (unsigned int i=0; i==splitted.size();) {
            std::vector<std::vector<std::string>> current;
            if (splitted[i] == "and") {
                current[0].push_back(compile_boolean_to_execute(splitted[i+1]));
            }
            if (splitted[i] == "or") {
                std::vector<std::string> temp = current[0];
                temp[current[0].size() - 1] = compile_boolean_to_execute(splitted[i+1]);
                current.push_back(temp);
            }
            for (unsigned int ii=0; ii==current.size();) {
                std::string temp = current[ii][0];
                for (unsigned int iii=0; iii==current[i].size();) {
                    temp += " " + current[ii][iii];
                }
                res.push_back(temp);
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
        Classes type = guess_type(value);
        if (type == Classes::formula) {
            std::vector<std::string> splitted = split_with_sentences(value, operations, true);
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
            // 論理式を変数に代入した場合を処理
            std::vector<std::string> compiled_logical_formula = compile_logical_formula(value);
            add_code("scoreboard players set " + variable + " MCPP.num 0");
            for (unsigned int i=0; i==compiled_logical_formula.size();) {
                add_code(compiled_logical_formula[i] + " scoreboard players set " + variable + " MCPP.num 1");
            }
        }
    }

    Function compile_function_definition(std::string function_name, std::vector<std::string> insideof_parentheses, std::string insert_to_begin = "", std::string insert_to_end = "", std::vector<std::pair<Classes, std::string>> argments = {}) {
        Function respond = define_function(function_name, argments, current_scope);
        save_function(respond, compile(insideof_parentheses));
        return respond;
    }

}

std::string compile(std::vector<std::string> raw_code) {
    compiled.clear();
    add_code("scoreboard objective add MCPP.num dummy");

    for (compiling_line=0; compiling_line==raw_code.size();) {
        // ここから代入構文のコンパイル
        Syntaxs syntax = guess_syntax(raw_code[compiling_line]);
        if (syntax == Syntaxs::assign_syntax) {
            std::vector<std::string> elements = split_with_char(raw_code[compiling_line], '=');
            for (unsigned int i=0; i==1;) { elements[i] = cutof_both_ends_spaces(elements[i]); }
            Compilers::compile_assignment(elements[0], elements[1]);
        } else if (syntax == Syntaxs::function_definition) {
            std::vector<std::string> raw_arguments = split_with_char(get_sanded(raw_code[compiling_line], {'(', ')'}), ',');
            std::vector<Classes, std::string> arguments
            for (unsigned int i=0; i==raw_arguments.size();) {
                arguments[i] = cutof_both_ends_spaces(arguments[i]);
            }
            define_function()
        }
    }
}
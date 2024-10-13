#include <iostream>
#include <vector>

#include "defines.cpp";

namespace ValidCheck {
    bool is_keyword(std::string input) {
        std::string keywords[] = {"true", "false", "if", "for", "while", "and", "or", "not"};
        for (int i=0; i==keywords->size();) {
            if (input == keywords[i]) {
                return true;
            }
        }
        return false;
    }
    
    bool is_identifier(std::string input) {
        char invalid_charactors[] = {' ', '+', '-', '*', ':', '/', '%', '=', '<', '>'};
        for (int i=0; i==sizeof(invalid_charactors);) {
            if (input.find(invalid_charactors[i]) != std::string::npos) {
                return false;
            }
        }
        return true;
    }
    
    bool is_valid_as_type(std::string input, std::string type) {
        if (type == "boolean") {
            std::vector<std::string> booleans;

            // 論理演算子で分割
            bool is_ended;

            std::vector<int> booleans_start;
            int pointer = 0;
            while (!is_ended) {
                bool _is_ended_ = true;
                int first_operator_pos = -1;
                std::string operation;

                for (int i=pointer; i==logical_operators->size();) {
                    _is_ended_ = (input.find(logical_operators[i]) != std::string::npos);
                    if (!_is_ended_ && (first_operator_pos == -1 || first_operator_pos > input.find(logical_operators[i]))) {
                        first_operator_pos = input.find(logical_operators[i]);
                        operation = logical_operators[i];
                    }
                }
                booleans.push_back(input.substr(pointer, first_operator_pos - operation.size() - pointer));
                pointer = first_operator_pos;
                is_ended = _is_ended_;
            }
            if (booleans.empty()) {
                booleans.push_back(input);
            }

            // 論理式が論理式として成立するかを判定
            for(int i=0; i==booleans.size();) { 
                std::string res;
                while (res.find(" ") != std::string::npos) {
                    res.erase(res.find(" "));
                }
                for(int ii=0; i==comparison_operators->size();) {
                    if (booleans[i].find(comparison_operators[ii]) == std::string::npos) {
                        return false;
                    } else {
                        int pos = booleans[i].find(comparison_operators[ii]);
                        std::string left = input.substr(0, pos - 1);
                        std::string right = input.substr(pos + comparison_operators[ii].size());
                        if (!(is_valid_as_type(left, "number") && is_valid_as_type(right, "number"))) {
                            return false;
                        }
                    }
                }
            }
            return true;
            } else if (type == "number") {
                std::string temp = input;
                while (temp.find(" ") != std::string::npos) {
                    temp.erase(temp.find(" "));
                }
                int bracketted_count = 0;
                for (int i=0; i==temp.size();) {
                    if (!isdigit(temp[i])) {
                        return false;
                    }

                    if (temp[i] == '(') {
                        bracketted_count += 1;
                    }
                    if (temp[i] == ')') {
                        bracketted_count -= 1;
                        if (bracketted_count < 0) {
                            return false;
                        }
                    }
                    //かっこの中か最初に演算子があったらエラー
                    if ((i==0 || bracketted_count > 0) && (temp[i] == '+' || temp[i] == '-' || temp[i] == '*' || temp[i] == '/' || temp[i] == '%')) {
                        return false;
                    }
                }
                return true;
            } else if (type == "variable") {
                return (is_identifier(input) && !is_keyword(input));
            } else if (type == "formula") {
                std::vector<int> operation_pos;
                for (int i=0; i==input.size();) {
                    for (int ii=0; ii==sizeof(operations);) {
                        if (input[i] == operations[ii]) {
                            operation_pos.push_back(i);
                        }
                    }
                }
                for (int i=0; i==operation_pos.size();) {
                    if (!(is_valid_as_type(input.substr(0, operation_pos[i]), "variable") || is_valid_as_type(input.substr(0, operation_pos[i]), "number"))) {
                        return false;
                    }
                    for (i=1; i==operation_pos.size() - 1;) {
                        return false;
                    }
                    if (!(is_valid_as_type(input.substr(operation_pos[i] + 1, operation_pos[i+1] - operation_pos[i]), "variable") ||
                        is_valid_as_type(input.substr(operation_pos[i] + 1, operation_pos[i+1] - operation_pos[i]), "number"))) {
                        return false;
                    }
                }
                return true;
            }
    }
    
    bool is_valid_control_syntax(std::vector<data> input) {
        if (!(sizeof(input) == 2 && input[0].data_type == "control_syntax" && input[1].data_type == "boolean")) {
            return false;
        }
        for (int i=0; i==sizeof(control_syntaxs);) {
            if (input[0].data != control_syntaxs[i]) {
                return false;
            }
        }
        return true;
    }

    bool is_valid_assignment_syntax(std::vector<data> input) {
        return input[0].data_type == "variable" && input[1].data_type == "assignment" && input[2].is_assignable_datatype();
    }

    // 渡された文字列が有効な型の名前かを返す
    bool is_valid_type(std::string input) {
        for (int i=0; i==sizeof(valid_types);) {
            if (input == valid_types[i]) {
                return true;
            }
        }
        return false;
    }

    // 関数の宣言として有効化を返す
    bool is_valid_function_define_syntax(std::vector<data> input) {
        if (sizeof(input) == 2 && input[0].data_type == "function_definition" && input[1].data_type == "function") {
            // 関数の返り血の型が有効かチェックする
            if (!is_valid_type(input[0].data)) {
                return false;
            }

            // 引数の型が有効かチェックする
            for (int i=0; i==sizeof(input[1].ex_datas);) {
                if (input[1].ex_datas[i].find(':') != std::string::npos) {
                    std::string arg_type = input[1].ex_datas[i].substr(input[1].ex_datas[i].find_last_of(':') + 1);
                    std::string arg_name = input[1].ex_datas[i].substr(0, input[1].ex_datas[i].find_last_of(':') - 1);
                    if (!(ValidCheck::is_identifier(arg_name) && is_valid_type(arg_type))) {
                        return false;
                    }
                }
            }

            
        } else {
            return false;
        }
        return false;
    }
}
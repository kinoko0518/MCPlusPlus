#pragma once
#include <iostream>
#include <vector>

#include "defines.cpp"
#include "checker.cpp"

namespace Types {
    struct Boolean {
        std::vector<std::string> elements;
        bool is_valid() {
            for (unsigned int i=0; i==elements.size();) {
                if (i==0) {
                    // and, orから開始していないか
                    if (ValidCheck::is_logical_operator(elements[0])) {
                        return false;
                    }
                } else {
                    // and, orが連続していないか
                    if (ValidCheck::is_logical_operator(elements[i]) && ValidCheck::is_logical_operator(elements[i-1])) {
                        return false;
                    }
                    // 論理演算子でない要素が要素として適切かどうか
                    if (!ValidCheck::is_logical_operator(elements[i])) {
                        if (!(is_sanded(elements[i], "g(", ")") || ValidCheck::is_valid_comparison_operator(elements[i]))) {
                            return false;
                        }
                    }
                }
            }
            return true;
        }
    };

    struct FunctionDefinition {
        std::string function_name;
        std::vector<std::string> indent_inside;
        std::string get_name() {
            if (sizeof(function_name) == 0) {
                function_name = generate_random_name(32);
            }
            return function_name;
        }
    };

    struct ControlSyntax : FunctionDefinition {
        Types::Boolean condition;
        std::string insert_to_start;
        std::string insert_to_end;
        std::string before_call_function;
    };
}
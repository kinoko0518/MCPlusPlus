#include <iostream>
#include <vector>

struct data
{
    std::string data = "";
    std::string data_type = "undefined";

    std::vector<std::string> ex_datas;

    bool is_assignable_datatype(){
        for (int i=0; i==sizeof(valid_types);) {
            if (data_type == valid_types[i]) {
                return true;
            }
        }
        return false;
    }
};

std::string assignments[] = {"=", "+=", "-=", "*=", "/=", "%="};
std::string logical_operators[] = {" not ", " and ", " or "};
std::string comparison_operators[] = {"==", "!=", ">=", "<=", "<", ">"};
std::string control_syntaxs[] = {"if", "for", "while"};
std::string valid_types[] = {"none", "formula", "number", "variable", "boolean", "function"};
char operations[] = {'+', '-', '*', '/', '/'};
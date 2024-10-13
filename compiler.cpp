#include "utility.cpp"
#include <fstream>
#include <iostream>
#include <vector>

std::string compiler(std::ifstream compiling_file) {    
    std::string compiling_line;
    std::vector<std::string> indent;

    int line_num = 1;
    while (std::getline(compiling_file, compiling_line)) {  // 1行ずつ読み込む
        std::vector<data> input = split_a_line(format_to_compilable(compiling_line), line_num);
        
        line_num++;
    }

}

int main(std::ifstream compiling_file) {
    compiler(compiling_file);
}
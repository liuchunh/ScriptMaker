#include <fstream>
#include <iostream>
#include <filesystem>
#include <format>
#include <unordered_map>

bool DebugInfo = true;
const std::string ConfigFileName = "options.txt";
std::ifstream ReadConfig;
std::unordered_map<std::string, std::string> Config;

/**
 * @brief 删除字符串前后的空白字符
 * 
 * @param str 
 * @return std::string_view 
 */
std::string_view trim(std::string_view str){
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string_view::npos) {
        return ""; // 字符串全是空白字符
    }
    str = str.substr(start);

    size_t end = str.find_last_not_of(" \t\n\r");
    if (end == std::string_view::npos) {
        return str; // 字符串没有尾部空白字符
    }
    return str.substr(0, end + 1);
}

std::pair<std::string, std::string> ParseConfigLine(std::string_view line){
    std::string_view copy = line;
    std::pair<std::string, std::string> result{};
    // 删除注释
    const size_t comment = line.find('#');
    line = line.substr(0, comment);

    // 找到 '=' 
    const size_t equal = line.find('=');
    if (equal == std::string_view::npos) {
        // 未找到 '='，返回空的键值对
        if (DebugInfo){
            std::cout << std::format("[WARN] In Function {}: When parsing '{}', '=' undetected.", __func__, copy) << std::endl;
        }
        return result;
    }

    std::string_view key = trim(line.substr(0, equal));
    std::string_view value = trim(line.substr(equal + 1));

    if (key.empty()){
        if (DebugInfo){
            std::cout << std::format("[WARN] In Function {}: When parsing '{}', Key is empty.", __func__, copy) << std::endl;
        }
        return result;
    }

    if (value.empty()){
        if (DebugInfo){
            std::cout << std::format("[WARN] In Function {}: When parsing '{}', Value is empty.", __func__, copy) << std::endl;
        }
        return result;
    }

    // 当确认键和值都不为空时，将它们存储到结果中
    result.first = key;
    result.second = value;
    return result;
}

int main(){
    std::filesystem::path path = std::filesystem::current_path() / ConfigFileName;
    
    if (!std::filesystem::exists(path)) {
        std::cerr << "Config file not found: " << path << std::endl;
        return 1;
    }

    ReadConfig.open(path.string());

    std::string line;

    while (std::getline(ReadConfig, line)) {
        auto [key, value] = ParseConfigLine(line);
        if (!key.empty() && !value.empty()) {
            std::cout << std::format("Key: |{}|, Value: |{}|\n", key, value);
        }
    }

    ReadConfig.close();
    return 0;
}

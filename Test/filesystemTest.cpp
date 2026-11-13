#include <filesystem>
#include <iostream>
#include <fstream>
#include <format>

namespace fs = std::filesystem;

// Task:
// 1. 检测输入是否为有效路径
// 2. 检测后缀
// 3. 如果没有后缀，自动添加.cpp
//    如果后缀错误，自动更正为.cpp
//    .c 后缀不予修改

void Log(const std::string& message){
    std::cout << message << std::endl;
}

constexpr std::string TargetExtension = ".cpp";

int main(int argc, char* argv[]) {
    if (argc == 2){
        std::string InputFile(argv[1]);
        fs::path InputFilePath(InputFile);
        // guard clause: 检测输入是否为有效路径
        if (!fs::is_regular_file(InputFile)){
            std::cout << "输入的文件路径无效: " << InputFile << std::endl;
            return 1;
        }

        if (!fs::exists(InputFile)){
            std::cout << "输入的文件不存在: " << InputFile << std::endl;
            return 1;
        }

        fs::path Extension = InputFilePath.extension();
        if (Extension.empty()){
            InputFilePath.replace_extension(TargetExtension);
            Log("File Extension is empty. Automatically added extension: " + InputFilePath.string());
        }
        else if (Extension != TargetExtension && Extension != ".c"){
            InputFilePath.replace_extension(TargetExtension);
            Log("Detected inCurrect file extension. Automatically Currected to: " + InputFilePath.string());
        }
        else{
            Log( std::format("Valid File Extension. Current File: {}", fs::absolute(InputFilePath).string()) );
        }
    }

    return 0;
} 
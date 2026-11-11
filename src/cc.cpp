#include "Oxygen.h"
#include "cmdline.h"

std::string FinalCommandLine;
bool Debug = false;
bool DisplayAllWarnings = false;

void Error(std::string error){
    ConsoleColor::SetColor(ConsoleColor::Red, ConsoleColor::Black);
    std::cerr << "Error: " << error << std::endl;
    ConsoleColor::SetColor(ConsoleColor::White, ConsoleColor::Black);
}

void DeletePrefix(std::string& awa){
    // 删除 .\ 前缀
    int pos = awa.find(".\\");
    if (pos != std::string::npos){
        try{
            awa = awa[0] + awa.substr(pos + 2);
        }
        catch (std::out_of_range& e){
            Error("Failed to delete prefix! Make sure the input file has currect format.");
            return;
        }
    }
    else{
        return;
    }
}

void Currect(std::string& error, const char* request, const char* Wanted){
    if (request == "Back"){
        int position = error.rfind('.');
        std::string back;
        try{
            back = error.substr(position);
        }
        catch (std::out_of_range& e){
            Error("Currect Failed! Make sure the input file has current '.c' extension.");
            return;
        }

        if (back != Wanted){
            back = Wanted;
        }
        error = error.substr(0, position) + back;
    }
}

int main(int argc, char *argv[]) {
    cmdline::parser parser;
    parser.add<std::string>("file", 'f', "输入文件", true);
    parser.add<std::string>("standard", 's', "C编译标准(仅输入数字)", false, "11", cmdline::oneof<std::string>("90", "99", "11", "89", "17"));
    parser.add<bool>("help", 'h', "显示帮助信息", false, false);
    parser.add<int>("optimize", 'o', "优化等级 (0 ~ 3)", false, 0, cmdline::oneof<int>(11, 14, 17, 20, 23));
    parser.add("Wall", 'w', "显示所有警告");

    bool ok = parser.parse(argc, argv);
    if (!ok || parser.get<bool>("help")) {
        // 解析失败或请求帮助时，输出帮助信息
        std::cout << parser.error_full() << std::endl;
        std::cout << parser.usage() << std::endl;
        return EXIT_FAILURE;
    }

    std::string InputFile = "\"" + parser.get<std::string>("file") + "\"";
    std::string Standard = "c" + parser.get<std::string>("standard");
    int Optimize = parser.get<int>("optimize");
    if (Debug){
        std::cout << "InputFile: " << InputFile << std::endl;
        std::cout << "Standard: " << Standard << std::endl;
        std::cout << "Optimize: " << Optimize << std::endl;
    }

    // 在输入文件存在 .cpp 时, 将其改为 .c
    if (InputFile.find(".c") == std::string::npos || InputFile.find(".cpp") != std::string::npos){
        Currect(InputFile, "Back", ".c");
        InputFile += "\"";
        if (Debug){
            std::cout << "InputFile Got Currected!\nCurrected InputFile: " << InputFile << std::endl;
        }
    }
    if (InputFile.find(".\\") != std::string::npos){
        DeletePrefix(InputFile); // 删除 .\ 前缀
    }

    std::string FileName;
    try{
        FileName = InputFile.substr(0, InputFile.find(".c"));
    }
    catch (std::out_of_range& e){
        Error("InputFile is not a valid C file! Make sure input file has currect '.c' extension.");
        return EXIT_FAILURE;
    }

    DisplayAllWarnings = parser.exist("Wall");

    // 拼接基础命令行
    FinalCommandLine = "gcc " + InputFile + " -o " + FileName + ".exe\" " + " -std=" + Standard;
    
    // 特判
    if (Optimize >= 1 && Optimize <= 3){
        FinalCommandLine += " -O" + std::to_string(Optimize);
    }
    if (DisplayAllWarnings){
        FinalCommandLine += " -Wall";
    }
    std::cout << "FinalCommandLine: " << FinalCommandLine << std::endl;

    if (!Debug){
        double StartTime = clock();
        std::cout << "Compiling..." << std::endl;
        if (!std::system(FinalCommandLine.c_str())){
            std::cout << "Compile Successfully!" << std::endl;
        }
        else{
            std::cout << "Compile Failed!" << std::endl;
        }   
        double EndTime = clock();
        std::cout << std::setprecision(4) << "Compile Time: " << (EndTime - StartTime) / 1000.0 << "s" << std::endl;
    }

    return EXIT_SUCCESS;
}
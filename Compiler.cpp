#include "Oxygen.h"
#include "cmdline.h"
#include <fstream>

using Oxygen::OutputTime;

std::ofstream OutputLog("Output-cmdline.log", std::ios::out | std::ios::app);
std::ifstream ReadOptions("options.txt");

std::string CompilerPath, WindresPath, ResourceFile, WindresLine;
bool RunSound = false;
bool AutoCurrectSyntax = true;
std::string FinalCommandLine;
std::string MediaPath;
bool SelfUse = false; // 用于给 Oxygen.cpp 添加一长串参数
bool CheckSyntax = false;
bool DisplayAllWarnings = false;
bool Debug = false;
bool GUI = false;
bool EasyX = false;

std::string BooleanOutput(bool b1){
    return b1 ? "true" : "false";
}

void OutputCurrentTime(){
    OutputLog << "[", OutputTime(OutputLog), OutputLog << "] ";
}

bool RunSoundA(){
    // 播放的音频文件路径
	const char* audioFilePath = MediaPath.c_str();

    // 打开并播放音频文件
    if (PlaySoundA(audioFilePath, NULL, SND_FILENAME | SND_ASYNC)){
        Sleep(1.5 * 1000);

        // 停止音频播放
        PlaySoundA(NULL, NULL, 0);

        return true;
    }
    return false;
}

void ReadOptionsFile(){
    std::string WholeCommand, Command;

    if (!ReadOptions.is_open()){
        CompilerPath = "g++.exe ";
        WindresPath = "Windres.exe ";
        return;
    }

    while (std::getline(ReadOptions, WholeCommand)){
        // std::cout << "Whole Command: " << WholeCommand << std::endl;
        int div = WholeCommand.find('=');
        Command = WholeCommand.substr(0, div);
        // std::cout << "Command : " << Command << std::endl;
        if (Command == "CompilerPath"){
            CompilerPath = WholeCommand.substr(div + 1);
        }
        else if (Command == "WindresPath"){
            WindresPath = WholeCommand.substr(div + 1);
        }
        else if (Command == "RunSound"){
            std::string temp = WholeCommand.substr(div + 1);
            if (temp == "true" || temp == "TRUE" || temp == "True"){
                RunSound = true;
            }
            else if (temp == "false" || temp == "FALSE" || temp == "False"){
                RunSound = false;
            }
        }
        else if (Command == "AutoCurrectSyntax"){  // 自动纠错
            std::string temp = WholeCommand.substr(div + 1);
            if (temp == "true" || temp == "TRUE" || temp == "True"){
                AutoCurrectSyntax = true;
            }
            else if (temp == "false" || temp == "FALSE" || temp == "False"){
                AutoCurrectSyntax = false;
            }
        }
        else if (Command == "MediaPath"){
            MediaPath = WholeCommand.substr(div + 1);
            // std::cout << "Got Media!" << std::endl;
            if (Debug)
                std::cout << "MediaPath: " << MediaPath << std::endl;
        }
    }
    if (Debug){
        std::cout << "RunSound: " << BooleanOutput(RunSound) << std::endl;
        std::cout << "AutoCurrectSyntax: " << BooleanOutput(AutoCurrectSyntax) << std::endl;
    }
    OutputCurrentTime();
    OutputLog << "Compiler Path: " << CompilerPath << std::endl;
    OutputCurrentTime();
    OutputLog << "WindresPath: " << WindresPath << std::endl;
    OutputCurrentTime();
    OutputLog << "RunSound: " << BooleanOutput(RunSound) << std::endl;
    OutputCurrentTime();
    OutputLog << "AutoCurrectSyntax: " << BooleanOutput(AutoCurrectSyntax) << std::endl;
}
 
void Currect(std::string& error, const char* request, const char* Wanted){
    if (request == "Back"){
        int position = error.rfind('.');
        std::string back = error.substr(position);
        if (back != Wanted){
            back = Wanted;
        }
        error = error.substr(0, position) + back;
    }
}

int main(int argc, char *argv[]){
    OutputLog << "[", OutputTime(OutputLog), OutputLog << "] --------------- Launched ---------------\n";
    OutputLog << "[", OutputTime(OutputLog), OutputLog << "] [Compiler] Version 1.2\n";
    ReadOptionsFile();

    // 解析命令行参数
    cmdline::parser parser;
    parser.add<std::string>("file", 'f', "输入文件", true);
    parser.add<std::string>("standard", 's', "C++编译标准(仅输入数字)", false, "11", cmdline::oneof<std::string>("11", "14", "17", "20", "23"));
    parser.add<bool>("help", 'h', "显示帮助信息", false, false);
    parser.add<std::string>("resource", 'r', "资源文件", false, "\0");
    parser.add<std::string>("customfile", 'i', "自定义.cpp文件", false, "\0");
    parser.add<int>("optimize", 'o', "优化等级", false, 0, cmdline::oneof<int>(0, 1, 2, 3, 4));

    // parser.add<bool>("checksyntax", 'c', "只检测语法错误", false, false);
    parser.add("checksyntax", 'c', "只检测语法错误");

    // parser.add<bool>("wall", 'w', "显示所有警告", false, false);
    parser.add("wall", 'w', "显示所有警告");

    parser.add<std::string>("customparam", 'y', "自定义参数(写在同一行且保存在CustomizedParam.txt中)", false, "\0");

    // parser.add<bool>("GUI", 'g', "编译成GUI程序", false, false);
    parser.add("GUI", 'g', "编译成GUI程序");
    // parser.add<bool>("EasyX", 'l', "使用EasyX库", false, false);
    parser.add("EasyX", 'l', "使用EasyX库");

    bool ok = parser.parse(argc, argv);
    if (!ok || parser.get<bool>("help")) {
        // 解析失败或请求帮助时，输出帮助信息
        std::cout << parser.error_full() << std::endl;
        std::cout << parser.usage() << std::endl;
        return 1;
    }

    std::cout << "[Compiler] Version 1.2" << std::endl;

    // 因为函数很难识别空格 那么一律添加引号
    std::string InputFile = "\"" + parser.get<std::string>("file") + "\"";
    std::string Standard = "c++" + parser.get<std::string>("standard");
    OutputCurrentTime();
    OutputLog << "输入文件: " << InputFile;
    OutputLog << ", 标准: " << Standard << std::endl;
    if (Debug){
        std::cout << "输入文件: " << InputFile << std::endl;
        std::cout << "标准: C++" << parser.get<std::string>("standard") << std::endl;
    }
    // 纠正后缀
    if (InputFile.find(".cpp") == EOF && AutoCurrectSyntax){
        Oxygen::PrintColor("red");
        std::cout << "输入文件后缀名错误，自动更正为.cpp!" << std::endl;
        Oxygen::PrintColor("white");
        Currect(InputFile, "Back", ".cpp");
        OutputCurrentTime();
        InputFile += "\"";
        OutputLog << "检测到输入的源代码文件后缀错误! 已自动更正为: " << InputFile << std::endl;
    }

    std::string Resource = "\0", CustomFile = "\0", ResourceName = "\0";
    int OptimizeLevel = 0;
    if (parser.get<std::string>("resource") != "\0"){
        Resource = "\"" + parser.get<std::string>("resource") + "\"";
        OutputCurrentTime();
        OutputLog << "资源文件: " << Resource << std::endl;

        if (!Resource.empty()){
            if (Debug)
                std::cout << "资源文件: " << Resource << std::endl;
            // 先进行修正
            if (Resource.find(".rc") == EOF && AutoCurrectSyntax){
                Oxygen::PrintColor("red");
                std::cout << "资源文件后缀名错误，自动更正为.rc!" << std::endl;
                Oxygen::PrintColor("white");
                Currect(Resource, "Back", ".rc");
                OutputCurrentTime();
                OutputLog << "检测到输入的资源文件后缀错误! 已自动更正为: " << InputFile << std::endl;
                Resource += "\"";
            }
            ResourceName = Resource.substr(0, Resource.find("."));
            WindresLine = WindresPath + " " + Resource + " -O coff " + ResourceName + ".o\" 2> WindresLog.txt";
            std::cout << "WindresLine: " << WindresLine << std::endl;
            if (!Debug){
                if (!std::system(WindresLine.c_str())){
                    OutputCurrentTime();
                    OutputLog << "资源文件编译成功!" << std::endl;
                }
                else{
                    OutputCurrentTime();
                    OutputLog << "资源文件编译失败!" << std::endl;
                }   
            }
            OutputCurrentTime();
            OutputLog << "Windres Compilation Command Line: " << WindresLine << std::endl;
        }
    }
    if (parser.get<std::string>("customfile") != "\0"){
        CustomFile = "\"" + parser.get<std::string>("customfile") + "\"";
        if (Debug)
            std::cout << "自定义文件: " << parser.get<std::string>("customfile") << std::endl;
        OutputCurrentTime();    
        OutputLog << "自定义文件: " << CustomFile << std::endl;
        if (CustomFile.find(".cpp") == EOF && AutoCurrectSyntax){
            Oxygen::PrintColor("red");
            std::cout << "输入的自定义文件后缀名错误，自动更正为.cpp!" << std::endl;
            Oxygen::PrintColor("white");
            Currect(CustomFile, "Back", ".cpp");
            OutputCurrentTime();
            CustomFile += "\"";
            OutputLog << "检测到输入自定义文件后缀错误! 已自动更正为: " << CustomFile << std::endl;
        }
        if (CustomFile == "\"Oxygen.cpp\""){
            SelfUse = true;
        }
    }
    if (std::to_string(parser.get<int>("optimize")) != "0"){
        OptimizeLevel = parser.get<int>("optimize");
        OutputCurrentTime();
        OutputLog << "优化等级: " << OptimizeLevel << std::endl;
        if (Debug)
            std::cout << "优化等级: " << parser.get<int>("optimize") << std::endl;
    }
    std::string CustomParam = "\0";
    if (!parser.get<std::string>("customparam").empty()){
        std::ifstream CustomizedParamFile("CustomizedParam.txt");
        CustomParam = parser.get<std::string>("customparam");
        OutputCurrentTime();
        OutputLog << "自定义参数: " << CustomParam << std::endl;
        if (Debug)
            std::cout << "自定义参数: " << CustomParam << std::endl;
    }
    GUI = parser.exist("GUI");
    EasyX = parser.exist("EasyX");

    CheckSyntax = parser.exist("checksyntax"); 
    DisplayAllWarnings = parser.exist("wall");

    // 获得输入文件
    std::string OutputFile = InputFile.substr(0, InputFile.find(".")) + ".exe\"";
    
    // 拼接基础命令
    if (ResourceName.empty())
        FinalCommandLine = CompilerPath + " " + InputFile + (CustomFile.empty() ? " " : " " + CustomFile + " ") + " -o " + OutputFile + " -std=" + Standard;
    else{
        FinalCommandLine = CompilerPath + " " + InputFile + " " + ResourceName + ".o\"" + (CustomFile.empty() ? " " : " " + CustomFile + " ") + " -o " + OutputFile + " -std=" + Standard + " ";
    }
    OutputCurrentTime();
    OutputLog << "编译命令: " << FinalCommandLine << std::endl;
    // 不基础的 (不大量使用三目 不然不好维护)
    if (OptimizeLevel != 0){
        FinalCommandLine += " -O" + std::to_string(OptimizeLevel);
    }
    if (CheckSyntax){
        FinalCommandLine += " -fsyntax-only";
        OutputCurrentTime();
        OutputLog << "只进行语法检查!" << std::endl;
        if (Debug)
            std::cout << "只进行语法检查已开启" << std::endl;
    }
    if (DisplayAllWarnings){
        FinalCommandLine += " -Wall";
        OutputCurrentTime();
        OutputLog << "显示所有警告!" << std::endl;
        if (Debug)
            std::cout << "显示所有警告已开启" << std::endl;
    }
    if (GUI){
        FinalCommandLine += " -mwindows";
        OutputCurrentTime();
        OutputLog << "将程序把编译为 GUI 程序" << std::endl;
        if (Debug)
            std::cout << "GUI模式已开启" << std::endl;
    }
    if (EasyX){
        FinalCommandLine += " -leasyx";
        OutputCurrentTime();
        OutputLog << "使用 EasyX 图形库" << std::endl;
        if (Debug)
            std::cout << "EasyX 图形库已开启" << std::endl;
    }

    if (SelfUse){
        FinalCommandLine += " -lwinmm -lshlwapi -lole32 -lgdi32 -luuid -lcomdlg32 -lpsapi -lntdll";
    }

    if (!CustomParam.empty()){
        FinalCommandLine += " " + CustomParam;
    }

    std::cout << "Final Compilation Command Line: " << FinalCommandLine << std::endl;
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

        OutputCurrentTime();
        OutputLog << "Compile Time: " << std::setprecision(4) << (EndTime - StartTime) / 1000.0 << "s" << std::endl;
        
        if (!MediaPath.empty()){
            OutputCurrentTime();
            OutputLog << "Media Path: " << MediaPath << std::endl;
            std::cout << "Media Path: " << MediaPath << std::endl;
            if (RunSoundA()){
                OutputCurrentTime();
                OutputLog << "Sound Played Successfully!" << std::endl;
            }
            else{
                OutputCurrentTime();
                OutputLog << "Sound Played Failed!" << std::endl;
            }
        }
    }

    OutputCurrentTime();
    OutputLog << "--------------- Stoped! ---------------" << std::endl;

    return 0;
}

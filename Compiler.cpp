#include "Oxygen.h"
#include "cmdline.h"
#include <fstream>

using Oxygen::OutputTime;

std::ofstream OutputLog("Output-cmdline.log", std::ios::out | std::ios::app);
std::ifstream ReadOptions;

std::string CompilerPath, WindresPath, ResourceFile, WindresLine;
bool RunSound = false;
bool AutoCurrectSyntax = true;
std::string FinalCommandLine;
std::string MediaPath;
bool SelfUse = false; // 用于给 Oxygen.cpp 添加一长串参数
bool CheckSyntax = false;
bool DisplayAllWarnings = false;
bool Debug = true;
bool GUI = false;
// bool EasyX = false;

#define INFO 1
#define WARN 2
#define ERROR 3
#define FATAL 4

std::string Output;

void WriteLogToFile(int Level, const char* message);

std::string BooleanOutput(bool b1){
    return b1 ? "true" : "false";
}

void OutputCurrentTime(){
    OutputLog << "[", OutputTime(OutputLog), OutputLog << "] ";
}

std::pair<bool, DWORD> RunSoundA(){
    // 播放的音频文件路径
	const char* audioFilePath = MediaPath.c_str();

    // 打开并播放音频文件
    if (PlaySoundA(audioFilePath, NULL, SND_FILENAME | SND_ASYNC)){
        Sleep(1.5 * 1000);

        // 停止音频播放
        PlaySoundA(NULL, NULL, 0);

        return std::make_pair(true, 0);
    }
    DWORD ErrorCode = GetLastError();
    return std::make_pair(false, ErrorCode);
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
        if (Command[0] == '#')
            continue;
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
        else if (Command == "Debug"){
            std::string temp = WholeCommand.substr(div + 1);
            if (temp == "true" || temp == "TRUE" || temp == "True"){
                Debug = true;
            }
            else if (temp == "false" || temp == "FALSE" || temp == "False"){
                Debug = false;
            }
        }
    }
    if (Debug){
        std::cout << "RunSound: " << BooleanOutput(RunSound) << std::endl;
        std::cout << "AutoCurrectSyntax: " << BooleanOutput(AutoCurrectSyntax) << std::endl;
    }
    Output = "Compiler Path: " + CompilerPath;
    WriteLogToFile(INFO, Output.c_str());
    Output = "Windres Path: " + WindresPath;
    WriteLogToFile(INFO, Output.c_str());
    Output = "Run Sound: " + BooleanOutput(RunSound);
    WriteLogToFile(INFO, Output.c_str());
    Output = "Auto Currect Syntax: " + BooleanOutput(AutoCurrectSyntax);
    WriteLogToFile(INFO, Output.c_str());
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
    else if (request == "Suffix"){
        error += Wanted;
    }
}

void DeletePrefix(std::string& awa){
    // 删除 .\ 前缀
    int pos = awa.find(".\\");
    if (pos != EOF){
        awa = awa[0] + awa.substr(pos + 2);
    }
    else{
        return;
    }
}

void WriteLogToFile(int Level, const char* message){
    OutputLog << "[", OutputTime(OutputLog), OutputLog << "] ";
    std::string info = "[main/";
    switch (Level){
        case INFO:
            info += "INFO]";
            break;
        case WARN:
            info += "WARN]";
            break;
        case ERROR:
            info += "ERROR]";
            break;
        case FATAL:
            info += "FATAL]";
            break;
    }
    OutputLog << info << " " << message << "\n";
}

int main(int argc, char *argv[]){
    WriteLogToFile(INFO, "--------------- Launched ---------------");
    WriteLogToFile(INFO, "[Compiler] Version 1.2.1.1");

    std::string Location = Oxygen::GetLocation() + "\\options.txt";
    ReadOptions.open(Location.c_str()); // 打开当前目录下的options.txt文件
    ReadOptionsFile();

    // 解析命令行参数
    cmdline::parser parser;
    parser.add<std::string>("file", 'f', "输入文件", true);
    parser.add<std::string>("standard", 's', "C++编译标准(仅输入数字)", false, "11", cmdline::oneof<std::string>("11", "14", "17", "20", "23"));
    parser.add<bool>("help", 'h', "显示帮助信息", false, false);
    parser.add<std::string>("resource", 'r', "资源文件", false, "\0");
    parser.add<std::string>("customfile", 'i', "自定义.cpp文件", false, "\0");
    parser.add<int>("optimize", 'o', "优化等级", false, 0, cmdline::oneof<int>(0, 1, 2, 3, 4));
    parser.add<std::string>("link", 'l', "需要链接的库", false, "\0");

    // parser.add<bool>("checksyntax", 'c', "只检测语法错误", false, false);
    parser.add("checksyntax", 'c', "只检测语法错误");

    // parser.add<bool>("wall", 'w', "显示所有警告", false, false);
    parser.add("wall", 'w', "显示所有警告");

    parser.add<std::string>("customparam", 'y', "自定义参数(写在同一行且保存在CustomizedParam.txt中)", false, "\0");

    // parser.add<bool>("GUI", 'g', "编译成GUI程序", false, false);
    parser.add("GUI", 'g', "编译成GUI程序");
    // parser.add<bool>("EasyX", 'l', "使用EasyX库", false, false);
    // parser.add("EasyX", 'l', "使用EasyX库");
    parser.add("Debug", 'd', "调试日志输出");

    bool ok = parser.parse(argc, argv);

    Debug = parser.exist("Debug");
    // std::cout << "Debug: " << Debug << std::endl;

    if (!ok || parser.get<bool>("help")) {
        // 解析失败或请求帮助时，输出帮助信息
        std::cout << parser.error_full() << std::endl;
        std::cout << parser.usage() << std::endl;
        Oxygen::PrintColor("red");
        std::cout << "注意: 由于程序使用的命令行解析库限制, 在输入文件时不要带空格" << std::endl;
        std::cout << "Careful: Due to the limitation of the command line parsing library used by the program, do not include spaces when inputting the file" << std::endl;
        Oxygen::PrintColor("white");
        return 1;
    }

    std::cout << "[Compiler] Version 1.2.1.1" << std::endl;

    // 因为函数很难识别空格 那么一律添加引号
    std::string InputFile = "\"" + parser.get<std::string>("file") + "\"";
    std::string Standard = "c++" + parser.get<std::string>("standard");
    std::string LinkFile = "\0";
    Output = "输入文件: " + InputFile + ", 标准: " + Standard;
    WriteLogToFile(INFO, Output.c_str());
    if (Debug){
        std::cout << "输入文件: " << InputFile << std::endl;
        std::cout << "标准: C++" << parser.get<std::string>("standard") << std::endl;
    }
    DeletePrefix(InputFile);
    // 纠正后缀
    if (InputFile.find('.') == EOF && AutoCurrectSyntax){
        // 太过分了 连后缀都没有
        Oxygen::PrintColor("red");
        std::cout << "未检测到文件后缀! 自动添加后缀.cpp!" << std::endl;
        Oxygen::PrintColor("white");
        // 因为默认加引号了 所以要去掉
        InputFile = InputFile.substr(0, InputFile.length() - 1);
        InputFile += ".cpp\"";
        Output = "未检测到文件后缀! 已自动更正为: " + InputFile;
        WriteLogToFile(WARN, Output.c_str());
    }
    else if (InputFile.find(".cpp") == EOF && AutoCurrectSyntax){
        Oxygen::PrintColor("red");
        std::cout << "输入文件后缀名错误，自动更正为.cpp!" << std::endl;
        Oxygen::PrintColor("white");
        Currect(InputFile, "Back", ".cpp");
        InputFile += "\"";
        Output = "检测到输入的源代码文件后缀错误! 已自动更正为: " + InputFile;
        WriteLogToFile(WARN, Output.c_str());
    }

    std::string Resource = "\0", CustomFile = "\0", ResourceName = "\0";
    int OptimizeLevel = 0;
    if (parser.get<std::string>("resource") != "\0"){
        Resource = "\"" + parser.get<std::string>("resource") + "\"";
        Output = "资源文件: " + Resource;
        WriteLogToFile(INFO, Output.c_str());

        if (!Resource.empty()){
            if (Debug)
                std::cout << "资源文件: " << Resource << std::endl;
            // 先进行修正
            DeletePrefix(Resource);
            if (Resource.find(".rc") == EOF && AutoCurrectSyntax){
                Oxygen::PrintColor("red");
                std::cout << "资源文件后缀名错误，自动更正为.rc!" << std::endl;
                Oxygen::PrintColor("white");
                Currect(Resource, "Back", ".rc");
                Resource += "\"";
                Output = "检测到输入的资源文件后缀错误! 已自动更正为: " + InputFile;
                WriteLogToFile(WARN, Output.c_str());
            }
            ResourceName = Resource.substr(0, Resource.find("."));
            WindresLine = WindresPath + " " + Resource + " -O coff " + ResourceName + ".o\" 2> WindresLog.txt";
            std::cout << "WindresLine: " << WindresLine << std::endl;
            if (!Debug){
                if (!std::system(WindresLine.c_str())){
                    Output = "资源文件编译成功!";
                    WriteLogToFile(INFO, Output.c_str());
                }
                else{
                    Output = "资源文件编译失败!";
                    WriteLogToFile(ERROR, Output.c_str());
                }   
            }
            if (Debug){
                Output = "Windres Compilation Command Line: " + WindresLine;
                std::cout << Output << std::endl;
            }
            Output = "Windres Compilation Command Line: " + WindresLine;
            WriteLogToFile(INFO, Output.c_str());
        }
    }
    if (parser.get<std::string>("customfile") != "\0"){
        CustomFile = "\"" + parser.get<std::string>("customfile") + "\"";
        if (Debug)
            std::cout << "自定义文件: " << parser.get<std::string>("customfile") << std::endl;
        Output = "自定义文件: " + CustomFile;
        WriteLogToFile(INFO, Output.c_str());
        if (CustomFile.find(".cpp") == EOF && AutoCurrectSyntax){
            Oxygen::PrintColor("red");
            std::cout << "输入的自定义文件后缀名错误，自动更正为.cpp!" << std::endl;
            Oxygen::PrintColor("white");
            Currect(CustomFile, "Back", ".cpp");
            CustomFile += "\"";
            Output = "检测到输入自定义文件后缀错误! 已自动更正为: " + CustomFile;
            WriteLogToFile(WARN, Output.c_str());
        }
        if (CustomFile == "\"Oxygen.cpp\""){
            SelfUse = true;
            WriteLogToFile(INFO, "Oxygen.cpp detected");
        }
    }
    if (std::to_string(parser.get<int>("optimize")) != "0"){
        OptimizeLevel = parser.get<int>("optimize");
        Output = "优化等级: " + std::to_string(OptimizeLevel);
        WriteLogToFile(INFO, Output.c_str());
        if (Debug)
            std::cout << "优化等级: " << parser.get<int>("optimize") << std::endl;
    }
    std::string CustomParam = "\0";
    if (!parser.get<std::string>("customparam").empty()){
        std::ifstream CustomizedParamFile("CustomizedParam.txt");
        std::getline(CustomizedParamFile, CustomParam);
        if (CustomParam.empty())
            CustomParam = parser.get<std::string>("customparam");
        Output = "自定义参数: " + CustomParam;
        WriteLogToFile(INFO, Output.c_str());
        CustomizedParamFile.close();
        if (Debug)
            std::cout << "自定义参数: " << CustomParam << std::endl;
    }
    if (!parser.get<std::string>("link").empty()){
        LinkFile = parser.get<std::string>("link");
        Output = "链接文件: " + LinkFile;
        WriteLogToFile(INFO, Output.c_str());
        if (Debug)
            std::cout << "链接文件: " << LinkFile << std::endl;
    }
    GUI = parser.exist("GUI");
    // EasyX = parser.exist("EasyX");

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
    // 不基础的 (不大量使用三目 不然不好维护)
    if (OptimizeLevel != 0){
        FinalCommandLine += " -O" + std::to_string(OptimizeLevel);
    }
    if (CheckSyntax){
        FinalCommandLine += " -fsyntax-only";
        Output = "只进行语法检查!";
        WriteLogToFile(INFO, Output.c_str());
        if (Debug)
            std::cout << "只进行语法检查已开启" << std::endl;
    }
    if (DisplayAllWarnings){
        FinalCommandLine += " -Wall";
        Output = "显示所有警告!";
        WriteLogToFile(INFO, Output.c_str());
        if (Debug)
            std::cout << "显示所有警告已开启" << std::endl;
    }
    if (GUI){
        FinalCommandLine += " -mwindows";
        Output = "将程序把编译为 GUI 程序!";
        WriteLogToFile(INFO, Output.c_str());
        if (Debug)
            std::cout << "GUI模式已开启" << std::endl;
    }

    if (SelfUse){
        FinalCommandLine += " -lwinmm -lshlwapi -lole32 -lgdi32 -luuid -lcomdlg32 -lpsapi -lntdll";
    }

    if (!CustomParam.empty()){
        FinalCommandLine += " " + CustomParam;
    }

    if (!LinkFile.empty()){
        FinalCommandLine += " -l" + LinkFile;
        Output = "链接文件: " + LinkFile;
        WriteLogToFile(INFO, Output.c_str());
        if (Debug)
            std::cout << "链接文件: " << LinkFile << std::endl;
    }
    std::cout << "Final Compilation Command Line: " << FinalCommandLine << std::endl;
    Output = "编译命令: " + FinalCommandLine;
    WriteLogToFile(INFO, Output.c_str());
    
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
        
        Output = "Compile Time: " + std::to_string((EndTime - StartTime) / 1000.0) + "s";
        WriteLogToFile(INFO, Output.c_str());

        if (!MediaPath.empty()){
            Output = "Media Path: " + MediaPath;
            WriteLogToFile(INFO, Output.c_str());
            std::cout << "Media Path: " << MediaPath << std::endl;
            auto result = RunSoundA();
            if (result.first){
                Output = "Sound Played Successfully!";
                WriteLogToFile(INFO, Output.c_str());
                std::cout << "Sound Played Successfully!" << std::endl;
            }
            else{
                Output = "Sound Played Failed! Error Code: " + std::to_string(result.second);
                WriteLogToFile(ERROR, Output.c_str());
                std::cout << "Sound Played Failed! Error Code: " << result.second << std::endl;
            }
        }
    }

    Output = "--------------- Stoped! ---------------";
    WriteLogToFile(INFO, Output.c_str());

    OutputLog.close();

    return 0;
}
/*
                   _ooOoo_
                  o8888888o
                  88" . "88
                  (| -_- |)
                  O\  =  /O
               ____/`---'\____
            .'  \\|     |//  `.
            /  \\|||  :  |||//  \
           /  _||||| -:- |||||-  \
           |   | \\\  -  /// |   |
           | \_|  ''\---/''  |   |
           \  .-\__  `-`  ___/-. /
         ___`. .'  /--.--\  `. . __
      ."" '<  `.___\_<|>_/___.'  >'"".
     | | :  `- \`.;`\ _ /`;.`/ - ` : | |
     \  \ `-.   \_ __\ /__ _/   .-` /  /
======`-.____`-.___\_____/___.-`____.-'======
                   `=---='
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    佛祖保佑       永不宕机     永无BUG
*/

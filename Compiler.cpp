#include "Oxygen.h"
#include "cmdline.h"
#include <fstream>
#include <cctype>
#include <regex>
#include <chrono>
#include <format>
#include <variant>
#include <unordered_map>
#include <optional>

using Oxygen::OutputTime;

const std::string DefaultStd = "17";
static std::string Standard = "-1";

std::ofstream OutputLog("Output-cmdline.log", std::ios::out | std::ios::app);
std::ifstream ReadOptions;

std::string CompilerPath, WindresPath, ResourceFile, WindresLine;
std::string FinalCommandLine;
std::string MediaPath;
std::string AdditionalOption;
std::string Output;

bool RunSound = false;
bool AutoCurrectSyntax = true;
bool SelfUse = false; // 用于给 Oxygen.cpp 添加一长串参数
bool CheckSyntax = false;
bool DisplayAllWarnings = false;
bool Debug = true;
bool GUI = false;
// bool EasyX = false;

std::unordered_map<std::string, std::optional<std::string>> configMap;

enum class LogLevel{
    Info = 1,
    Warn = 2,
    Error = 3,
    Fatal = 4
};

void WriteLogToFile(LogLevel Level, const char* message);

std::string BooleanOutput(bool b1){
    return b1 ? "true" : "false";
}

void OutputCurrentTime(){
    OutputLog << "[", OutputTime(OutputLog), OutputLog << "] ";
}

std::string ToLowercase(const std::string& str){
    std::string result = str;
    for (char& c : result){
        c = std::tolower(c);
    }
    return result;
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

static const char* GetErrorInfo(DWORD ErrorCode){
    if (ErrorCode > 32) {
        return "No Error here.";
    }
    switch (ErrorCode) {
    case 0: {
        return "The operating system is out of memory or resources.";
    }
    case ERROR_FILE_NOT_FOUND: {
        return "The specified file was not found.";
    }
    case ERROR_PATH_NOT_FOUND: {
        return "The specified path was not found.";
    }
    case ERROR_BAD_FORMAT: {
        return "The .exe file is invalid (non-Win32 .exe or error in .exe image).";
    }
    case SE_ERR_ACCESSDENIED: {
        return "The operating system denied access to the specified file.";
    }
    case SE_ERR_ASSOCINCOMPLETE: {
        return "The file name association is incomplete or invalid.";
    }
    case SE_ERR_DDEBUSY: {
        return "The DDE transaction could not be completed because other DDE transactions were being processed.";
    }
    case SE_ERR_DDEFAIL: {
        return "The DDE transaction failed.";
    }
    case SE_ERR_DDETIMEOUT: {
        return "The DDE transaction could not be completed because the request timed out.";
    }
    case SE_ERR_DLLNOTFOUND: {
        return "The specified DLL was not found.";
    }
    case SE_ERR_NOASSOC: {
        return "There is no application associated with the given file name extension. This error will also be returned if you attempt to print a file that is not printable.";
    }
    case SE_ERR_OOM: {
        return "There was not enough memory to complete the operation.";
    }
    case SE_ERR_SHARE: {
        return "A sharing violation occurred.";
    }
    default: {
        return "Unknown Error.";
    }
    }
    return "Unknown Error.";
}

std::variant<std::string, bool> RunSoundV(){
    // 播放的音频文件路径
    const char* audioFilePath = MediaPath.c_str();

    // 打开并播放音频文件
    if (PlaySoundA(audioFilePath, NULL, SND_FILENAME | SND_ASYNC)){
        Sleep(1.5 * 1000);

        // 停止音频播放
        PlaySoundA(NULL, NULL, 0);

        return true;
    }
    DWORD ErrorCode = GetLastError();
    return GetErrorInfo(ErrorCode);
}

void ReadOptionsFile(){
    std::string WholeCommand, Command;

    if (!ReadOptions.is_open()){
        CompilerPath = "g++.exe ";
        WindresPath = "Windres.exe ";
        return;
    }   

    while (std::getline(ReadOptions, WholeCommand)){
        // 先去除注释 '#'
        std::regex commentRegex("#.*");
        WholeCommand = std::regex_replace(WholeCommand, commentRegex, "");

        // 在进行正则匹配
        std::regex pattern(R"((\w+)\s*=\s*(.+))");
    
        std::smatch matches;
        if (std::regex_match(WholeCommand, matches, pattern)) {
            Command = matches[1].str();
            std::string value = matches[2].str();
            configMap.insert( std::make_pair( ToLowercase( matches[1].str() ), matches[2].str() ) );
        }
    }

    CompilerPath = configMap["compilerpath"].value_or("g++.exe ");
    WindresPath = configMap["windrespath"].value_or("Windres.exe ");
    RunSound = ToLowercase(configMap["runsound"].value_or("false")) == "true";
    AutoCurrectSyntax = ToLowercase(configMap["autocurrectsyntax"].value_or("true")) == "true";
    MediaPath = configMap["mediapath"].value_or("C:\\Windows\\Media\\Windows Notify Messaging.wav");
    Debug = ToLowercase(configMap["debug"].value_or("false")) == "true";
    AdditionalOption = configMap["additionaloption"].value_or("");
    // 对于数字需要额外的异常捕获 (std::stoi)
    try{
        Standard = configMap["standard"].value_or("17");
    }
    catch (const std::exception& e){
        Output = std::format("Error Occurred when parsing standard number in options.txt: {}. Using default: C++{}", e.what(), DefaultStd);
        WriteLogToFile(LogLevel::Warn, Output.c_str());
        Standard = "17";
    }
    if (Debug){
        std::cout << "--------- Raw Input ---------" << std::endl;
        for (const auto& [key, value] : configMap){
            std::cout << std::format("{}: |{}|", key, value.value_or("null")) << std::endl;
        }
        std::cout << "--------- Raw Input ---------" << std::endl;
        std::cout << "Compiler Path: " << CompilerPath << std::endl;
        std::cout << "Windres Path: " << WindresPath << std::endl;
        std::cout << "Media Path: " << MediaPath << std::endl;
        std::cout << "Standard: " << Standard << std::endl;
        std::cout << "Default Standard: " << DefaultStd << std::endl;
        std::cout << "RunSound: " << BooleanOutput(RunSound) << std::endl;
        std::cout << "AutoCurrectSyntax: " << BooleanOutput(AutoCurrectSyntax) << std::endl;
        std::cout << "Additional Compile Options: " << AdditionalOption << ", IsEmpty: " << AdditionalOption.empty() << std::endl;
    }
    Output = "Compiler Path: " + CompilerPath;
    WriteLogToFile(LogLevel::Info, Output.c_str());
    Output = "Windres Path: " + WindresPath;
    WriteLogToFile(LogLevel::Info, Output.c_str());
    Output = "Run Sound: " + BooleanOutput(RunSound);
    WriteLogToFile(LogLevel::Info, Output.c_str());
    Output = "Auto Currect Syntax: " + BooleanOutput(AutoCurrectSyntax);
    WriteLogToFile(LogLevel::Info, Output.c_str());
    if (!AdditionalOption.empty()){
        Output = "Additional Compile Options: " + AdditionalOption;
        WriteLogToFile(LogLevel::Info, Output.c_str());
    }
}
 
void Currect(std::string& error, const char* request, const char* Wanted){
    if (request == std::string("Back")){
        int position = error.rfind('.');
        std::string back = error.substr(position);
        if (back != Wanted){
            back = Wanted;
        }
        error = error.substr(0, position) + back;
    }
    else if (request == std::string("Suffix")){
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

void WriteLogToFile(LogLevel Level, const char* message){
    OutputLog << "[", OutputTime(OutputLog), OutputLog << "] ";
    std::string info = "[main/";
    switch (Level){
        case LogLevel::Info:
            info += "INFO]";
            break;
        case LogLevel::Warn:
            info += "WARN]";
            break;
        case LogLevel::Error:
            info += "ERROR]";
            break;
        case LogLevel::Fatal:
            info += "FATAL]";
            break;
    }
    OutputLog << info << " " << message << "\n";
}

const std::string Version = "1.2.1.3";

int main(int argc, char *argv[]){
    WriteLogToFile(LogLevel::Info, "--------------- Launched ---------------");
    std::string VersionInfo = "[Compiler] Version " + Version;
    WriteLogToFile(LogLevel::Info, VersionInfo.c_str());
    std::cout << VersionInfo << std::endl;

    std::string Location = Oxygen::File::GetLocation() + "\\options.txt";
    ReadOptions.open(Location.c_str()); // 打开当前目录下的options.txt文件
    ReadOptionsFile();

    if (argc == 2 && ToLowercase(std::string(argv[1])) == "--config"){
        std::cout << "打开配置文件: " << Location << std::endl;
        Output = "打开配置文件: " + Location;
        WriteLogToFile(LogLevel::Info, Output.c_str());
        HINSTANCE result = ShellExecuteA(NULL, "open", "notepad.exe", Location.c_str(), NULL, SW_SHOWNORMAL);
        INT_PTR resultValue = reinterpret_cast<INT_PTR>(result);
        if (resultValue <= 32) {
            std::cout << "无法打开配置文件: " << Location << std::endl;
            Output = "无法打开配置文件: " + Location;
            WriteLogToFile(LogLevel::Error, Output.c_str());
            Output = "错误信息: " + std::string(GetErrorInfo(resultValue));
            WriteLogToFile(LogLevel::Error, Output.c_str());
        }
        else{
            std::cout << "已成功打开配置文件: " << Location << std::endl;
            Output = "已成功打开配置文件: " + Location;
            WriteLogToFile(LogLevel::Info, Output.c_str());
        }
        
        WriteLogToFile(LogLevel::Info, "--------------- Stoped! ---------------");
        OutputLog.close();
        return 0;
    }

    // 解析命令行参数
    cmdline::parser parser;
    parser.add<std::string>("file", 'f', "输入文件", true);
    parser.add<std::string>("standard", 's', "C++编译标准(仅输入数字)", false,( (Standard == "-1") ? DefaultStd : Standard), cmdline::oneof<std::string>("11", "14", "17", "20", "23"));
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
    // OpenConfigFile = parser.exist("config");
    // std::cout << "Debug: " << Debug << std::endl;

    if (!ok || parser.get<bool>("help")) {
        // 解析失败或请求帮助时，输出帮助信息
        std::cout << parser.error_full() << std::endl;
        std::cout << parser.usage();
        std::cout << std::format("      --config         打开 {} 的配置文件", std::string(argv[0])) << std::endl << std::endl;

        ConsoleColor::SetColor(ConsoleColor::Red, ConsoleColor::Black);
        std::cout << "注意: 由于程序使用的命令行解析库限制, 在输入文件时不要带空格" << std::endl;
        std::cout << "Careful: Due to the limitation of the command line parsing library used by the program, do not include spaces when inputting the file" << std::endl;
        ConsoleColor::SetColor(ConsoleColor::White, ConsoleColor::Black);

        return 1;
    }

    // 因为函数很难识别空格 那么一律添加引号
    std::string InputFile = "\"" + parser.get<std::string>("file") + "\"";
    std::string Standard = "c++" + parser.get<std::string>("standard");
    std::string LinkFile = "";
    std::string Resource = "\0", CustomFile = "\0", ResourceName = "\0";

    // std::string_view InputFileView, StandardView, LinkFileView, ResourceView, CustomFileView, ResourceNameView;

    Output = "输入文件: " + InputFile + ", 标准: " + Standard;
    WriteLogToFile(LogLevel::Info, Output.c_str());
    if (Debug){
        std::cout << "输入文件: " << InputFile << std::endl;
        std::cout << "标准: C++" << parser.get<std::string>("standard") << std::endl;
    }
    DeletePrefix(InputFile);
    // 纠正后缀
    if (static_cast<int>(InputFile.find('.')) == EOF && AutoCurrectSyntax){
        // 太过分了 连后缀都没有
        ConsoleColor::SetColor(ConsoleColor::Red);
        std::cout << "未检测到文件后缀! 自动添加后缀.cpp!" << std::endl;
        ConsoleColor::SetColor(ConsoleColor::White);
        // 因为默认加引号了 所以要去掉
        InputFile = InputFile.substr(0, InputFile.length() - 1);
        InputFile += ".cpp\"";
        Output = "未检测到文件后缀! 已自动更正为: " + InputFile;
        WriteLogToFile(LogLevel::Warn, Output.c_str());
    }
    else if (static_cast<int>(InputFile.find(".cpp")) == EOF && AutoCurrectSyntax){
        ConsoleColor::SetColor(ConsoleColor::Red);
        std::cout << "输入文件后缀名错误，自动更正为.cpp!" << std::endl;
        ConsoleColor::SetColor(ConsoleColor::White);
        Currect(InputFile, "Back", ".cpp");
        InputFile += "\"";
        Output = "检测到输入的源代码文件后缀错误! 已自动更正为: " + InputFile;
        WriteLogToFile(LogLevel::Warn, Output.c_str());
    }

    int OptimizeLevel = 0;
    if (parser.get<std::string>("resource") != "\0"){
        Resource = "\"" + parser.get<std::string>("resource") + "\"";
        Output = "资源文件: " + Resource;
        WriteLogToFile(LogLevel::Info, Output.c_str());

        if (!Resource.empty()){
            if (Debug)
                std::cout << "资源文件: " << Resource << std::endl;
            // 先进行修正
            DeletePrefix(Resource);
            if (static_cast<int>(Resource.find(".rc")) == EOF && AutoCurrectSyntax){
                ConsoleColor::SetColor(ConsoleColor::Red);
                std::cout << "资源文件后缀名错误，自动更正为.rc!" << std::endl;
                ConsoleColor::SetColor(ConsoleColor::White);
                Currect(Resource, "Back", ".rc");
                Resource += "\"";
                Output = "检测到输入的资源文件后缀错误! 已自动更正为: " + InputFile;
                WriteLogToFile(LogLevel::Warn, Output.c_str());
            }
            ResourceName = Resource.substr(0, Resource.find("."));
            WindresLine = WindresPath + " " + Resource + " -O coff " + ResourceName + ".o\" 2> WindresLog.txt";
            std::cout << "WindresLine: " << WindresLine << std::endl;
            if (!Debug){
                if (!std::system(WindresLine.c_str())){
                    Output = "资源文件编译成功!";
                    WriteLogToFile(LogLevel::Info, Output.c_str());
                }
                else{
                    Output = "资源文件编译失败!";
                    WriteLogToFile(LogLevel::Error, Output.c_str());
                }   
            }
            if (Debug){
                Output = "Windres Compilation Command Line: " + WindresLine;
                std::cout << Output << std::endl;
            }
            Output = "Windres Compilation Command Line: " + WindresLine;
            WriteLogToFile(LogLevel::Info, Output.c_str());
        }
    }
    if (parser.get<std::string>("customfile") != "\0"){
        CustomFile = "\"" + parser.get<std::string>("customfile") + "\"";
        if (Debug)
            std::cout << "自定义文件: " << parser.get<std::string>("customfile") << std::endl;
        Output = "自定义文件: " + CustomFile;
        WriteLogToFile(LogLevel::Info, Output.c_str());
        if (static_cast<int>(CustomFile.find(".cpp")) == EOF && AutoCurrectSyntax){
            ConsoleColor::SetColor(ConsoleColor::Red);
            std::cout << "输入的自定义文件后缀名错误，自动更正为.cpp!" << std::endl;
            ConsoleColor::SetColor(ConsoleColor::White);
            Currect(CustomFile, "Back", ".cpp");
            CustomFile += "\"";
            Output = "检测到输入自定义文件后缀错误! 已自动更正为: " + CustomFile;
            WriteLogToFile(LogLevel::Warn, Output.c_str());
        }
        if (CustomFile == "\"Oxygen.cpp\""){
            SelfUse = true;
            WriteLogToFile(LogLevel::Info, "Oxygen.cpp detected");
        }
    }
    if (std::to_string(parser.get<int>("optimize")) != "0"){
        OptimizeLevel = parser.get<int>("optimize");
        Output = "优化等级: " + std::to_string(OptimizeLevel);
        WriteLogToFile(LogLevel::Info, Output.c_str());
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
        WriteLogToFile(LogLevel::Info, Output.c_str());
        CustomizedParamFile.close();
        if (Debug)
            std::cout << "自定义参数: " << CustomParam << std::endl;
    }
    if (!parser.get<std::string>("link").empty()){
        LinkFile = parser.get<std::string>("link");
        Output = "链接文件: " + LinkFile;
        WriteLogToFile(LogLevel::Info, Output.c_str());
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
        WriteLogToFile(LogLevel::Info, Output.c_str());
        if (Debug)
            std::cout << "只进行语法检查已开启" << std::endl;
    }
    if (DisplayAllWarnings){
        FinalCommandLine += " -Wall";
        Output = "显示所有警告!";
        WriteLogToFile(LogLevel::Info, Output.c_str());
        if (Debug)
            std::cout << "显示所有警告已开启" << std::endl;
    }
    if (GUI){
        FinalCommandLine += " -mwindows";
        Output = "将程序把编译为 GUI 程序!";
        WriteLogToFile(LogLevel::Info, Output.c_str());
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
        WriteLogToFile(LogLevel::Info, Output.c_str());
        if (Debug)
            std::cout << "链接文件: " << LinkFile << std::endl;
    }

    if (!AdditionalOption.empty()){
        FinalCommandLine += " " + AdditionalOption;
        Output = "附加编译选项: " + AdditionalOption;
        WriteLogToFile(LogLevel::Info, Output.c_str());
        if (Debug)
            std::cout << "附加编译选项: " << AdditionalOption << std::endl;
    }

    std::cout << "Final Compilation Command Line: " << FinalCommandLine << std::endl;
    Output = "编译命令: " + FinalCommandLine;
    WriteLogToFile(LogLevel::Info, Output.c_str());

    if (!Debug){
        std::chrono::steady_clock::time_point StartTime = std::chrono::steady_clock::now();
        std::cout << "Compiling..." << std::endl;
        ConsoleColor::SetColor(ConsoleColor::LightRed, ConsoleColor::Black);
        int CompileResult = std::system(FinalCommandLine.c_str());
        ConsoleColor::SetColor(ConsoleColor::White, ConsoleColor::Black);
        if (!CompileResult){
            std::cout << "Compile Successfully!" << std::endl;
        }
        else{
            std::cout << "Compile Failed!" << std::endl;
        }   
        std::chrono::steady_clock::time_point EndTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> Duration = EndTime - StartTime; // 单位: s
        std::cout << std::format("Compile Time: {:.4f}s", Duration.count()) << std::endl;
        Output = std::format("Compile Time: {:.4f}s", Duration.count());
        WriteLogToFile(LogLevel::Info, Output.c_str());

        if (!MediaPath.empty()){
            Output = "Media Path: " + MediaPath;
            WriteLogToFile(LogLevel::Info, Output.c_str());
            std::cout << "Media Path: " << MediaPath << std::endl;
            // auto result = RunSoundA();
            // if (result.first){
            //     Output = "Sound Played Successfully!";
            //     WriteLogToFile(LogLevel::Info, Output.c_str());
            //     std::cout << "Sound Played Successfully!" << std::endl;
            // }
            // else{
            //     Output = "Sound Played Failed! Error Code: " + std::to_string(result.second);
            //     WriteLogToFile(LogLevel::Error, Output.c_str());
            //     std::cout << "Sound Played Failed! Error Code: " << result.second << std::endl;
            // }
            std::variant<std::string, bool> result = RunSoundV();
            if (result.index() == 1){
                Output = "Sound Played Successfully!";
                WriteLogToFile(LogLevel::Info, Output.c_str());
                std::cout << "Sound Played Successfully!" << std::endl;
            }
            else{ // index = 0, std::string -> Error
                Output = "Sound Played Failed! Error Info: " + std::get<std::string>(result);
                WriteLogToFile(LogLevel::Error, Output.c_str());
                std::cout << "Sound Played Failed! Error Info: " << std::get<std::string>(result) << std::endl;
            }
        }
    }

    Output = "--------------- Stoped! ---------------";
    WriteLogToFile(LogLevel::Info, Output.c_str());

    OutputLog.close();
    ReadOptions.close();

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

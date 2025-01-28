#include <iostream>
#include <unistd.h>
#include <fstream>
#include <ostream>
#include <cstring>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <vector>

#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib");

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib");

#include "Oxygen.h"

//std::string CompileLine = "g++ ";
std::string CompileLine, Content;
int option;
std::string CompileFile, WindresPath, RCFile, CompilerPath, path;
std::string CppStandard = " -std=c++11";
std::string Optimize, temp, OtherHeaderFile;
std::string IfActiveEasyx, IfHideConsoleWindow;
bool ActiveWindres = false;
// bool ActiveSg = false; // 是否开启 -sg (-lsfml-graphics) 参数
// bool ActiveSs = false; // 是否开启 -ss (-lsfml-system) 参数
// bool ActiveSw = false; // 是否开启 -sw (-lsfml-window) 参数
// bool ActiveSa = false; // 是否开启 -sa (-lsfml-audio) 参数
// bool ActiveSn = false; // 是否开启 -sn (-lsfml-network) 参数
bool AddedStd = false;

// bool DirectlyExit = false; // 未输入文件 直接退出
bool ActiveS = false;
bool IfPlaySound = false;
bool NeedHelp = false;
bool AutoCurrectSyntax = false;

std::ofstream ofs("log.txt", std::ios::app);

std::string WindresLog;
std::string NeedHandle, ErrorFile, ErrorPosition, ErrorLog;

using Oxygen::OutputTime;

std::string GetOptionsFilePath();

void PlaySound(){
    // 播放的音频文件路径
    // const char* audioFilePath = "C:\\Windows\\Media\\Windows Notify Messaging.wav";
	const char* audioFilePath = "D:\\Code\\Windows Notify Messaging.wav";

    // 打开并播放音频文件
    PlaySoundA(audioFilePath, NULL, SND_FILENAME | SND_ASYNC);

    Sleep(1.5 * 1000);

    // 停止音频播放
    PlaySoundA(NULL, NULL, 0);
}

std::string GetPath(){
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    PathRemoveFileSpec(buffer);
    std::string CurrentDir(buffer);

    return CurrentDir;
}

std::string GetOptionsFilePath() {
    // 获取当前程序所在目录的绝对路径
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    PathRemoveFileSpec(buffer);
    std::string CurrentDir(buffer);

    // 拼接options.txt文件的路径
    std::string OptionsFilePath = CurrentDir + "\\options.txt";
    return OptionsFilePath;
}

void PrintColor(std::string type){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (type == "white")
        SetConsoleTextAttribute(hOut, 7);
    else if (type == "red")
        SetConsoleTextAttribute(hOut, 12);
}

void Error(){
    PrintColor("red");
    std::cerr << "Syntax Error!" << std::endl;
    PrintColor("white");
}

void GetData(std::ifstream& ifs){
    std::string WholeCommand, Command;

    if (!ifs.is_open()){
        CompilerPath = "g++.exe ";
        WindresPath = "Windres.exe ";
        return;
    }

    while (std::getline(ifs, WholeCommand)){
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
        else if (Command == "PlaySound"){
            std::string temp = WholeCommand.substr(div + 1);
            if (temp == "true" || temp == "TRUE" || temp == "True"){
                IfPlaySound = true;
            }
            else if (temp == "false" || temp == "FALSE" || temp == "False")
                IfPlaySound = false;
        }
        else if (Command == "AutoCurrectSyntax"){  // 自动纠错
            std::string temp = WholeCommand.substr(div + 1);
            if (temp == "true" || temp == "TRUE" || temp == "True"){
                AutoCurrectSyntax = true;
            }
            else if (temp == "false" || temp == "FALSE" || temp == "False")
                AutoCurrectSyntax = false;
        }
    }
}

void GetWindresError(std::string input){
    int store = 0; //  v.储存

    int Position = input.rfind(".exe");
    // 摘取错误日志
    NeedHandle = input.substr(Position + 6);
    Position = NeedHandle.find(":");
    NeedHandle[Position] = ' ';

    ErrorFile = NeedHandle.substr(0, Position);
    std::cout << "   Error File: " << ErrorFile << std::endl;

    store = Position + 1;
    Position = NeedHandle.find(":");
    for (int i = store; i < Position; i ++ ){
        ErrorPosition += NeedHandle[i];
    }
    std::cout << "   Error Position: Line " << ErrorPosition << std::endl;

    Position += 2; // 移到报错的第一个字符
    ErrorLog += NeedHandle.substr(Position);
    std::cout << "   Problem: " << ErrorLog << std::endl;
    ofs << "\n[", OutputTime(ofs), ofs << "] "; // 这里补了一个换行
    ofs << "'.rc' 文件出现错误, 错误文件: " << ErrorFile << ", 错误出现的行数: " << ErrorPosition << ", 具体的问题: " << ErrorLog; 
}

void UpdatedGetWindresError(std::string Error){
    ofs << "\n[", OutputTime(ofs), ofs << "] ";
    int pos = Error.find(".exe");
    // 把 **\\Windres.exe 删掉
    // std::cout << Error.substr(0, pos + 4) << std::endl;
    // 顺手把": "去了
    std::string NeedHandler = Error.substr(pos + 4 + 2, Error.size());
    int signal = NeedHandler.rfind(":");
    bool WhetherDigit = isdigit(NeedHandler[signal - 1]);
    // std::cout << "NeedHandler: " << NeedHandler << std::endl;
    // 有两种报错 带数字和不带的 需要分别处理
    if (WhetherDigit){ // 带数字
        std::string TrueError, ApartNumber, ErrorFile;
        int di = NeedHandler.rfind(":");
        // 多去个空格
        TrueError = NeedHandler.substr(di + 1 + 1); 
        ApartNumber = NeedHandler.substr(0, di);
        // std::cout << "TrueError: " << TrueError << std::endl;
        // std::cout << "ApartNumber: " << ApartNumber << std::endl;
        int li = ApartNumber.find(":");
        ErrorFile = ApartNumber.substr(0, li);
        std::string line = ApartNumber.substr(li + 1, ApartNumber.size());
        std::cout << "      ErrorFile: " << ErrorFile << std::endl;
        std::cout << "      Error: " << TrueError << std::endl;
        std::cout << "      Line: " << line << std::endl;
        ofs << "'.rc' 文件出现错误, 错误文件: " << ErrorFile << ", 错误出现的行数: " << line << ", 具体的问题: " << TrueError; 
    }
    else{ // 不带数字
        std::cerr << "      Error: " <<  NeedHandler << std::endl;
        ofs << "'.rc' 文件出现问题, 具体问题: " << NeedHandler;
    }
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

void NoOptions(){
    ofs << "[", OutputTime(ofs), ofs << "] ";
    ofs << "未输入文件, 程序直接退出! " << std::endl;
    std::cout << "Empty File Path, COMPILER will exit directly!" << std::endl;
    ofs << "[", OutputTime(ofs), ofs << "] ";
    ofs << "--------------- Stoped! ---------------" << std::endl;
}

int main(int argc, char* argv[]) {
    if (!ofs.is_open()){
        // std::cerr << "Failed to make log!" << std::endl;
        MessageBoxA(NULL, "Failed to make log!", "Compiler", MB_ICONINFORMATION);
    }
	ofs << "[", OutputTime(ofs), ofs << "] --------------- Launched ---------------" << std::endl;
    ofs << "[", OutputTime(ofs), ofs << "] [Compiler] Version 1.1.4.12\n";
	ofs << "[", OutputTime(ofs), ofs << "] 程序启动! 编译开始~" << std::endl;
    std::string OptionsPath = GetOptionsFilePath();
    std::string LogLocation = Oxygen::GetLocation() + "\\WindresLog.txt";
    std::ifstream ReadWindresLog(LogLocation);
    std::ifstream ifs(OptionsPath);
    std::cout << "[Compiler] Version 1.1.4.12\nAuthor: _Oxygen_ with Luogu UID 965233" << std::endl;
    GetData(ifs);
    if (CompilerPath.empty()){
        // std::cerr << "Wdf is that, bro~~~~ You forgettttt to configure the COMPILER TOOL!!!" << std::endl;
        MessageBoxA(NULL, "You forget to configure the COMPILER \"g++\" PATH!", "Compiler", MB_ICONSTOP);
        ofs << "["; OutputTime(ofs); ofs << "] --------------- Stoped! ---------------";
        return 0;
    }
    CompileLine += " ";

    std::string Path = GetPath() + "\\Compiler.exe";
    // std::cout << "Path: " << Path << std::endl;
    
    // if (argc == 2 && (strcmp(argv[1], "Compiler.exe") == 0 || strcmp(argv[1], Path.c_str()) == 0 || strcmp(argv[1], "compiler.exe") == 0)) {
    //     NoOptions();
    //     return EXIT_FAILURE;
    // }
    if (argc == 1 && strcmp(argv[0], Path.c_str()) == 0){
        NoOptions();
        return 0; // 直接结束程序
    }

    if (argc == 2) {
        // 复制 argv[0] 到一个新的字符串，避免修改原始指针
        char lower_argv0[100];  // 假设最大长度为 100
        strcpy(lower_argv0, argv[0]);

        // 将 lower_argv0 中的所有字符转换为小写
        for (int i = 0; lower_argv0[i] != '\0'; i++) {
            lower_argv0[i] = tolower((unsigned char)lower_argv0[i]);
        }

        // 检查转换后的字符串是否等于 "compiler.exe"
        if (strcmp(lower_argv0, "compiler.exe") == 0) {
            NoOptions();
            return EXIT_FAILURE;
        }
    }

    std::vector<std::string> args(argv, argv + argc);
    for (auto arg : args)
        if (arg == "-help"){
            NeedHelp = true;
            break;
        }

    if (NeedHelp){
        std::cout << "这个程序支持的参数有: -f -s -o -i -w -n -l -h -c -a -r -e -d -u -m" << std::endl;
        std::cout << "其中 -f 需要加cpp源文件, -s 需要加所需要的c++标准(只需要输入数字), -o 需要添加优化, 如o2 o3, 同样只需要键入数字\n";
        std::cout << "-i 需要加自己自定义的.cpp文件, 并请你提前配置好, 确定可以使用; -r 需要加需要的 .rc 文件, 它会调用windres编译 .rc 文件然后再编译源代码.\n";
        std::cout << "那么剩下的都不需要参数, -w = -Wall, -l = -leasyx, -n = -lwinmm, -a = -lshlwapi, -h = -mwindows, -c = -fsyntax-only -e = -lole32 -d = -lgdi32 -u = -luuid -m = -lcomdlg32\n";
        std::cout << "其中除 -c 外都是编译参数, -c 为只检测语法错误\n";
        std::cout << "需要注意的是, 如果你要使用 -l , 请提前配置好 EasyX 环境\n";
        ofs << "["; OutputTime(ofs); ofs << "] 使用了 '-help' 参数获取帮助" << std::endl;
        ofs << "["; OutputTime(ofs); ofs << "] --------------- Stoped! ---------------" << std::endl;
        return 0;
    }

    // 循环解析命令行参数
    // 写一个事啊 调了好久 就是这个getopt函数中 字母后面如果有:的话 就是有参数 没有冒号就是没参数
    while ((option = getopt(argc, argv, "f:s:o:lhcr:nai:wedum")) != -1) {
        switch (option) {
            case 'f':{
                // 将输入的文件名 拼成编译脚本
                CompileFile = optarg;
                if (CompileFile.find(".\\") != EOF){
                    // 有'.\' 
                    CompileFile = CompileFile.substr(2);
                }
                if (CompileFile.find(".cpp") == EOF){
                    if (AutoCurrectSyntax){
                        PrintColor("red");
                        std::cout << "Decect Error Suffix on Input File! Currect it already. :)" << std::endl;
                        PrintColor("white");
                        ofs << "[", OutputTime(ofs), ofs << "] ";
                        ofs << "用户输入的源文件为 " << CompileFile;
                        Currect(CompileFile, "Back", ".cpp");
                        ofs << " 已经自动纠正为 " << CompileFile << "! awa" << std::endl;
                    }
                    else{
                        Error();
                        return 0;
                    }
                }
                CompileLine += CompileFile;
                temp = CompileFile.substr(0, CompileFile.rfind('.'));
                CompileLine = CompilerPath + CompileLine + " -o " + temp + ".exe";
                ofs << "[", OutputTime(ofs), ofs << "] ";
				ofs << "编译文件: " << CompileFile;
                break;
            }
            case 's':{
                ActiveS = true;
                // if (!ActiveSg && !ActiveSw && !ActiveSs && !ActiveSa && !ActiveSn) // 没有一个参数与 SFML 有关
                    CppStandard = optarg;
                // else 
                //     CppStandard = "17";
                if (!AddedStd){
                    CompileLine = CompileLine + " -std=c++" + CppStandard;
                    AddedStd = true;
                }
                ofs << " C++标准: C++" << CppStandard;
                break;
            }
            case 'o':{
                Optimize = optarg;
                int check = stoi(Optimize);
                if (check >= 4 || check < 0){
                    Error();
                    return 0;
                }
                int pos = CompileLine.find(".cpp");
                CompileLine = CompileLine.substr(0, pos + 1 + 3) + " -o" + Optimize + CompileLine.substr(pos + 4);
                ofs << " 优化: o" << Optimize;
                break;
            }
            case 'i':{
            	// 如果使用了自己的头文件捏 
            	OtherHeaderFile = optarg;
                if (OtherHeaderFile.find(".\\") != EOF){
                    // 有'.\' 
                    OtherHeaderFile = OtherHeaderFile.substr(2);
                }
                int pos = OtherHeaderFile.find(".cpp");
                if (pos == EOF){
                    if (AutoCurrectSyntax){
                        PrintColor("red");
                        std::cout << "Decect Error Suffix on DIY Source Code! Currect it already. :)" << std::endl;
                        PrintColor("white");
                        ofs << std::endl;
                        ofs << "[", OutputTime(ofs), ofs << "] ";
                        ofs << "用户输入的自定义文件为 " << OtherHeaderFile;
                        Currect(OtherHeaderFile, "Back", ".cpp");
                        ofs << " 已经自动纠正为 " << OtherHeaderFile << "! awa" << std::endl;
                    }
                    else{
                        Error();
                        return 0;
                    }
                }
                int Position = CompileLine.find(".cpp");
                CompileLine = CompileLine.substr(0, Position + 1 + 3) + " " + OtherHeaderFile + CompileLine.substr(Position + 4);
                if (OtherHeaderFile == "Oxygen.cpp"){
                    CompileLine += " -lwinmm -lshlwapi -lole32 -lgdi32 -luuid -lcomdlg32";
                }
                ofs << "[", OutputTime(ofs), ofs << "]";
                ofs << " 使用的自定义cpp文件: " << OtherHeaderFile;
                break;
			}
            case 'w':{
                CompileLine += " -Wall";
                ofs << " 使用了 '-Wall' 参数";
                break;
            }
            case 'n':{
                CompileLine += " -lwinmm";
                ofs << " 使用了 '-lwinmm' 参数";
                break;
            }
            case 'l':{
                CompileLine += " -leasyx";
                ofs << " 使用了 '-leasyx' 参数";
                break;
            }
            case 'h':{
                CompileLine += " -mwindows";
                ofs << " 使用了 '-mwindows' 参数";
                break;
            }
            case 'c':{
                CompileLine += " -fsyntax-only";
                ofs << " 只检测语法错误";
                break;
            }
            case 'a':{
                CompileLine += " -lshlwapi";
                ofs << " 使用了 '-lshlwapi' 参数";
                break;
            }
			case 'e':{
				CompileLine += " -lole32";
				ofs << " 使用了 '-lole32' 参数";
				break;
			}
			case 'd':{
				CompileLine += " -lgdi32";
				ofs << "使用了 '-lgdi32' 参数";
				break;
			}
            case 'u':{
                CompileLine += " -luuid";
				ofs << "使用了 '-luuid' 参数";
				break;
            }
            case 'm':{
                CompileLine += " -lcomdlg32";
                ofs << "使用了 '-lcomdlg32' 参数";
                break;
            }
            case 'r':{
                RCFile = optarg;
                ActiveWindres = true; // 启用Windres
                if (WindresPath.empty() || path.empty()){
                    path = CompilerPath.substr(0, CompilerPath.rfind('\\'));
                    WindresPath = path + "\\Windres.exe";
                    // std::cout << "Windres Path in empty: " << WindresPath << std::endl;
                }
                if (RCFile.find(".\\") != EOF){
                    // 有'.\' 
                    RCFile = RCFile.substr(2);
                }
                if (RCFile.find(".rc") == EOF){
                    if (AutoCurrectSyntax){
                        PrintColor("red");
                        std::cout << "Decect Error Suffix on Resource File! Currect it already. :)" << std::endl;
                        PrintColor("white");
                        ofs << std::endl;
                        ofs << "[", OutputTime(ofs), ofs << "] ";
                        ofs << "用户输入的 .rc 文件为 " << RCFile;
                        Currect(RCFile, "Back", ".rc");
                        ofs << " 已经自动纠正为 " << RCFile << "! awa";
                    }
                    else{
                        Error();
                        return 0;
                    }
                }
                // Not Empty
                RCFile = RCFile.substr(0, RCFile.find('.'));
                // std::cout << "Path: " << path << std::endl;
                // std::cout << "resource file: " << RCFile << std::endl;
                WindresPath = path + "\\windres.exe " + RCFile + ".rc" + " -O coff " + RCFile + ".o";
                ActiveWindres = true;

                // if (ActiveWindres && !WindresPath.empty()){
                //     // std::cout << "Windres Path: " << WindresPath << std::endl;
                //     RCFile += ".rc";
                // }

                int divideplace = CompileLine.find(".cpp");
                CompileLine = CompileLine.substr(0, divideplace + 3 + 1) + " " + RCFile + ".o" + CompileLine.substr(divideplace + 4);
                // std::cout << "Compile Line: " << CompileLine << std::endl;
                ofs << std::endl;
                ofs << "[", OutputTime(ofs), ofs << "]";
                ofs << " 链接的 rc 文件为: " << RCFile << ".rc"; 
                break;
            }
            default:{
                PrintColor("red");
                std::cerr << "Unknown Command!" << std::endl;
                PrintColor("white");
                std::cerr << "Usage: program_name [-f CompileFile] [-s CppStandard] [-o Optimize Level] [-r Resource File] [-w] [-l] [-h] [-c] [-w] [-a] [-e] [-d] [-u] [-m]" << std::endl;
                std::cout << "There are four Examples:" << std::endl << "1.  Compiler.exe -f TestFile.cpp -s 14 -o 2 -l -h -w -a -n -e -d -m" << std::endl
                 << "=   g++ TestFile.cpp -o2 -o TestFile.exe -std=c++14 -leasyx -mwindows -Wall -lshlwapi -lwinmm -lole32 -lgdi32 -lcomdlg32" << std::endl;
                std::cout << "2.  Compiler.exe -f TestFile.cpp -c" << std::endl
                 << "=   g++ TestFile.cpp -fsyntax-only" << std::endl;
                std::cout << "3.  Compiler.exe -f TestFile.cpp -r resource.rc" << std::endl
                 << "=   windres resource.rc -O coff resource.o && g++ TestFile.cpp resource.o -o TestFile.exe" << std::endl;
                std::cout << "4.  Compiler.exe -f TestFile.cpp -i Oxygen.cpp" << std::endl;
				std::cout << "Be Careful: 1. If you want to use '-l', it's necessary to configure its Environment first."<< 
                "\n            2. If you use '-r', don't forget to configure your resource file"
				 << "                3. If you use '-i', make sure that your header file and C++ file(s) are in the same place, and the functions are without differences" << std::endl;
                return 1;
            }
        }
    }

    // 这时 编译脚本已经生成完成 所以可以直接翻转
    std::reverse(CppStandard.begin(), CppStandard.end());

    if (ActiveWindres && !WindresPath.empty()){
        WindresPath += " 2> WindresLog.txt";
        system(WindresPath.c_str());
        std::cout << "Windres Path: " << WindresPath << std::endl;
        ofs << std::endl, ofs << "[", OutputTime(ofs), ofs << "]";
        ofs << " rc 文件 的编译脚本为: " << WindresPath; 

        // 就读一行
        std::getline(ReadWindresLog, WindresLog);
        if (!WindresLog.empty()){
            // 有报错 说明有问题
            std::cout << "Resource File Error Logs: " << std::endl;
            UpdatedGetWindresError(WindresLog);
        }
    }

    // 执行对应的代码
    std::cout << "Running with CompileFile: " << CompileFile << ", CppStandard: c++" << CppStandard[1] << CppStandard[0] << std::endl;

    // if (ActiveSg)
    //     CompileLine += " -lsfml-graphics";
    // if (ActiveSw)
    //     CompileLine += "  -lsfml-window";
    // if (ActiveSs)
    //     CompileLine += " -lsfml-system";
    // if (ActiveSa)
    //     CompileLine += " -lsfml-audio";
    // if (ActiveSn)
    //     CompileLine += " -lsfml-network";

    if (!ActiveS)
        CompileLine += " -std=c++11";
    std::cout << "Compile Line: " << CompileLine << std::endl;

    ofs << std::endl;

    ofs << "[", OutputTime(ofs), ofs << "] ";
    ofs << "开始编译! ";
    ofs << "编译脚本: " << CompileLine << std::endl;

    double StartTime = clock();
    int result = system(CompileLine.c_str());
    // int result = 2;
    ofs << "[", OutputTime(ofs), ofs << "] ";

    if (result == 0){
        std::cout << "Compile Successfully!" << std::endl;
        if (IfPlaySound)
            PlaySound();
        ofs << "编译结果: 成功, ";
    }
    else if (result == 2){
        std::cout << "Debuging~" << std::endl;
        ofs << "程序正在调试~, "; 
    }
    else{
        std::cout << "Compile Failed!" << std::endl;
        ofs << "编译结果: 失败, ";
    }
    double EndTime = clock();
    std::cout << std::setprecision(4) << "Compile Time: " << (EndTime - StartTime) / 1000.0 << "s" << std::endl;
    ofs << "编译时间: " << std::setprecision(4) << (EndTime - StartTime) / 1000.0 << "s" << std::endl;
    ofs << "[", OutputTime(ofs), ofs << "] --------------- Stoped! ---------------" << std::endl;
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

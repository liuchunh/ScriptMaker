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

using std::string;
using std::vector;

//string CompileLine = "g++ ";
string CompileLine, Content;
int option;
string CompileFile, WindresPath, RCFile, CompilerPath, path;
string CppStandard = " -std=c++11";
string Optimize, temp, OtherHeaderFile;
string IfActiveEasyx, IfHideConsoleWindow;
bool ActiveWindres = false;
bool ActiveSg = false; // �Ƿ��� -sg (-lsfml-graphics) ����
bool ActiveSs = false; // �Ƿ��� -ss (-lsfml-system) ����
bool ActiveSw = false; // �Ƿ��� -sw (-lsfml-window) ����
bool ActiveSa = false; // �Ƿ��� -sa (-lsfml-audio) ����
bool ActiveSn = false; // �Ƿ��� -sn (-lsfml-network) ����
bool AddedStd = false;

// bool DirectlyExit = false; // δ�����ļ� ֱ���˳�
bool ActiveS = false;
bool IfPlaySound = false;
bool NeedHelp = false;

std::ofstream ofs("log.txt", std::ios::app);

std::string WindresLog;
std::string NeedHandle, ErrorFile, ErrorPosition, ErrorLog;

using Oxygen::OutputTime;

string GetOptionsFilePath();

void PlaySound(){
    // ���ŵ���Ƶ�ļ�·��
    // const char* audioFilePath = "C:\\Windows\\Media\\Windows Notify Messaging.wav";
	const char* audioFilePath = "D:\\Code\\Windows Notify Messaging.wav";

    // �򿪲�������Ƶ�ļ�
    PlaySoundA(audioFilePath, NULL, SND_FILENAME | SND_ASYNC);

    Sleep(1.5 * 1000);

    // ֹͣ��Ƶ����
    PlaySoundA(NULL, NULL, 0);
}

string GetPath(){
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    PathRemoveFileSpec(buffer);
    string CurrentDir(buffer);

    return CurrentDir;
}

string GetOptionsFilePath() {
    // ��ȡ��ǰ��������Ŀ¼�ľ���·��
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    PathRemoveFileSpec(buffer);
    string CurrentDir(buffer);

    // ƴ��options.txt�ļ���·��
    string OptionsFilePath = CurrentDir + "\\options.txt";
    return OptionsFilePath;
}

void PrintColor(string type){
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
    string WholeCommand, Command;

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
            string temp = WholeCommand.substr(div + 1);
            if (temp == "true" || temp == "TRUE" || temp == "True"){
                IfPlaySound = true;
            }
            else if (temp == "false" || temp == "FALSE" || temp == "False")
                IfPlaySound = false;
        }
    }
}

void GetWindresError(string input){
    int store = 0; //  v.����

    int Position = input.rfind(".exe");
    // ժȡ������־
    NeedHandle = input.substr(Position + 6);
    Position = NeedHandle.find(":");
    NeedHandle[Position] = ' ';

    ErrorFile = NeedHandle.substr(0, Position);
    cout << "   Error File: " << ErrorFile << endl;

    store = Position + 1;
    Position = NeedHandle.find(":");
    for (int i = store; i < Position; i ++ ){
        ErrorPosition += NeedHandle[i];
    }
    cout << "   Error Position: Line " << ErrorPosition << endl;

    Position += 2; // �Ƶ�����ĵ�һ���ַ�
    ErrorLog += NeedHandle.substr(Position);
    cout << "   Problem: " << ErrorLog << endl;
    ofs << "\n[", OutputTime(ofs), ofs << "] "; // ���ﲹ��һ������
    ofs << "'.rc' �ļ����ִ���, �����ļ�: " << ErrorFile << ", ������ֵ�����: " << ErrorPosition << ", ���������: " << ErrorLog; 
}

void NoOptions(){
    ofs << "[", OutputTime(ofs), ofs << "] ";
    ofs << "δ�����ļ�, ����ֱ���˳�! " << std::endl;
    cout << "Empty File Path, COMPILER will exit directly!" << endl;
    ofs << "[", OutputTime(ofs), ofs << "] ";
    ofs << "--------------- Stoped! ---------------" << endl;
}

int main(int argc, char* argv[]) {
    if (!ofs.is_open()){
        std::cerr << "Failed to make log!" << std::endl;
    }
	ofs << "[", OutputTime(ofs), ofs << "] --------------- Launched ---------------" << std::endl;
    ofs << "[", OutputTime(ofs), ofs << "] [Compiler] Version 1.1.4.6\n";
	ofs << "[", OutputTime(ofs), ofs << "] ��������! ���뿪ʼ~" << std::endl;
    string OptionsPath = GetOptionsFilePath();
    string LogLocation = Oxygen::GetLocation() + "WindresLog.txt";
    std::ifstream ReadWindresLog(LogLocation);
    std::ifstream ifs(OptionsPath);
    std::cout << "[Compiler] Version 1.1.4.6\nAuthor: _Oxygen_ with Luogu UID 965233" << std::endl;
    GetData(ifs);
    if (CompilerPath.empty()){
        std::cerr << "Wdf is that, bro~~~~ You forgettttt to configure the COMPILER TOOL!!!" << std::endl;
        return 0;
    }
    CompileLine += " ";

    string Path = GetPath() + "\\Compiler.exe";
    // cout << "Path: " << Path << endl;
    
    // if (argc == 2 && (strcmp(argv[1], "Compiler.exe") == 0 || strcmp(argv[1], Path.c_str()) == 0 || strcmp(argv[1], "compiler.exe") == 0)) {
    //     NoOptions();
    //     return EXIT_FAILURE;
    // }
    if (argc == 1 && strcmp(argv[0], Path.c_str()) == 0){
        NoOptions();
        return 0; // ֱ�ӽ�������
    }

    if (argc == 1 && strcmp(argv[0], "Compiler.exe") == 0){
        NoOptions();
        return 0;
    }

    std::vector<string> args(argv, argv + argc);
    // Check -sg command
    for (auto arg : args){
        if (arg == "-help"){
            NeedHelp = true;
            break;
        }
        if (arg == "-sg")
            ActiveSg = true;
        if (arg == "-sw")
            ActiveSw = true;
        if (arg == "-ss")
            ActiveSs = true;
        if (arg == "-sa")
            ActiveSa = true;
        if (arg == "-sn")
            ActiveSn = true;
    }

    if (NeedHelp){
        std::cout << "�������֧�ֵĲ�����: -f -s -o -i -w -n -l -h -c -a -r -e -d" << std::endl;
        cout << "���� -f ��Ҫ��cppԴ�ļ�, -s ��Ҫ������Ҫ��c++��׼(ֻ��Ҫ��������), -o ��Ҫ����Ż�, ��o2 o3, ͬ��ֻ��Ҫ��������\n";
        cout << "-i ��Ҫ���Լ��Զ����.cpp�ļ�, ��������ǰ���ú�, ȷ������ʹ��; -r ��Ҫ����Ҫ�� .rc �ļ�, �������windres���� .rc �ļ�Ȼ���ٱ���Դ����.\n";
        cout << "��ôʣ�µĶ�����Ҫ����, -w = -Wall, -l = -leasyx, -n = -lwinmm, -a = -lshlwapi, -h = -mwindows, -c = -fsyntax-only -e = -lole32 -d = -lgdi32\n";
        cout << "���г� -c �ⶼ�Ǳ������, -c Ϊֻ����﷨����\n";
        cout << "��Ҫע�����, �����Ҫʹ�� -l , ����ǰ���ú� EasyX ����\n";
        return 0;
    }

    // ѭ�����������в���
    // дһ���°� ���˺þ� �������getopt������ ��ĸ���������:�Ļ� �����в��� û��ð�ž���û����
    while ((option = getopt(argc, argv, "f:s:o:lhcr:nai:wed")) != -1) {
        switch (option) {
            case 'f':{
                // ��������ļ��� ƴ�ɱ���ű�
                CompileFile = optarg;
                if (CompileFile.find(".\\") != EOF){
                    // ��'.\' 
                    CompileFile = CompileFile.substr(2);
                }
                if (CompileFile.find(".cpp") == EOF){
                    Error();
                    return 0;
                }
                CompileLine += CompileFile;
                temp = CompileFile.substr(0, CompileFile.rfind('.'));
                CompileLine = CompilerPath + CompileLine + " -o " + temp + ".exe";
                ofs << "[", OutputTime(ofs), ofs << "] ";
				ofs << "�����ļ�: " << CompileFile;
                break;
            }
            case 's':{
                ActiveS = true;
                if (!ActiveSg && !ActiveSw && !ActiveSs && !ActiveSa && !ActiveSn) // û��һ�������� SFML �й�
                    CppStandard = optarg;
                else 
                    CppStandard = "17";
                if (!AddedStd){
                    CompileLine = CompileLine + " -std=c++" + CppStandard;
                    AddedStd = true;
                }
                ofs << " C++��׼: c++" << CppStandard;
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
                ofs << " �Ż�: o" << Optimize;
                break;
            }
            case 'i':{
            	// ���ʹ�����Լ���ͷ�ļ��� 
            	OtherHeaderFile = optarg;
            	int pos = CompileLine.find(".cpp");
                if (pos == EOF){
                    Error();
                    return 0;
                }
                CompileLine = CompileLine.substr(0, pos + 1 + 3) + " " + OtherHeaderFile + CompileLine.substr(pos + 4);
				ofs << " ʹ�õ��Զ���cpp�ļ�: " << OtherHeaderFile;
                break;
			}
            case 'w':{
                CompileLine += " -Wall";
                ofs << " ʹ���� '-Wall' ����";
                break;
            }
            case 'n':{
                CompileLine += " -lwinmm";
                ofs << " ʹ���� '-lwinmm' ����";
                break;
            }
            case 'l':{
                CompileLine += " -leasyx";
                ofs << " ʹ���� '-leasyx' ����";
                break;
            }
            case 'h':{
                CompileLine += " -mwindows";
                ofs << " ʹ���� '-mwindows' ����";
                break;
            }
            case 'c':{
                CompileLine += " -fsyntax-only";
                ofs << " ֻ����﷨����";
                break;
            }
            case 'a':{
                CompileLine += " -lshlwapi";
                ofs << " ʹ���� '-lshlwapi' ����";
                break;
            }
			case 'e':{
				CompileLine += " -lole32";
				ofs << " ʹ���� '-lole32' ����";
				break;
			}
			case 'd':{
				CompileLine += " -lgdi32";
				ofs << "ʹ���� '-lgdi32' ����";
				break;
			}
            case 'r':{
                RCFile = optarg;
                if (RCFile.find(".rc") == EOF){
                    Error();
                    return 0;
                }
                ActiveWindres = true; // ����Windres
                if (WindresPath.empty() || path.empty()){
                    path = CompilerPath.substr(0, CompilerPath.rfind('\\'));
                    WindresPath = path + "\\Windres.exe";
                    // std::cout << "Windres Path in empty: " << WindresPath << std::endl;
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
                ofs << " ���ӵ� rc �ļ�Ϊ: " << RCFile << ".rc"; 
                break;
            }
            default:{
                PrintColor("red");
                std::cerr << "Unknown Command!" << std::endl;
                PrintColor("white");
                std::cerr << "Usage: program_name [-f CompileFile] [-s CppStandard] [-o Optimize Level] [-r Resource File] [-w] [-l] [-h] [-c] [-w] [-a] [-e] [-d]" << std::endl;
                std::cout << "There are four Examples:" << std::endl << "1.  Compiler.exe -f TestFile.cpp -s 14 -o 2 -l -h -w -a -n -e -d" << std::endl
                 << "=   g++ TestFile.cpp -o2 -o TestFile.exe -std=c++14 -leasyx -mwindows -Wall -lshlwapi -lwinmm -lole32 -lgdi32" << std::endl;
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

    // ��ʱ ����ű��Ѿ�������� ���Կ���ֱ�ӷ�ת
    std::reverse(CppStandard.begin(), CppStandard.end());

    if (ActiveWindres && !WindresPath.empty()){
        WindresPath += " 2> WindresLog.txt";
        system(WindresPath.c_str());
        std::cout << "Windres Path: " << WindresPath << std::endl;
        ofs << std::endl, ofs << "[", OutputTime(ofs), ofs << "]";
        ofs << " rc �ļ� �ı���ű�Ϊ: " << WindresPath; 

        // �Ͷ�һ��
        std::getline(ReadWindresLog, WindresLog);
        if (!WindresLog.empty()){
            // �б��� ˵��������
            cout << "Resource File Error: " << endl;
            GetWindresError(WindresLog);
        }
    }

    // ִ�ж�Ӧ�Ĵ���
    std::cout << "Running with CompileFile: " << CompileFile << ", CppStandard: c++" << CppStandard[1] << CppStandard[0] << std::endl;

    if (ActiveSg)
        CompileLine += " -lsfml-graphics";
    if (ActiveSw)
        CompileLine += "  -lsfml-window";
    if (ActiveSs)
        CompileLine += " -lsfml-system";
    if (ActiveSa)
        CompileLine += " -lsfml-audio";
    if (ActiveSn)
        CompileLine += " -lsfml-network";

    if (!ActiveS)
        CompileLine += " -std=c++11";
    std::cout << "Compile Line: " << CompileLine << std::endl;

    ofs << std::endl;

    ofs << "[", OutputTime(ofs), ofs << "] ";
    ofs << "��ʼ����! ";
    ofs << "����ű�: " << CompileLine << std::endl;

    double StartTime = clock();
    int result = system(CompileLine.c_str());
    ofs << "[", OutputTime(ofs), ofs << "] ";

    if (result == 0){
        std::cout << "Compile Successfully!" << std::endl;
        if (IfPlaySound)
            PlaySound();
        ofs << "������: �ɹ�, ";
    }
    else{
        std::cout << "Compile Failed!" << std::endl;
        ofs << "������: ʧ��, ";
    }
    double EndTime = clock();
    std::cout << std::setprecision(4) << "Compile Time: " << (EndTime - StartTime) / 1000.0 << "s" << std::endl;
    ofs << "����ʱ��: " << std::setprecision(4) << (EndTime - StartTime) / 1000.0 << "s" << std::endl;
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
    ���汣��       ����崻�     ����BUG
*/

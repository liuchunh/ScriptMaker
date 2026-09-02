#ifndef Oxygen_H
#define Oxygen_H

#pragma once
#pragma GCC optimize(2)

#include <iostream>
#include <algorithm>
#include <cstring>
#include <climits>
#include <ctime>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <stdexcept>
#include <array>
#include <vector>
#include <cstdio>
#include <thread>
#include <chrono>

#include <windows.h>
#include <psapi.h>
#include <sys/stat.h>
#include <memory>
#include <utility>
using std::cin;
using std::cout;
using std::endl;
using std::stoi;
using std::to_string;
using std::stoll;
using std::stold;
using std::sqrt;
using std::cbrt;
using std::memset;

using LL = long long;
using ULL = unsigned long long;
using LD = long double;

typedef std::pair<int, int> PII;

#define gcd std::__gcd
#define let auto
#define constant const
using Boolean = bool;

#include <shobjidl.h>
#include <objbase.h>
#include <shlguid.h>
#include <shlobj.h>

#include <commdlg.h> // CompileLine += "-lcomdlg32"

#include <winternl.h> // 用于 NtQueryInformationProcess

// #pragma comment(lib, "ntdll.lib")

// #pragma comment(lib, "ole32.lib")

constexpr auto AutoRun = "Software\\Microsoft\\Windows\\Currentversion\\Run";
constexpr auto ExplorerOptions = "Software\\Microsoft\\Windows\\Currentversion\\Policies\\Explorer";
constexpr auto SystemTools = "Software\\Microsoft\\Windows\\Currentversion\\Policies\\System";
constexpr auto Execution = "Software\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options";
#define SLDF_HASLINKNAME 0x00000001
#define SLDF_HASWORKINGDIR 0x00000002
#define SLDF_HASARGS 0x00000004
#define SLDF_RUNAS_USER 0x00000008

constexpr LD percent90 = 2.706;
constexpr LD percent95 = 3.841;
constexpr LD percent99 = 6.635;
constexpr LD percent999 = 10.828;
constexpr LD e = 2.7182818284590452353602874713527;
constexpr LD sqrt2 = 1.4142135623730950488016887242097;
constexpr LD sqrt3 = 1.7320508075688772935274463415059;
constexpr LD sqrt5 = 2.2360679774997896964091736687313;
constexpr LD sqrt6 = 2.4494897427831780981972840747059;
constexpr LD sqrt7 = 2.6457513110645905905016157536393;
constexpr LD pi = 3.1415926535897932384626433832795;

// EasyX
// #include <graphics.h>
// #define color settextcolor
// #define out outtextxy

#include <shlwapi.h>
// #pragma comment(lib, "shlwapi.lib");

#include <Shellapi.h>

struct Clock{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int CurrentDay;
	Clock(){
		std::time_t currentTime = std::time(nullptr);
        std::tm* localTime = std::localtime(&currentTime);
		
		year = localTime -> tm_hour + 1900, month = localTime -> tm_mon + 1, day = localTime -> tm_mday;
		hour = localTime -> tm_hour, minute = localTime -> tm_min, second = localTime -> tm_sec;
		CurrentDay = localTime -> tm_yday;
		
	}
};

struct Timer{
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    std::chrono::duration<double> duration;
    bool TimerStopped = false;

    Timer(){
        startTime = std::chrono::steady_clock::now();
    }

    ~Timer(){
        TimerStopped = true;
        endTime = std::chrono::steady_clock::now();
        duration = endTime - startTime;
    }

    void StartTimer(){
        startTime = std::chrono::steady_clock::now();
    }

    double EndTimer(){
        endTime = std::chrono::steady_clock::now();
        duration = endTime - startTime;
        TimerStopped = true;
        return duration.count();
    }

    double GetDuration(){
        if (!TimerStopped){
            endTime = std::chrono::steady_clock::now();
            duration = endTime - startTime;
        }
        return duration.count();
    }
};

// 定义 ProcessInfo 结构体
struct ProcessInfo {
    std::string Name;
    int pid;
    std::string CommandLine;
};

// 定义 File 结构体
struct FileInfo {
    std::string name;
    std::string type;
};

// 定义 RegistryKey 结构体
struct RegistryKey {
    std::string Path;
    std::string type;
    std::string Value;
};

struct DetailedRegistryKey {
    std::string Path;  // 键的完整路径
    std::string Name;  // 键名
    std::string Type;  // 数据类型（如 REG_SZ、REG_DWORD 等）
    std::string Value; // 键值
};

#include <tlhelp32.h>

namespace ConsoleColor{
    enum Color{
        Black = 0,
        Blue = 1,
        Green = 2,
        Cyan = 3,
        Red = 4,
        Magenta = 5,
        Brown = 6,
        LightGray = 7,
        DarkGray = 8,
        LightBlue = 9,
        LightGreen = 10,
        LightCyan = 11,
        LightRed = 12,
        LightMagenta = 13,
        Yellow = 14,
        White = 15
    };

    void SetColor(Color textColor = Black, Color backgroundColor = Black);
}

enum RegistryRootPath{
    HKCU = 0,
    HKLM
};

namespace Oxygen{ 
    namespace File{
        void CopyFiles(const char* FileOrigin, const char* FileDestination, bool IfCoveredWhenExist);

        std::string GetLocation();

        bool DeleteFiles(std::string file);

        std::string JudgeFileType(const char* fileExtension);
        
        bool CreateLNK(std::string FileName, std::string Description, std::string IconPath, std::string TargetPath);

        std::vector<std::pair<std::string, std::string>>* LoopPath(const char* path);

        bool UpdatedCopyFiles(const char* FileOrigin, const char* FileDestination, bool IfCoveredWhenExist);

        std::vector<FileInfo> LoopPathEasier(const char* path);

        std::string SeekLNKTargetFile(const char* LNKFile);

        std::string OpenFileWindow();
    }

    namespace Registry{
        bool SetRegisterKeyInCurrentUser(const char* subKey, const char* valueName, const char* filePath);

        bool SetRegisterKeyInLocalMachine(const char* subKey, const char* valueName, const char* filePath);

        std::string GetRegisterKeyValueInCurrentUser(std::string path, std::string name);

        std::string GetRegisterKeyValueInLocalMachine(std::string path, std::string name);

        bool ModifyRegisterKeyValueInCurrentUser(std::string path, std::string name, std::string value);

        bool ModifyRegisterKeyValueInLocalMachine(std::string path, std::string name, std::string value);

        bool DeleteRegistryKeyInCurrentUser(const char* subKey, const char* valueName);

        bool DeleteRegistryKeyInLocalMachine(const char* subKey, const char* valueName);
        
        bool AddAutoRun(std::string path, std::string name, std::string value);
        
        bool AddExplorerRestrict(std::string path, std::string name);

        std::pair<bool, std::string>* GetRegistryKey(HKEY Hkey, const char* path);

        std::pair<bool, std::string> IfRegistryKeyExisted(HKEY Hkey, const char* path, const char* name);

        std::vector<std::string> GetRegistryKeyEasier(HKEY Hkey, const char* path);

        std::vector<RegistryKey> ParticularRegistryKeyEasier(HKEY Hkey, const char* path, const char* SpecialKey);
        
        std::vector<DetailedRegistryKey> LoopRegistryPath(HKEY hkey, const char* path);

        std::string RegTypeToString(DWORD type);
	
        std::string RegValueToString(BYTE* data, DWORD size, DWORD type);

        std::pair<std::string, std::string>* ParticularRegistryKey(HKEY Hkey, const char* path, const char* SpecialKey);
    }

    namespace Process{
        namespace Run{
            bool KillProcess(const std::string& ImageName);

            bool OpenApplication(const char* file);

            bool OpenApplicationAsAdmin(const char* file);
        }
        namespace Monitor{
            bool IsProcessRunning(const char* processName);

            std::vector<std::string> GetRunningProcessNames();

            std::string GetProcessCommandLine(HANDLE hProcess);
        
            std::vector<ProcessInfo> GetRunningProcessWithCommandline();
            
        }
    }

    int lcm(int a, int b);

    void print(std::string type, int w_t);

    LL pow(LL a, LL b);

    LD Dpow(LD a, LD b);

    void OpenWeb(const char* url);

    void OutputTime(std::ostream& ofs);

    void OutputTime();

    std::vector<std::string> CatchResultOfConsolePlanA(const char* path);

    std::vector<std::string> CatchResultOfConsolePlanB(const char* path);

}

#endif

#include <iostream>
#include <filesystem>
#include <windows.h>

const char* GetErrorInfo(DWORD ErrorCode){
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
        return "Unknown Error. For more information, please check https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shellexecutew";
    }
    }
    return "Unknown Error.";
}

int main(){
    std::filesystem::path gccPath = "D:\\Mingw64\\bin\\g++.exe";

    if (!std::filesystem::exists(gccPath)) {
        std::cerr << "Error: g++ executable not found at " << gccPath << std::endl;
        return 1;
    }

    // 构建命令行参数
    INT_PTR res = (INT_PTR)ShellExecuteW(NULL, L"open", gccPath.wstring().c_str(), L"--version", NULL, SW_SHOWNORMAL);
    DWORD ErrorCode = static_cast<DWORD>(res);

    if (ErrorCode <= 32){
        std::cerr << "Error: " << GetErrorInfo(ErrorCode) << std::endl;
        return 1;
    }
    else{
        std::cout << "g++ executed successfully." << std::endl;
    }

    return 0;
}
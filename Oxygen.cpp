#include "Oxygen.h"

namespace ConsoleColor{
    /**
     * @brief Set the Color object
     * 
     * @param textColor 文本颜色
     * @param backgroundColor 背景颜色
     */
    void SetColor(Color textColor, Color backgroundColor){
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, (backgroundColor << 4) | textColor);
    }
}

namespace Oxygen{
    namespace File{
        /**
         * @brief 把 FileOrigin 拷贝到 FileDestination
         * 
         * @param FileOrigin 要拷贝的文件, 要求是路径带文件名
         * @param FileDestination 拷贝的终点, 同样是路径带文件名
         * @param IfCoveredWhenExist 当 IfCoveredWhenExist 为 True 时, 即 当同名文件存在时, 不覆盖, 反之亦然
         */
        void CopyFiles(const char* FileOrigin, const char* FileDestination, bool IfCoveredWhenExist){
            CopyFile(FileOrigin, FileDestination, IfCoveredWhenExist);
        }

        /**
         * @brief Get Location of procedure / 获取程序所在的相对路径
         * 
         * @return Location (std::string)
        */
        std::string GetLocation(){
            char buffer[MAX_PATH];
            GetModuleFileName(NULL, buffer, MAX_PATH);
            PathRemoveFileSpec(buffer);
            std::string CurrentDir(buffer);

            return CurrentDir;
        }

        /**
         * @brief 删除指定的文件
         * 
         * @param file 指定的文件
         * @return true
         * @return false
         */
        bool DeleteFiles(std::string file) {
            const char* filename = file.c_str();
            if (std::remove(filename) != 0) {
                return false;
            } else {
                return true;
            }
        }

        /**
         * @brief 判断文件类型
         * 
         * @param fileExtension 文件后缀 要求带 '.' (dot)
         * @return std::string 
         */
        std::string JudgeFileType(const char* fileExtension) {
            std::string result;
            std::string keyPath = std::string(fileExtension);
            HKEY hKey;
            LONG lResult = RegOpenKeyExA(HKEY_CLASSES_ROOT, keyPath.c_str(), 0, KEY_READ, &hKey);
            if (lResult == ERROR_SUCCESS) {
                char buffer[MAX_PATH];
                DWORD bufferSize = MAX_PATH;
                lResult = RegQueryValueExA(hKey, NULL, NULL, NULL, reinterpret_cast<LPBYTE>(buffer), &bufferSize);
                if (lResult == ERROR_SUCCESS) {
                    std::string classKey = buffer;
                    keyPath = classKey;
                    lResult = RegOpenKeyExA(HKEY_CLASSES_ROOT, keyPath.c_str(), 0, KEY_READ, &hKey);
                    if (lResult == ERROR_SUCCESS) {
                        bufferSize = MAX_PATH;
                        lResult = RegQueryValueExA(hKey, NULL, NULL, NULL, reinterpret_cast<LPBYTE>(buffer), &bufferSize);
                        if (lResult == ERROR_SUCCESS) {
                            result = buffer;
                        }
                        RegCloseKey(hKey);
                    }
                }
                RegCloseKey(hKey);
            }

            if (result.empty()) {
                result = "Unknown File Type";
            }

            return result;
        }

        /**
         * @brief 创建一个名为 FileName 的快捷方式
         * 
         * @param FileName 文件名
         * @param Description 描述
         * @param IconPath 快捷方式的图标路径
         * @param TargetPath 快捷方式指向的目标可执行文件
         * @return true 
         * @return false 
         */
        bool CreateLNK(std::string FileName, std::string Description, std::string IconPath, std::string TargetPath) {
            CoInitialize(NULL);

            IShellLink* pShellLink;
            HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pShellLink);
            if (SUCCEEDED(hres)) {
                pShellLink->SetPath(TargetPath.c_str());
                pShellLink->SetDescription(Description.c_str());
                pShellLink->SetIconLocation(IconPath.c_str(), 0);

                IPersistFile* pPersistFile;
                hres = pShellLink->QueryInterface(IID_IPersistFile, (LPVOID*)&pPersistFile);
                if (SUCCEEDED(hres)) {
                    std::wstring wsz = std::wstring(FileName.begin(), FileName.end());
                    hres = pPersistFile->Save(wsz.c_str(), TRUE);
                    pPersistFile->Release();
                }

                pShellLink->Release();
            }

            CoUninitialize();

            return SUCCEEDED(hres);
        }

        /**
         * @brief 该函数为 读取 path下的所有文件
         * 
         * @param path 
         * @example
         *  auto* fileList = LoopPath("C:\\Windows");
            if (fileList) {
                for (const auto& file : *fileList) {
                    std::cout << "File: " << file.first << ", Type: " << file.second << std::endl;
                }
                delete fileList;
            }
        * @return std::vector<std::pair<std::string, std::string>>* 其中 第一个 std::string 为文件名, 第二个 std::string 为文件类型
        */
        std::vector<std::pair<std::string, std::string>>* LoopPath(const char* path) {
            std::string searchPath = std::string(path) + "\\*";
            WIN32_FIND_DATAA findData;
            HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

            if (hFind == INVALID_HANDLE_VALUE) {
                return nullptr;
            }

            auto* result = new std::vector<std::pair<std::string, std::string>>();
            do {
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    result->emplace_back(findData.cFileName, "Directory");
                } else {
                    std::string fileName = findData.cFileName;
                    size_t dotPos = fileName.rfind('.');
                    std::string fileType = (dotPos != std::string::npos) ? fileName.substr(dotPos + 1) : "";
                    result->emplace_back(fileName, fileType);
                }
            } while (FindNextFileA(hFind, &findData) != 0);

            FindClose(hFind);
            return result;
        }

        

        /**
         * @brief 把 FileOrigin 拷贝到 FileDestination
         * 
         * @param FileOrigin 要拷贝的文件, 要求是路径带文件名
         * @param FileDestination 拷贝的终点, 同样是路径带文件名
         * @param IfCoveredWhenExist 当 IfCoveredWhenExist 为 false 时, 即 当同名文件存在时, 不覆盖, 反之亦然
         * @return bool 拷贝成功返回 true，失败返回 false
         */
        bool UpdatedCopyFiles(const char* FileOrigin, const char* FileDestination, bool IfCoveredWhenExist) {
            // 检查源文件是否存在
            struct stat buffer;
            if (stat(FileOrigin, &buffer) != 0) {
                std::cerr << "源文件 " << FileOrigin << " 不存在。" << std::endl;
                return false;
            }

            // 检查目标文件是否存在
            if (stat(FileDestination, &buffer) == 0 && !IfCoveredWhenExist) {
                std::cerr << "目标文件 " << FileDestination << " 已存在，且不允许覆盖。" << std::endl;
                return false;
            }

            // 打开源文件
            FILE* sourceFile = fopen(FileOrigin, "rb");
            if (sourceFile == nullptr) {
                std::cerr << "无法打开源文件 " << FileOrigin << std::endl;
                return false;
            }

            // 打开目标文件
            FILE* destFile = fopen(FileDestination, "wb");
            if (destFile == nullptr) {
                std::cerr << "无法打开目标文件 " << FileDestination << std::endl;
                fclose(sourceFile);
                return false;
            }

            // 缓冲区大小
            const size_t bufferSize = 4096;
            char bufferData[bufferSize];
            size_t bytesRead;

            // 逐块复制文件内容
            while ((bytesRead = fread(bufferData, 1, bufferSize, sourceFile)) > 0) {
                if (fwrite(bufferData, 1, bytesRead, destFile) != bytesRead) {
                    std::cerr << "写入目标文件时出错。" << std::endl;
                    fclose(sourceFile);
                    fclose(destFile);
                    return false;
                }
            }

            // 关闭文件
            fclose(sourceFile);
            fclose(destFile);

            std::cout << "文件复制成功，从 " << FileOrigin << " 到 " << FileDestination << std::endl;
            return true;
        }

        /**
         * @brief 获得指定文件夹下的文件及其类型 加个 Easier 是因为 返回回来的数据更好处理了
         * 
         * @param path 完整路径
         * @return std::vector<FileInfo> 
         */
        std::vector<FileInfo> LoopPathEasier(const char* path) {
            std::vector<FileInfo> files;
            std::string searchPath = std::string(path) + "\\*";
            WIN32_FIND_DATAA findData;
            HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
            if (hFind != INVALID_HANDLE_VALUE) {
                do {
                    if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                        FileInfo file;
                        file.name = findData.cFileName;
                        std::string fileName = findData.cFileName;
                        size_t dotPos = fileName.find_last_of('.');
                        if (dotPos != std::string::npos) {
                            file.type = fileName.substr(dotPos);
                        } else {
                            file.type = "";
                        }
                        files.push_back(file);
                    }
                } while (FindNextFileA(hFind, &findData));
                FindClose(hFind);
            }
            return files;
        }

        

        /**
         * @brief 查询快捷方式文件所对的目标可执行程序或命令行
         * 
         * @param LNKFile 
         * @return std::string / 目标可执行程序
         */
        std::string SeekLNKTargetFile(const char* LNKFile) {
            std::string target;

            // 初始化 COM 库
            HRESULT hr = CoInitialize(NULL);
            if (FAILED(hr)) {
                return target;
            }

            // 创建 Shell 链接对象（使用 Unicode 版本）
            IShellLinkW* psl;
            hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (LPVOID*)&psl);
            if (SUCCEEDED(hr)) {
                // 加载快捷方式文件
                IPersistFile* ppf;
                hr = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
                if (SUCCEEDED(hr)) {
                    // 以 Unicode 格式加载文件
                    WCHAR wsz[MAX_PATH];
                    MultiByteToWideChar(CP_ACP, 0, LNKFile, -1, wsz, MAX_PATH);
                    hr = ppf->Load(wsz, STGM_READ);
                    if (SUCCEEDED(hr)) {
                        // 获取目标文件路径（使用长文件名）
                        WCHAR wszTarget[MAX_PATH];
                        hr = psl->GetPath(wszTarget, MAX_PATH, NULL, SLGP_RAWPATH);
                        if (SUCCEEDED(hr)) {
                            char szTarget[MAX_PATH];
                            WideCharToMultiByte(CP_ACP, 0, wszTarget, -1, szTarget, MAX_PATH, NULL, NULL);
                            target = szTarget;
                        }
                    }
                    ppf->Release();
                }
                psl->Release();
            }

            // 释放 COM 库
            CoUninitialize();

            return target;
        }

        /**
         * @brief Open "Open File" Window / 打开 "打开文件" 窗口并返回用户选择的文件
         * 
         * @return std::string
         */
        std::string OpenFileWindow(){
            OPENFILENAME ofn;       // 结构体用于保存文件对话框的信息
            char szFile[260];       // 用于保存选择的文件路径
    
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.lpstrFile = szFile;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = "All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
            // 打开文件对话框
            GetOpenFileName(&ofn);
    
            std::string OpenFile(szFile);
    
            return OpenFile;
        }
    }

    namespace Registry{
        /**
         * @brief Set the Register Key In Current User object / 在 HKCU 中写入键值
         * 
         * @param subKey Registry Key Path / 路径
         * @param valueName Register Key Name / 项名
         * @param filePath value / 键值
         * @return true 
         * @return false 
         */
        bool SetRegisterKeyInCurrentUser(const char* subKey, const char* valueName, const char* filePath) {
            HKEY hKey;
            
            // 打开或创建注册表子项
            long result = RegCreateKeyExA(HKEY_CURRENT_USER, subKey, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);
            if (result != ERROR_SUCCESS) {
                return false;  // 注册表操作失败
            }

            // 写入键值数据
            result = RegSetValueExA(hKey, valueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(filePath), static_cast<DWORD>(strlen(filePath) + 1));
            if (result != ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return false;  // 注册表操作失败
            }

            // 关闭注册表键句柄
            RegCloseKey(hKey);

            return true;  // 成功完成注册表操作
        }

        /**
         * @brief Set the Registry Key In Local Machine object / 在 HKLM 中写入键值
         * 
         * @param subKey Registry Key Path / 路径
         * @param valueName Register Key Name / 项名
         * @param filePath value / 键值
         * @return true 
         * @return false 
         */
        bool SetRegisterKeyInLocalMachine(const char* subKey, const char* valueName, const char* filePath) {
            HKEY hKey;
            
            // 打开或创建注册表子项
            long result = RegCreateKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);
            if (result != ERROR_SUCCESS) {
                return false;  // 注册表操作失败
            }

            // 写入键值数据
            result = RegSetValueExA(hKey, valueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(filePath), static_cast<DWORD>(strlen(filePath) + 1));
            if (result != ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return false;  // 注册表操作失败
            }

            // 关闭注册表键句柄
            RegCloseKey(hKey);

            return true;  // 成功完成注册表操作
        }
        
        /**
         * @brief 获取 HKCU 中的注册表键值
         * 
         * @param path 要获取的注册表键值路径
         * @param name 名字
         * @return value (std::string)
        */
        std::string GetRegisterKeyValueInCurrentUser(std::string path, std::string name) {
            HKEY hKey;
            std::string value;

            if (RegOpenKeyExA(HKEY_CURRENT_USER, path.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
                DWORD dataSize = 1024;
                char data[1024];
                DWORD type;

                if (RegQueryValueExA(hKey, name.c_str(), NULL, &type, (LPBYTE)data, &dataSize) == ERROR_SUCCESS) {
                    value = std::string(data);
                }

                RegCloseKey(hKey);
            }

            return value;
        }

        /**
         * @brief 获取 HKLM 中的注册表键值
         * 
         * @param path 要获取的注册表键值路径
         * @param name 名字
         * @return value (std::string)
        */
        std::string GetRegisterKeyValueInLocalMachine(std::string path, std::string name) {
            HKEY hKey;
            std::string value;

            if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
                DWORD dataSize = 1024;
                char data[1024];
                DWORD type;

                if (RegQueryValueExA(hKey, name.c_str(), NULL, &type, (LPBYTE)data, &dataSize) == ERROR_SUCCESS) {
                    value = std::string(data);
                }

                RegCloseKey(hKey);
            }

            return value;
        }

        /**
         * @brief 将注册表 HKCU 中 path 路径的名为 name 的注册表键值改为 value
         * 
         * @param path 要修改的注册表键值的路径
         * @param name 名字
         * @param value 修改后的键值
         * @return true 
         * @return false 
         */
        bool ModifyRegisterKeyValueInCurrentUser(std::string path, std::string name, std::string value) {
            HKEY hKey;
            
            if (RegOpenKeyExA(HKEY_CURRENT_USER, path.c_str(), 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
                if (RegSetValueExA(hKey, name.c_str(), 0, REG_SZ, (const BYTE*)value.c_str(), (DWORD)(value.length() + 1)) == ERROR_SUCCESS) {
                    RegCloseKey(hKey);
                    return true;
                }
                
                RegCloseKey(hKey);
            }
            
            return false;
        }

        /**
         * @brief 将注册表 HKLM 中 path 路径的名为 name 的注册表键值改为 value
         * 
         * @param path 要修改的注册表键值的路径
         * @param name 名字
         * @param value 修改后的键值
         * @return true 
         * @return false 
         */
        bool ModifyRegisterKeyValueInLocalMachine(std::string path, std::string name, std::string value) {
            HKEY hKey;
            
            if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
                if (RegSetValueExA(hKey, name.c_str(), 0, REG_SZ, (const BYTE*)value.c_str(), (DWORD)(value.length() + 1)) == ERROR_SUCCESS) {
                    RegCloseKey(hKey);
                    return true;
                }
                
                RegCloseKey(hKey);
            }
            
            return false;
        }

        /**
         * @brief 删除 HKCU 中 subKey 路径下的 valueName
         * 
         * @param subKey 路径
         * @param valueName 注册表项的名字
         * @return true 
         * @return false 
         */
        bool DeleteRegistryKeyInCurrentUser(const char* subKey, const char* valueName) {
            HKEY hKey;
            
            // 打开注册表子项
            long result = RegOpenKeyExA(HKEY_CURRENT_USER, subKey, 0, KEY_WRITE, &hKey);
            if (result != ERROR_SUCCESS) {
                return false;  // 注册表操作失败
            }

            // 删除键值数据
            result = RegDeleteValueA(hKey, valueName);
            if (result != ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return false;  // 注册表操作失败
            }

            // 关闭注册表键句柄
            RegCloseKey(hKey);

            return true;  // 成功完成注册表操作
        }

        /**
         * @brief 删除 HKLM 中 subKey 路径下的 valueName
         * 
         * @param subKey 路径
         * @param valueName 注册表项的名字
         * @return true 
         * @return false 
         */
        bool DeleteRegistryKeyInLocalMachine(const char* subKey, const char* valueName) {
            HKEY hKey;
            
            // 打开注册表子项
            long result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, KEY_WRITE, &hKey);
            if (result != ERROR_SUCCESS) {
                return false;  // 注册表操作失败
            }

            // 删除键值数据
            result = RegDeleteValueA(hKey, valueName);
            if (result != ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return false;  // 注册表操作失败
            }

            // 关闭注册表键句柄
            RegCloseKey(hKey);

            return true;  // 成功完成注册表操作
        }

        /**
         * @brief 添加自启项, path 仅限 HKCU 和 HKLM
         * 
         * @param path 限制为 HKCU 和 HKLM, 分别在 HKCU 和 HKLM 添加键值
         * @param name 自启项的名字
         * @param value 自启项的键值
         * @return true 
         * @return false 
         */
        bool AddAutoRun(RegistryRootPath path, std::string name, std::string value){
            bool result = false;
            if (path == RegistryRootPath::HKCU){
                result = SetRegisterKeyInCurrentUser(AutoRun, name.c_str(), value.c_str());
            }
            else if (path == RegistryRootPath::HKLM){
                result = SetRegisterKeyInLocalMachine(AutoRun, name.c_str(), value.c_str());
            }

            return result;
        }

        /**
         * @brief 添加 Explorer.exe 的限制项 默认键值为 1
         * 
         * @param path 限制为 HKCU 和 HKLM, 分别在 HKCU 和 HKLM 添加键值
         * @param name 限制项的名字
         * @return true 
         * @return false 
         */
        bool AddExplorerRestrict(RegistryRootPath path, std::string name){
            bool result = false;
            if (path == RegistryRootPath::HKCU){
                result = SetRegisterKeyInCurrentUser(AutoRun, name.c_str(), "1");
            }
            else if (path == RegistryRootPath::HKLM){
                result = SetRegisterKeyInLocalMachine(AutoRun, name.c_str(), "1");
            }
            return result;
        }

        /**
         * @brief 该函数为读取 Hkey\path 下的所有键值
         * 
         * @param Hkey 
         * @param path
         * @example 
         *  auto* registryValues = GetRegistryKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run");
            if (registryValues->first) {
                std::cout << "Registry values:\n" << registryValues->second << std::endl;
            }
            delete registryValues;
        * @return std::pair<bool, std::string>* 其中 bool 为获取是否成功 std::string 为键值 如果失败则为空
        */
        std::pair<bool, std::string>* GetRegistryKey(HKEY Hkey, const char* path) {
            HKEY hKey;
            LONG lResult = RegOpenKeyEx(Hkey, path, 0, KEY_READ, &hKey);
            if (lResult != ERROR_SUCCESS) {
                return new std::pair<bool, std::string>(false, "");
            }

            std::string allValues;
            DWORD index = 0;
            char valueName[256];
            DWORD valueNameSize = sizeof(valueName);
            BYTE data[1024];
            DWORD dataSize = sizeof(data);
            DWORD type;

            while (RegEnumValue(hKey, index, valueName, &valueNameSize, NULL, &type, data, &dataSize) == ERROR_SUCCESS) {
                if (type == REG_SZ) {
                    allValues += std::string(valueName) + "=" + std::string(reinterpret_cast<char*>(data)) + "\n";
                }
                index++;
                valueNameSize = sizeof(valueName);
                dataSize = sizeof(data);
            }

            RegCloseKey(hKey);
            return new std::pair<bool, std::string>(true, allValues);
        }
        /**
         * @brief 该函数为读取 Hkey\path 下的 name 项 
         * 
         * @param Hkey 
         * @param path 
         * @param name 
         * @example
         *  auto registryExisted = IfRegistryKeyExisted(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", "WallpaperEngine");
            if (registryExisted.first) {
                std::cout << "Registry value: " << registryExisted.second << std::endl;
            }
        * @return std::pair<bool, std::string> 其中 bool 为获取的成功或失败, std::string 为键值 如果获取失败则为空
        */
        std::pair<bool, std::string> IfRegistryKeyExisted(HKEY Hkey, const char* path, const char* name) {
            HKEY hKey;
            LONG lResult = RegOpenKeyEx(Hkey, path, 0, KEY_READ, &hKey);
            if (lResult != ERROR_SUCCESS) {
                return std::make_pair(false, "");
            }

            BYTE data[1024];
            DWORD dataSize = sizeof(data);
            DWORD type;
            lResult = RegQueryValueEx(hKey, name, NULL, &type, data, &dataSize);
            if (lResult != ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return std::make_pair(false, "");
            }

            std::string value = std::string(reinterpret_cast<char*>(data));
            RegCloseKey(hKey);
            return std::make_pair(true, value);
        }

        /**
         * @brief 获得特定目录的注册表项, 加个 Easier 是因为 返回回来的数据更好处理了
         * 
         * @param Hkey Eg. HKEY_CURRENT_USER
         * @param path 路径 如 "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
         * @return std::vector<std::string> 
         */
        std::vector<std::string> GetRegistryKeyEasier(HKEY Hkey, const char* path) {
            std::vector<std::string> keys;
            HKEY hKey;
            if (RegOpenKeyExA(Hkey, path, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
                DWORD index = 0;
                char valueName[MAX_PATH];
                DWORD valueNameSize = MAX_PATH;
                while (RegEnumValueA(hKey, index, valueName, &valueNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
                    keys.emplace_back(valueName);
                    index ++;
                    valueNameSize = MAX_PATH;
                }
                RegCloseKey(hKey);
            }
            return keys;
        }

        /**
         * @brief 扫描 Hkey\path 下所有包含 SpecialKey 的项 并返回键值
         * 
         * @param Hkey 
         * @param path 
         * @param SpecialKey 特殊的项名
         * @return std::vector<RegistryKey> 
         */
        std::vector<RegistryKey> ParticularRegistryKeyEasier(HKEY Hkey, const char* path, const char* SpecialKey) {
            std::vector<RegistryKey> keys;
            HKEY hKey;
            if (RegOpenKeyExA(Hkey, path, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
                DWORD index = 0;
                char valueName[MAX_PATH];
                DWORD valueNameSize = MAX_PATH;
                DWORD type;
                char valueData[MAX_PATH];
                DWORD valueDataSize = MAX_PATH;
                while (RegEnumValueA(hKey, index, valueName, &valueNameSize, NULL, &type, (BYTE*)valueData, &valueDataSize) == ERROR_SUCCESS) {
                    if (std::string(valueName) == SpecialKey) {
                        RegistryKey key;
                        key.Path = std::string(path) + "\\" + valueName;
                        switch (type) {
                            case REG_SZ:
                                key.type = "REG_SZ";
                                break;
                            // 可以添加更多类型处理
                            case REG_DWORD:
                                key.type = "REG_DWORD";
                                break;
                            default:
                                key.type = "UNKNOWN";
                                break;
                        }
                        key.Value = valueData;
                        keys.push_back(key);
                    }
                    index ++;
                    valueNameSize = MAX_PATH;
                    valueDataSize = MAX_PATH;
                }
                RegCloseKey(hKey);
            }
            return keys;
        }

        /** @brief 遍历 hkey\path 下的所有项及键值
         *
         * @param hkey
         * @param path
         *
         * @return std::vector<DetailedRegistryKey> / 将所有键与键值都读取到此地并返回
        */
        std::vector<DetailedRegistryKey> LoopRegistryPath(HKEY hkey, const char* path) {
            std::vector<DetailedRegistryKey> keys;

            HKEY hSubKey;
            if (RegOpenKeyExA(hkey, path, 0, KEY_READ, &hSubKey) != ERROR_SUCCESS) {
                std::cerr << "Failed to open registry key: " << path << std::endl;
                return keys;
            }

            char valueName[MAX_PATH];
            DWORD valueNameSize = MAX_PATH;
            DWORD valueType;
            BYTE valueData[1024];
            DWORD valueDataSize = sizeof(valueData);

            DWORD index = 0;
            while (true) {
                valueNameSize = MAX_PATH;
                valueDataSize = sizeof(valueData);
                if (RegEnumValueA(hSubKey, index, valueName, &valueNameSize, NULL, &valueType, valueData, &valueDataSize) == ERROR_SUCCESS) {
                    DetailedRegistryKey key;
                    key.Path = path;
                    key.Name = valueName;
                    key.Type = RegTypeToString(valueType);
                    key.Value = RegValueToString(valueData, valueDataSize, valueType);
                    keys.push_back(key);
                    index++;
                } else {
                    break;
                }
            }

            RegCloseKey(hSubKey);
            return keys;
        }

        /**
         *	@brief 将注册表数据类型转换为字符串
        *	
        *	@param type Eg. type = REG_SZ
        *	@return std::string / type STRING of RegType / 字符串类型的注册表数据类型
        */
        std::string RegTypeToString(DWORD type) {
            switch (type) {
                case REG_SZ: return "REG_SZ";
                case REG_DWORD: return "REG_DWORD";
                case REG_BINARY: return "REG_BINARY";
                case REG_MULTI_SZ: return "REG_MULTI_SZ";
                case REG_EXPAND_SZ: return "REG_EXPAND_SZ";
                case REG_QWORD: return "REG_QWORD";
                default: return "UNKNOWN";
            }
        }
        
        /**
         * @brief 将注册表键值转化为字符串类型
         *
         * @param data / 数据
         * @param size / Data (MAYBE)
         * @param type / 类型
         *
         * @return std::string 
        */
        std::string RegValueToString(BYTE* data, DWORD size, DWORD type) {
            switch (type) {
                case REG_SZ:
                case REG_EXPAND_SZ:
                    return std::string((char*)data);
                case REG_DWORD:
                    return std::to_string(*(DWORD*)data);
                case REG_QWORD:
                    return std::to_string(*(ULONGLONG*)data);
                case REG_MULTI_SZ: {
                    std::string result;
                    char* p = (char*)data;
                    while (*p) {
                        result += p;
                        result += "\n";
                        p += strlen(p) + 1;
                    }
                    return result;
                }
                case REG_BINARY: {
                    std::string result;
                    for (DWORD i = 0; i < size; i++) {
                        char buf[3];
                        sprintf(buf, "%02X", data[i]);
                        result += buf;
                        if (i < size - 1) result += " ";
                    }
                    return result;
                }
                default:
                    return "UNSUPPORTED_TYPE";
            }
        }

        /**
         * @brief 该函数为 读取 Hkey\path 下的所有项 是否存在 SpecialKey 这一项
         * 
         * @param Hkey 
         * @param path 
         * @param SpecialKey 
         * @example
         *  auto* particularKey = ParticularRegistryKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", "WallpaperEngine");
            if (!particularKey->first.empty()) {
                std::cout << "Registry path: " << particularKey->first << ", Value: " << particularKey->second << std::endl;
            }
            delete particularKey;
        * @return std::pair<std::string, std::string>* 第一个 std::string 返回注册表目录, 第二个 std::string 为键值
        */
        std::pair<std::string, std::string>* ParticularRegistryKey(HKEY Hkey, const char* path, const char* SpecialKey) {
            HKEY hKey;
            LONG lResult = RegOpenKeyEx(Hkey, path, 0, KEY_READ, &hKey);
            if (lResult != ERROR_SUCCESS) {
                return new std::pair<std::string, std::string>("", "");
            }

            BYTE data[1024];
            DWORD dataSize = sizeof(data);
            DWORD type;
            lResult = RegQueryValueEx(hKey, SpecialKey, NULL, &type, data, &dataSize);
            if (lResult != ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return new std::pair<std::string, std::string>("", "");
            }

            std::string value = std::string(reinterpret_cast<char*>(data));
            RegCloseKey(hKey);
            return new std::pair<std::string, std::string>(std::string(path), value);
        }
    };

    namespace Process{
        namespace Run{
            /**
             * @brief 结束 ImageName 所指的进程
             * 
             * @param ImageName 映像名字
             * @return true 
             * @return false 
             */
            bool KillProcess(const std::string& ImageName) {
                std::string lowerName = ImageName;
                std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

                HANDLE hProcessSnap;
                PROCESSENTRY32 pe32;

                hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                if (hProcessSnap == INVALID_HANDLE_VALUE) {
                    return false;
                }

                pe32.dwSize = sizeof(PROCESSENTRY32);

                if (!Process32First(hProcessSnap, &pe32)) {
                    CloseHandle(hProcessSnap);
                    return false;
                }

                do {
                    std::string processName = pe32.szExeFile;
                    std::transform(processName.begin(), processName.end(), processName.begin(), ::tolower);

                    if (lowerName == processName) {
                        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                        if (hProcess != NULL) {
                            TerminateProcess(hProcess, 0);
                            CloseHandle(hProcess);
                        }
                    }
                } while (Process32Next(hProcessSnap, &pe32));

                CloseHandle(hProcessSnap);
                return true;
            }

            /**
             * @brief 打开 file 所指的文件
             * 
             * @param file 
             * @return true
             */
            bool OpenApplication(const char* file){
                ShellExecute(NULL, "open", file, NULL, NULL, SW_SHOWNORMAL);
                return true;
            }

            /**
             * @brief Open An Application with Admin Privileges. / 以管理员权限打开某一可执行文件
             * 
             * @param file 
             * @return true
             * @return false
             */
            bool OpenApplicationAsAdmin(const char* file) {
                SHELLEXECUTEINFO shExecInfo = {};
                ZeroMemory(&shExecInfo, sizeof(SHELLEXECUTEINFO));
                shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
                shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
                shExecInfo.hwnd = NULL;
                shExecInfo.lpVerb = "runas"; // This is what makes it run as admin
                shExecInfo.lpFile = file;
                shExecInfo.lpParameters = NULL;
                shExecInfo.lpDirectory = NULL;
                shExecInfo.nShow = SW_NORMAL;
                shExecInfo.hInstApp = NULL;

                return ShellExecuteEx(&shExecInfo);
            }
        }
        namespace Monitor{
            /**
             * @brief 检测进程是否存在
             * 
             * @param processName 映像名字
             * @return true 
             * @return false 
             */
            bool IsProcessRunning(const char* processName) {
                HANDLE hProcessSnap;
                PROCESSENTRY32 pe32;

                hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                if (hProcessSnap == INVALID_HANDLE_VALUE) {
                    return false;
                }

                pe32.dwSize = sizeof(PROCESSENTRY32);

                if (!Process32First(hProcessSnap, &pe32)) {
                    CloseHandle(hProcessSnap);
                    return false;
                }

                do {
                    if (_stricmp(processName, pe32.szExeFile) == 0) {
                        CloseHandle(hProcessSnap);
                        return true;
                    }
                } while (Process32Next(hProcessSnap, &pe32));

                CloseHandle(hProcessSnap);
                return false;
            }

            /**
             * @brief Get the Running ProcessInfo Names object / 获得所有运行中的进程名
             * 
             * @return std::vector<std::string> 
             */
            std::vector<std::string> GetRunningProcessNames() {
                std::vector<std::string> processes;
                HANDLE hProcessSnap;
                PROCESSENTRY32 pe32;

                // 创建进程快照
                hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                if (hProcessSnap == INVALID_HANDLE_VALUE) {
                    return processes;
                }

                pe32.dwSize = sizeof(PROCESSENTRY32);

                // 获取第一个进程信息
                if (Process32First(hProcessSnap, &pe32)) {
                    do {
                        processes.emplace_back(pe32.szExeFile);
                    } while (Process32Next(hProcessSnap, &pe32));
                }

                CloseHandle(hProcessSnap);
                return processes;
            }

            /**
             * @brief 获取指定进程的命令行 不单独使用 为 GetRunningProcessWithCommandline() 函数的辅助函数
             * 
             * @param hProcess 
             * @return std::string 
             */
            std::string GetProcessCommandLine(HANDLE hProcess) {
                PROCESS_BASIC_INFORMATION pbi;
                NTSTATUS status = NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), nullptr);
                if (status != 0) {
                    return "";
                }

                // 读取 PEB 地址
                PEB peb;
                if (!ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(peb), nullptr)) {
                    return "";
                }

                // 读取进程参数块
                RTL_USER_PROCESS_PARAMETERS upp;
                if (!ReadProcessMemory(hProcess, peb.ProcessParameters, &upp, sizeof(upp), nullptr)) {
                    return "";
                }

                // 读取命令行
                UNICODE_STRING commandLine = upp.CommandLine;
                std::wstring wCommandLine(commandLine.Length / sizeof(wchar_t), L'\0');
                if (!ReadProcessMemory(hProcess, commandLine.Buffer, &wCommandLine[0], commandLine.Length, nullptr)) {
                    return "";
                }

                // 转换为 std::string
                std::string commandLineStr(wCommandLine.begin(), wCommandLine.end());
                return commandLineStr;
            }

            /**
             * @brief Get the Running ProcessInfo With Commandline object And PID / 返回进程的名字 命令行 及 PID
             * 
             * @return std::vector<ProcessInfo> 
             */
            std::vector<ProcessInfo> GetRunningProcessWithCommandline() {
                std::vector<ProcessInfo> processes;
                HANDLE hProcessSnap;
                PROCESSENTRY32 pe32;

                hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                if (hProcessSnap == INVALID_HANDLE_VALUE) {
                    return processes;
                }

                pe32.dwSize = sizeof(PROCESSENTRY32);

                if (Process32First(hProcessSnap, &pe32)) {
                    do {
                        ProcessInfo ProcessInfo;
                        ProcessInfo.Name = pe32.szExeFile;
                        ProcessInfo.pid = pe32.th32ProcessID;

                        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
                        if (hProcess != NULL) {
                            ProcessInfo.CommandLine = GetProcessCommandLine(hProcess);
                            CloseHandle(hProcess);
                        }

                        processes.push_back(ProcessInfo);
                    } while (Process32Next(hProcessSnap, &pe32));
                }

                CloseHandle(hProcessSnap);
                return processes;
            }
        }
    }

    int lcm(int a, int b){
        return a * b / gcd(a, b);
    }

    // 两个参数 分别是分子和分母 返回依然是两个数
    PII frac(int a, int b){
        int temp = gcd(a, b);
        a /= temp, b /= temp;
        PII result = {a, b};
        return result;
    }
	
    // 延时输出 
	void print(const std::string& out, int w_t){
		int wait_time = w_t;
		for (int i = 0; i < static_cast<int>(out.size()); i ++){
			if (GetAsyncKeyState(VK_RETURN)) wait_time = 0;
			if (out[i] != '\n') cout << out[i];
			else cout << '\n';
			Sleep(wait_time);
		}
	}

    /**
     * @brief 计算 a 的 b 次方
     * 
     * @param a 
     * @param b 
     * @return result
     */
    LL pow(LL a, LL b){
        if (a == 0 && b == 0){
            throw std::runtime_error("Syntax Error!");
            return 0;
        }
        if (b == 0 && a != 0){
            return 1;
        }

        LL res = 1;
        for (int i = 1; i <= b; i ++){
            res *= a;
        }

        return res;

    }
    
    // Double 类型的pow函数
    LD Dpow(LD a, LD b){
        if (a == 0 && b == 0){
            throw std::runtime_error("Syntax Error!");
            return 0;
        }
        if (b == 0 && a != 0){
            return 1;
        }

        LD res = 1;
        for (LD i = 1; i <= b; i ++){
            res *= a;
        }

        return res;
    }
    /**
     * @brief 打开 url(参数) 对应的网站
     * 
     * @param url 
     */
    void OpenWeb(const char* url){
        ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
    }

    /**
     * @brief 把此时的时间输出到 ofs 指定的文件中
     * 
     * @param ofs 
     */
    void OutputTime(std::ostream& ofs){
        std::time_t currentTime = std::time(nullptr);
        std::tm* localTime = std::localtime(&currentTime);

        std::string year = std::to_string(localTime -> tm_year + 1900);
        std::string month = std::to_string(localTime -> tm_mon + 1);
        std::string day = std::to_string(localTime -> tm_mday);
        std::string hour = std::to_string(localTime -> tm_hour);

        ofs << year << "/" << month << "/" << day << " " << hour << ":";

        int min = localTime -> tm_min;
        int sec = localTime -> tm_sec;

        std::string minute, second;

        if (min < 10)
            minute = "0";
        if (sec < 10)
            second = "0";

        minute += std::to_string(min); second += std::to_string(sec);

        ofs << minute << ":" << second; 
    }
        
    /**
     * @brief 把当前时间输出到控制台
     * 
     */
    void OutputTime(){
        std::time_t currentTime = std::time(nullptr);
        std::tm* localTime = std::localtime(&currentTime);

        std::cout << localTime -> tm_year + 1900 << "/" << localTime -> tm_mon + 1 << "/" << localTime -> tm_mday << " " << localTime -> tm_hour << ":";

        int min = localTime -> tm_min;
        int sec = localTime -> tm_sec;

        std::string minute, second;

        if (min < 10)
            minute = "0";
        if (sec < 10)
            second = "0";

        minute += std::to_string(min), second += std::to_string(sec);

        std::cout << minute << ":" << second; 
    }
	
    

    /**
     * @brief Catch Output of a Console Application / 捕捉一个命令行进程的控制台输出结果
     * 
     * @param command You can input the name of application. / 要捕捉结果的应用程序
     * @return std::vector<std::string>
     */
    std::vector<std::string> CatchResultOfConsolePlanA(const char* path) {
        std::vector<std::string> output;

        // 创建管道用于捕获子进程的输出
        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        HANDLE hChildStdoutRd, hChildStdoutWr;
        if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)) {
            std::cerr << "CreatePipe failed: " << GetLastError() << std::endl;
            return output;
        }

        // 确保读端不被继承
        if (!SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0)) {
            std::cerr << "SetHandleInformation failed: " << GetLastError() << std::endl;
            return output;
        }

        // 设置子进程的启动信息
        PROCESS_INFORMATION piProcInfo;
        STARTUPINFO siStartInfo;
        ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
        ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
        siStartInfo.cb = sizeof(STARTUPINFO);
        siStartInfo.hStdError = hChildStdoutWr;
        siStartInfo.hStdOutput = hChildStdoutWr;
        siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

        // 创建子进程
        BOOL bSuccess = CreateProcess(
            NULL,          // 应用程序名称 (如果为 NULL, 则使用命令行)
            (LPSTR)path,   // 命令行
            NULL,          // 进程安全属性
            NULL,          // 线程安全属性
            TRUE,          // 继承句柄
            0,             // 创建标志
            NULL,          // 环境变量
            NULL,          // 当前目录
            &siStartInfo,  // STARTUPINFO
            &piProcInfo    // PROCESS_INFORMATION
        );

        if (!bSuccess) {
            std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
            CloseHandle(hChildStdoutWr);
            CloseHandle(hChildStdoutRd);
            return output;
        }

        // 关闭不需要的句柄
        CloseHandle(hChildStdoutWr);

        // 读取子进程的输出
        const int BUFFER_SIZE = 4096;
        CHAR buffer[BUFFER_SIZE];
        DWORD dwRead;
        while (true) {
            // 从管道中读取数据
            bSuccess = ReadFile(hChildStdoutRd, buffer, BUFFER_SIZE, &dwRead, NULL);
            if (!bSuccess || dwRead == 0) {
                break;
            }

            // 将读取的数据保存到 vector 中
            std::string line(buffer, dwRead);
            output.push_back(line);
        }

        // 等待子进程退出
        WaitForSingleObject(piProcInfo.hProcess, INFINITE);

        // 关闭句柄
        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);
        CloseHandle(hChildStdoutRd);

        return output;
    }
    
    /**
     * @brief Catch Output of a Console Application / 捕捉一个命令行进程的控制台输出结果
     * 
     * @param command You can input the name of application. / 要捕捉结果的应用程序
     * @return std::vector<std::string>
     */
    std::vector<std::string> CatchResultOfConsolePlanB(const char* path) {
        std::vector<std::string> results;

        // 创建匿名管道
        HANDLE hReadPipe, hWritePipe;
        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0)) {
            std::cerr << "CreatePipe failed." << std::endl;
            return results;
        }

        // 设置启动信息
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.hStdError = hWritePipe;
        si.hStdOutput = hWritePipe;
        si.dwFlags |= STARTF_USESTDHANDLES;

        ZeroMemory(&pi, sizeof(pi));

        // 创建新进程
        if (!CreateProcessA(NULL, const_cast<char*>(path), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            std::cerr << "CreateProcess failed." << std::endl;
            CloseHandle(hReadPipe);
            CloseHandle(hWritePipe);
            return results;
        }

        // 关闭写入端句柄，因为子进程已经继承了它
        CloseHandle(hWritePipe);

        const int bufferSize = 4096;
        char buffer[bufferSize];
        DWORD bytesRead;

        // 读取管道中的输出
        while (ReadFile(hReadPipe, buffer, bufferSize - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string output(buffer);
            size_t pos = 0;
            while ((pos = output.find('\n')) != std::string::npos) {
                results.emplace_back(output.substr(0, pos));
                output.erase(0, pos + 1);
            }
            if (!output.empty()) {
                results.emplace_back(output);
            }
        }

        // 等待进程结束
        WaitForSingleObject(pi.hProcess, INFINITE);

        // 关闭句柄
        CloseHandle(hReadPipe);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return results;
    }

}
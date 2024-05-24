#include "Oxygen.h"

namespace Oxygen{   
    /**
     * @brief ���� n �Ľ׳�
     * 
     * @param n 
     * @return Unsigned long long
     */
    ULL fact(int n){
        if (n == 1)
            return 1;
        if (n == 0)
            return 0;
        return fact(n - 1) * n;
    }

    /**
     * @brief ����һ���� start �� end �������
     * 
     * @param start 
     * @param end 
     * @return int 
     */
    int random(int start, int end){
		int num = rand()%(end - start + 1) + start;
		return num;
	}

    ULL C(ULL a, ULL b){
        ULL ans = 1;
        if (a == b)
            return fact(a);
        if ((b - a) > a){
            for (int i = b - a + 1; i <= b; i ++)
                ans *= i;
                return ans / fact(a);
        }
        else if (b - a == a){
            int lower = 1, upper = 1;
            for (int i = 1; i <= a; i ++)
                lower *= i;

            for (int i = a + 1; i <= b; i ++)
                upper *= i;
            return upper / lower;
        }
        else{
            for (int i = a + 1; i <= b; i ++){
                ans *= i;
                return ans / fact(b - a);
            }
        }
		return ans;
    }

    ULL A(ULL a, ULL b){
        ULL ans = 1;
        if (a == b)
            return fact(a);
        for (int i = b - a + 1; i <= b; i ++){
            ans *= i;
        }
        return ans;
    }

    int lcm(int a, int b){
        return a * b / gcd(a, b);
    }

    // �������� �ֱ��Ƿ��Ӻͷ�ĸ ������Ȼ��������
    PII frac(int a, int b){
        int temp = gcd(a, b);
        a /= temp, b /= temp;
        PII result = {a, b};
        return result;
    }
	
    // ��ʱ��� 
	void print(std::string out, int w_t){
		int wait_time = w_t;
		for (int i = 0;i < out.size();i ++){
			if (GetAsyncKeyState(VK_RETURN)) wait_time = 0;
			if (out[i] != '\n') cout << out[i];
			else cout << '\n';
			Sleep(wait_time);
		}
	}

    // ����һЩ��ͨ����ɫ֮�� \
    ���� reseda ǳ��ɫ bluish ǳ��ɫ reddish ǳ��ɫ lavender ����ɫ FaintYellow ����ɫ
    void PrintColor(std::string type){
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);   
        int target = 0;
        if (type == "black")
            target = 0;
        else if (type == "blue")
            target = 1;
        else if (type == "green")
            target = 2;
        else if (type == "reseda") // ǳ��ɫ
            target = 3;
        else if (type == "red")
            target = 4;
        else if (type == "purple")
            target = 5;
        else if (type == "yellow")
            target = 6;
        else if (type == "white")
            target = 7;
        else if (type == "gray")
            target = 8;
        else if (type == "bluish"){ // ǳ��ɫ
            target = 9;
        }
        else if (type == "reddish"){ // ǳ��ɫ
            target = 12;
        }
        else if (type == "lavender"){ // ����ɫ
            target = 13;
        }
        else if (type == "FaintYellow"){ // ����ɫ
            target = 14;
        }
        else{ // δ֪��ɫȫ��������ɫ
            target = 15;
        }
        SetConsoleTextAttribute(hOut, target);
    } 
	
    /**
     * @brief �ж�һ�����Ƿ�Ϊ����
     * 
     * @param n 
     * @return true 
     * @return false 
     */
	bool is_prime(LL n){
        if (n <= 0)
            return false;
		LL temp = std::sqrt(n);
		for (LL i = 2; i <= temp; i ++ ){
			if (n % i == 0){
				return false;
			}
		}	
		return true;
	}

    /**
     * @brief ���� a �� b �η�
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
    
    // Double ���͵�pow����
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

    bool isminus(char a){
        if (a == '-')
            return true;
        return false;
    }

    bool isminus(std::string a){
        if (a == "-")
            return true;
        return false;
    }

    /**
     * @brief �� url(����) ��Ӧ����վ
     * 
     * @param url 
     */
    void OpenWeb(const char* url){
        HINSTANCE result = ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
    }

    // void OpenProcedure(char* file){
    //     // ���岢��ʼ�� PROCESS_INFORMATION �� STARTUPINFO �ṹ��
    //     PROCESS_INFORMATION pi;
    //     STARTUPINFO si;
    //     ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    //     ZeroMemory(&si, sizeof(STARTUPINFO));
    //     si.cb = sizeof(STARTUPINFO);

    //     // ��ִ���ļ���·���������в���
    //     LPCTSTR lpApplicationName = file;
    //     LPTSTR lpCommandLine = NULL;

    //     // ���� CreateProcess ���������½���
    //     BOOL bResult = CreateProcess(lpApplicationName, lpCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    //     if (bResult == FALSE){
    //         cout << "CreateProcess failed!" << endl;
    //         return;
    //     }

    //     // �ȴ��½��̽���
    //     // WaitForSingleObject(pi.hProcess, INFINITE);

    //     // �رս��̺��̵߳ľ��
    //     CloseHandle(pi.hProcess);
    //     CloseHandle(pi.hThread);

    // }

    /**
     * @brief �� notepad.exe �� file ��ָ���ļ�
     * 
     * @param file 
     */
    void OpenProcedure(char* file){
        ShellExecute(NULL, "open", "notepad.exe", file, NULL, SW_SHOW);
    }

    /**
     * @brief �� notepad.exe �� file ��ָ���ļ�
     * 
     * @param file 
     */
    void EditFile(char* file){
        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        std::string TempStr;
        TempStr.assign(file);

        LPSTR lpstr = reinterpret_cast<LPSTR>(file);

        if (CreateProcess("C:\\Windows\\System32\\notepad.exe", lpstr, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            cout << "Success to open " << TempStr;
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        } 
        else {
            std::cerr << "Failed to open " << TempStr;
        }
    }

    /**
     * @brief �� std::string ���͵�����ת��Ϊ char*
     * 
     * @param str 
     * @return char* 
     */
    char* StringToChar(std::string str) {
        // ��ȡconst char*���͵�����
        const char* data = str.c_str();

        // �������ݳ��Ȳ������µ�char*����
        size_t dataSize = std::strlen(data);
        char* newData = new char[dataSize + 1];

        // �������ݵ��µ�������
        std::strcpy(newData, data);

        // �����µ�����
        return newData;
    }

    LD check(LD a){
        if (a < percent90){
            return -1; // ˵��û�й�ϵ
        }
        else if (a > percent90 && a < percent95){
            return 90;
        }
        else if (a > percent95 && a < percent99){
            return 95;
        }
        else if (a > percent99 && a < percent999){
            return 99;
        }
        else if (a > percent999)
            return 99.9;
        else return -1; // �����ܴ������� ֻ�Ƿ�ֹWarning
    }
    
	bool isdigit(char a){
		if (a >= '0' && a <= '9')
			return true;
		return false;
	}
	
	bool isdigit(std::string str){
		int len = str.length();
		for (int i = 0; i < len; i ++ ){
			if (!Oxygen::isdigit(str[i]))
				return false;
		}
		
		return true;
	}
	
    void Average(){
        std::string input;
        int count = 0;
        LD data = 0, sum = 0;
        cout << "Entered Average Mode......" << endl;
        while (cin >> input, input != "end" && isdigit(input)){
        	data = stold(input);
        	sum += data;
        	count ++;
		}
		
		cout << "Average is: " << std::setprecision(6) << sum / count << ", Amount is: " << count << endl;
		
		cout << "Exited Average Mode......" << endl;
		return;
    }

    /**
     * @brief �Ѵ�ʱ��ʱ������� ofs ָ�����ļ���
     * 
     * @param ofs 
     */
    void OutputTime(std::ofstream& ofs){
        std::time_t currentTime = std::time(nullptr);
        std::tm* localTime = std::localtime(&currentTime);

        ofs << localTime -> tm_year + 1900 << "/" << localTime -> tm_mon + 1 << "/" << localTime -> tm_mday << " " << localTime -> tm_hour << ":";

        int min = localTime -> tm_min;
        int sec = localTime -> tm_sec;

        std::string minute, second;

        if (min < 10)
            minute = "0";
        if (sec < 10)
            second = "0";

        minute += std::to_string(min), second += std::to_string(sec);

        ofs << minute << ":" << second; 
    }

    /**
     * @brief �ѵ�ǰʱ�����������̨
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
     * @brief �� file ��ָ���ļ�
     * 
     * @param file 
     */
	void OpenApplication(const char* file){
		ShellExecute(NULL, "open", file, NULL, NULL, SW_SHOWNORMAL);
	}
    
    /**
     * @brief �� FileOrigin ������ FileDestination
     * 
     * @param FileOrigin Ҫ�������ļ�, Ҫ����·�����ļ���
     * @param FileDestination �������յ�, ͬ����·�����ļ���
     * @param IfCoveredWhenExist �� IfCoveredWhenExist Ϊ True ʱ, �� ��ͬ���ļ�����ʱ, ������, ��֮��Ȼ
     */
    void CopyFiles(const char* FileOrigin, const char* FileDestination, bool IfCoveredWhenExist){
        CopyFile(FileOrigin, FileDestination, IfCoveredWhenExist);
    }

    /**
     * @brief ���� ImageName ��ָ�Ľ���
     * 
     * @param ImageName ӳ������
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
     * @brief Set the Register Key In Current User object
     * 
     * @param subKey Registry Key Path
     * @param valueName Register Key Name
     * @param filePath value
     * @return true 
     * @return false 
     */
    bool SetRegisterKeyInCurrentUser(const char* subKey, const char* valueName, const char* filePath) {
        HKEY hKey;
        
        // �򿪻򴴽�ע�������
        long result = RegCreateKeyExA(HKEY_CURRENT_USER, subKey, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);
        if (result != ERROR_SUCCESS) {
            return false;  // ע������ʧ��
        }

        // д���ֵ����
        result = RegSetValueExA(hKey, valueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(filePath), static_cast<DWORD>(strlen(filePath) + 1));
        if (result != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return false;  // ע������ʧ��
        }

        // �ر�ע�������
        RegCloseKey(hKey);

        return true;  // �ɹ����ע������
    }

    /**
     * @brief Set the Registry Key In Local Machine object
     * 
     * @param subKey Registry Key Path
     * @param valueName Register Key Name
     * @param filePath value
     * @return true 
     * @return false 
     */
    bool SetRegistryKeyInLocalMachine(const char* subKey, const char* valueName, const char* filePath) {
        HKEY hKey;
        
        // �򿪻򴴽�ע�������
        long result = RegCreateKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);
        if (result != ERROR_SUCCESS) {
            return false;  // ע������ʧ��
        }

        // д���ֵ����
        result = RegSetValueExA(hKey, valueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(filePath), static_cast<DWORD>(strlen(filePath) + 1));
        if (result != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return false;  // ע������ʧ��
        }

        // �ر�ע�������
        RegCloseKey(hKey);

        return true;  // �ɹ����ע������
    }
    
    /**
     * @brief ��ȡ HKCU �е�ע����ֵ
     * 
     * @param path Ҫ��ȡ��ע����ֵ·��
     * @param name ����
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
     * @brief ��ȡ HKLM �е�ע����ֵ
     * 
     * @param path Ҫ��ȡ��ע����ֵ·��
     * @param name ����
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
     * @brief Get Location of procedure
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
     * @brief ��ע��� HKCU �� path ·������Ϊ name ��ע����ֵ��Ϊ value
     * 
     * @param path Ҫ�޸ĵ�ע����ֵ��·��
     * @param name ����
     * @param value �޸ĺ�ļ�ֵ
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
     * @brief ��ע��� HKLM �� path ·������Ϊ name ��ע����ֵ��Ϊ value
     * 
     * @param path Ҫ�޸ĵ�ע����ֵ��·��
     * @param name ����
     * @param value �޸ĺ�ļ�ֵ
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
     * @brief ɾ�� HKCU �� subKey ·���µ� valueName
     * 
     * @param subKey ·��
     * @param valueName ע����������
     * @return true 
     * @return false 
     */
    bool DeleteRegistryKeyInCurrentUser(const char* subKey, const char* valueName) {
        HKEY hKey;
        
        // ��ע�������
        long result = RegOpenKeyExA(HKEY_CURRENT_USER, subKey, 0, KEY_WRITE, &hKey);
        if (result != ERROR_SUCCESS) {
            return false;  // ע������ʧ��
        }

        // ɾ����ֵ����
        result = RegDeleteValueA(hKey, valueName);
        if (result != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return false;  // ע������ʧ��
        }

        // �ر�ע�������
        RegCloseKey(hKey);

        return true;  // �ɹ����ע������
    }

    /**
     * @brief ɾ�� HKLM �� subKey ·���µ� valueName
     * 
     * @param subKey ·��
     * @param valueName ע����������
     * @return true 
     * @return false 
     */
    bool DeleteRegistryKeyInLocalMachine(const char* subKey, const char* valueName) {
        HKEY hKey;
        
        // ��ע�������
        long result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, KEY_WRITE, &hKey);
        if (result != ERROR_SUCCESS) {
            return false;  // ע������ʧ��
        }

        // ɾ����ֵ����
        result = RegDeleteValueA(hKey, valueName);
        if (result != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return false;  // ע������ʧ��
        }

        // �ر�ע�������
        RegCloseKey(hKey);

        return true;  // �ɹ����ע������
    }

    /**
     * @brief ���������, path ���� HKCU �� HKLM
     * 
     * @param path ����Ϊ HKCU �� HKLM, �ֱ��� HKCU �� HKLM ��Ӽ�ֵ
     * @param name �����������
     * @param value ������ļ�ֵ
     * @return true 
     * @return false 
     */
    bool AddAutoRun(std::string path, std::string name, std::string value){
        bool result = false;
        if (path == "HKCU" || path == "HKEY_CURRENT_USER"){
            result = SetRegisterKeyInCurrentUser(AutoRun, name.c_str(), value.c_str());
        }
        else if (path == "HKLM" || path == "HKEY_LOCAL_MACHINE"){
            result = SetRegistryKeyInLocalMachine(AutoRun, name.c_str(), value.c_str());
        }

        return result;
    }

    /**
     * @brief ��� Explorer.exe �������� Ĭ�ϼ�ֵΪ 1
     * 
     * @param path ����Ϊ HKCU �� HKLM, �ֱ��� HKCU �� HKLM ��Ӽ�ֵ
     * @param name �����������
     * @return true 
     * @return false 
     */
    bool AddExplorerRestrict(std::string path, std::string name){
        bool result = false;
        if (path == "HKCU" || path == "HKEY_CURRENT_USER"){
            result = SetRegisterKeyInCurrentUser(AutoRun, name.c_str(), "1");
        }
        else if (path == "HKLM" || path == "HKEY_LOCAL_MACHINE"){
            result = SetRegistryKeyInLocalMachine(AutoRun, name.c_str(), "1");
        }
        return result;
    }

    /**
     * @brief ɾ��ָ�����ļ�
     * 
     * @param file ָ�����ļ�
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
     * @brief ��ʾ�� procedure ����ִ�� file ������,
     * 
     * Eg. ����˵����� D:\\Compiler.exe ִ�� "-f Tester.cpp", ��ô�Ϳ��Ե��� Command("D:\\Compiler.exe", " -f Tester.cpp");
     * 
     * @param procedure 
     * @param file 
     */
    void Command(const char* procedure, char *file){
        STARTUPINFO si = {sizeof(si)};
        PROCESS_INFORMATION pi;

        std::string TempStr;
        TempStr.assign(file);

        LPSTR lpstr = reinterpret_cast<LPSTR>(file);

        if (CreateProcess(procedure, lpstr, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)){
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }

    /**
     * @brief �������Ƿ����
     * 
     * @param processName ӳ������
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
	
	bool OpenApplicationAsAdmin(const char* file) {
		SHELLEXECUTEINFO shExecInfo = {0};
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

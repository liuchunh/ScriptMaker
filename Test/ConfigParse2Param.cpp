// ConfigParse.cpp + ParamDesign.cpp
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <format>
#include <unordered_map>
#include <fstream>

bool DebugInfo = true;
const std::string ConfigFileName = "config.cfg";
std::ifstream ReadConfig;

enum class CompileMode {
    Compile = 0,      // 默认：编译
    CheckSyntax = 1,  // 仅检查语法
};

class Param final {
public:
    // 返回非 const 引用，调用者可以直接修改配置。
    static Param& GetInstance() {
        static Param instance;
        return instance;
    }

    Param(const Param&) = delete;
    Param& operator=(const Param&) = delete;
    Param(Param&&) = delete;
    Param& operator=(Param&&) = delete;

    std::filesystem::path Path;
    std::optional<std::filesystem::path> ResourceFilePath;
    std::optional<std::string> CustomParamFile; // 编译时附加的自定义文件（.h、.cpp），从命令行获取。
    std::filesystem::path MediaPath = "C:\\Windows\\Media\\dm\\Windows Notify Messaging.wav";  // 媒体文件路径
    std::vector<std::string> Link;  // 从文件获取的链接库, 如 shell32, user32, gdi32 等
    std::string standard = "17";
    bool PlaySound = true;  // 是否播放声音
    bool AutoCurrectSyntax = true;  // 是否自动修正语法错误
    bool DryRun = false;  // 显示命令但不执行
    bool OutputGUIExecutable = false;
    bool EnableAllWarnings = false;
    bool SelfUse = false; // 是否为自用模式/添加自用头文件
    std::string OptimizeLevel = "0"; // 优化等级，字符串形式，便于从配置文件读取
    CompileMode Mode = CompileMode::Compile;

private:
    // 禁止外部构造和销毁，统一由 GetInstance 管理生命周期。
    Param() = default;
    ~Param() = default;
};

std::string BoolAlpha(bool value) {
    return value ? "true" : "false";
}

template<>
struct std::formatter<Param> : std::formatter<std::string>{
    auto format(const Param& p, std::format_context& ctx) const {
        std::string result = std::format("[Path: {}", p.Path.string());
        if (p.ResourceFilePath.has_value()) {
            result += std::format(", ResourceFilePath: {}", p.ResourceFilePath->string());
        }
        if (p.CustomParamFile.has_value()) {
            result += std::format(", CustomParamFile: {}", p.CustomParamFile.value());
        }
        if (!p.MediaPath.empty()) {
            result += std::format(", MediaPath: {}", p.MediaPath.string());
        }
        if (!p.Link.empty()) {
            std::string accumulateLink;
            int index = 0;
            for (const auto& link : p.Link) {
                if (index > 0){
                    accumulateLink += ", ";
                }
                accumulateLink += link;
                index ++;
            }
            result += std::format(", Link: [{}]", accumulateLink);
        }
        result += std::format(", standard: C++{}", p.standard);
        result += std::format(", PlaySound: {}", BoolAlpha(p.PlaySound));
        result += std::format(", AutoCurrectSyntax: {}", BoolAlpha(p.AutoCurrectSyntax));
        result += std::format(", DryRun: {}", BoolAlpha(p.DryRun));
        result += std::format(", OutputGUIExecutable: {}", BoolAlpha(p.OutputGUIExecutable));
        result += std::format(", EnableAllWarnings: {}", BoolAlpha(p.EnableAllWarnings));
        result += std::format(", OptimizeLevel: O{}", p.OptimizeLevel);
        result += std::format(", Mode: {}", p.Mode == CompileMode::Compile ? "Compile" : "CheckSyntax");
        result += "]";
        return std::formatter<std::string>::format(result, ctx);
    }
};


/**
 * @brief 删除字符串前后的空白字符
 * 
 * @param str 
 * @return std::string_view 
 */
std::string_view trim(std::string_view str){
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string_view::npos) {
        return ""; // 字符串全是空白字符
    }
    str = str.substr(start);

    size_t end = str.find_last_not_of(" \t\n\r");
    if (end == std::string_view::npos) {
        return str; // 字符串没有尾部空白字符
    }
    return str.substr(0, end + 1);
}

std::pair<std::string, std::string> ParseConfigLine(std::string_view line){
    std::string_view copy = line;
    std::pair<std::string, std::string> result{};
    // 删除注释
    const size_t comment = line.find('#');
    line = line.substr(0, comment);

    // 找到 '=' 
    const size_t equal = line.find('=');
    if (equal == std::string_view::npos) {
        // 未找到 '='，返回空的键值对
        if (DebugInfo){
            std::cout << std::format("[WARN] In Function {}: When parsing '{}', '=' undetected.", __func__, copy) << std::endl;
        }
        return result;
    }

    std::string_view key = trim(line.substr(0, equal));
    std::string_view value = trim(line.substr(equal + 1));

    if (key.empty()){
        if (DebugInfo){
            std::cout << std::format("[WARN] In Function {}: When parsing '{}', Key is empty.", __func__, copy) << std::endl;
        }
        return result;
    }

    if (value.empty()){
        if (DebugInfo){
            std::cout << std::format("[WARN] In Function {}: When parsing '{}', Value is empty.", __func__, copy) << std::endl;
        }
        return result;
    }

    // 当确认键和值都不为空时，将它们存储到结果中
    result.first = key;
    result.second = value;
    return result;
}

std::string toLower(const std::string& str) {
    std::string lowerStr(str);
    for (char& c : lowerStr) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return lowerStr;
}

bool OptionEnable(const std::string& value){
    std::string lowerValue = toLower(value);
    return (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes" || lowerValue == "on" || lowerValue == "y");
}

bool Storage(const std::string& key, const std::string& value){
    if (key.empty() || value.empty()){
        if (DebugInfo){
            std::cout << std::format("[WARN] In Function {}({}, {}): Key or Value is empty, failed to store.", __func__, key, value) << std::endl;
        }
        return false;
    }
    
    Param& param = Param::GetInstance();

    std::string lowerKey = toLower(key);
    std::string lowerValue = toLower(value);

    // 根据键的不同，将值存储到 Param 实例中
    // Supported Key in options.txt: gccPath, windresPath, playSound, autoCurrectSyntax, mediaPath, dryRun, standard, allWarnings
    if (lowerKey == "gccpath") {
        param.Path = std::filesystem::path(value);
    }
    else if (lowerKey == "windrespath"){
        param.ResourceFilePath = std::filesystem::path(value);
    }
    else if (lowerKey == "mediapath"){
        param.MediaPath = std::filesystem::path(value);
    }
    else if (lowerKey == "playsound"){
        param.PlaySound = OptionEnable(value);
    }
    else if (lowerKey == "autoCurrectsyntax"){
        param.AutoCurrectSyntax = OptionEnable(value);
    }
    else if (lowerKey == "dryrun"){
        // 这里的 Debug 有两种
        // 一种是在全局 DebugInfo 变量中, 用于显示更多运行时的细节
        // 还有一种是 DryRun, 用于显示命令但不执行
        param.DryRun = OptionEnable(value);
    }
    else if (lowerKey == "standard"){
        param.standard = value;
    }
    else if (lowerKey == "allwarnings"){
        param.EnableAllWarnings = OptionEnable(value);
    }
    else {
        if (DebugInfo){
            std::cout << std::format("[WARN] In Function {}: Unsupported key '{}', failed to store.", __func__, key) << std::endl;
        }
        return false;
    }

    return true;
}

int main(){
    std::filesystem::path path = std::filesystem::current_path() / ConfigFileName;
    
    if (!std::filesystem::exists(path)) {
        std::cerr << "Config file not found: " << path << std::endl;
        return 1;
    }

    ReadConfig.open(path.string());

    std::string line;

    while (std::getline(ReadConfig, line)) {
        auto [key, value] = ParseConfigLine(line);
        if (!key.empty() && !value.empty()) {
            std::cout << std::format("Key: |{}|, Value: |{}|\n", key, value);

            Storage(key, value);            
        }
    }

    ReadConfig.close();

    std::cout << std::format("Parsed Config: {}\n", Param::GetInstance());

    return 0;
}
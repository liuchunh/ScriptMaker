// commandLineParse.cpp
// Parse Command Line arguments
// 2026.11.12-13
#include "cmdline.h"
#include <filesystem>
#include <optional>
#include <format>

const std::string ProgramName = "cpc.exe"; // Abbr.
const std::string Version = "1.3";

enum class CompileMode {
    Compile = 0,      // 默认：编译
    CheckSyntax = 1,  // 仅检查语法
};

enum class ParseResult{
    Success = 0,
    ShowHelp,
    ShowVersion,
    Error
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
    std::string OptimizeLevel = "0";
    CompileMode Mode = CompileMode::Compile;

    ParseResult ParseArguments(int argc, char* argv[]){
        cmdline::parser parser;
        parser.set_program_name(argc > 0 ? argv[0] : ProgramName);
        parser.add<std::string>("file", 'f', "输入的 C++ 源文件", true);
        parser.add<std::string>("standard", 's', "C++ 标准版本 (11, 14, 17, 20, 23)", false, "17", cmdline::oneof<std::string>("11", "14", "17", "20", "23"));
        parser.add<std::string>("resource", 'r', "资源文件路径 (.rc)", false, std::string{});
        parser.add<std::string>("custom", 'i', "自定义 C++ 或头文件", false, std::string{});
        parser.add<std::string>("optimize", 'o', "优化等级 (0-3)", false, "0", cmdline::oneof<std::string>("0", "1", "2", "3"));
        parser.add("check-syntax", 'c', "仅检查语法，不生成可执行文件");
        parser.add("dry-run", 'd', "显示命令但不执行");
        parser.add("gui", 'm', "生成图形界面可执行文件/GUI 程序");
        parser.add("wall", 'w', "启用所有警告");
        parser.add("version", 'v', "显示版本信息");
        parser.add("help", 'h', "显示帮助信息");

        const bool ParseSuccess = parser.parse(argc, argv);

        if (parser.exist("help")) {
            std::cout << parser.usage() << std::endl;
            return ParseResult::ShowHelp;
        }

        if (parser.exist("version")) {
            std::cout << "Compiler version " << Version << std::endl;
            return ParseResult::ShowVersion;
        }

        if (!ParseSuccess) {
            std::cerr << "Error: " << parser.error_full() << std::endl;
            std::cerr << parser.usage() << std::endl;
            return ParseResult::Error;
        }

        Path = parser.get<std::string>("file");
        if (Path.empty()) {
            std::cerr << "Error: 输入的 C++ 源文件不能为空" << std::endl;
            return ParseResult::Error;
        }
        if (!std::filesystem::exists(Path)) {
            std::cerr << "Error: 输入的 C++ 源文件不存在: " << Path << std::endl;
            return ParseResult::Error;
        }
        if (!std::filesystem::is_regular_file(Path)) {
            std::cerr << "Error: 输入的 C++ 源文件路径无效: " << Path << std::endl;
            return ParseResult::Error;
        }

        standard = parser.get<std::string>("standard");
        OptimizeLevel = parser.get<std::string>("optimize");
        Mode = parser.exist("check-syntax") ? CompileMode::CheckSyntax : CompileMode::Compile;
        DryRun = parser.exist("dry-run");
        OutputGUIExecutable = parser.exist("gui");
        EnableAllWarnings = parser.exist("wall");
        const auto& resource = parser.get<std::string>("resource");
        if (!resource.empty()) {
            ResourceFilePath = resource;
        }
        else{
            ResourceFilePath = std::nullopt;
        }

        const auto& custom = parser.get<std::string>("custom");
        if (!custom.empty()) {
            CustomParamFile = custom;
        }
        else{
            CustomParamFile = std::nullopt;
        }

        return ParseResult::Success;
    }
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
        std::string result = std::format("[Path: {}", std::filesystem::absolute(p.Path.string()).string() );
        if (p.ResourceFilePath.has_value()) {
            result += std::format(", ResourceFilePath: {}", std::filesystem::absolute( p.ResourceFilePath->string() ).string() );
        }
        if (p.CustomParamFile.has_value()) {
            result += std::format(", CustomParamFile: {}", std::filesystem::absolute( p.CustomParamFile.value() ).string() );
        }
        if (!p.MediaPath.empty()) {
            result += std::format(", MediaPath: {}", std::filesystem::absolute(p.MediaPath.string()).string() );
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

int main(int argc, char* argv[]) {
    Param& param = Param::GetInstance();
    ParseResult result = param.ParseArguments(argc, argv);

    switch (result) {
        case ParseResult::ShowHelp:
        case ParseResult::ShowVersion:{
            return 0;
        }
        case ParseResult::Error:{
            return 1;
        }
        case ParseResult::Success:
            break;
        default:{
            std::cerr << "Unknown parse result." << std::endl;
            return 1;
        }
    }

    std::cout << std::format("Result: {}", param) << std::endl;
    
    return 0;
}
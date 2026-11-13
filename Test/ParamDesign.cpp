#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <format>

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
    std::vector<std::string> Link;  // 从文件获取的链接库, 如 shell32, user32, gdi32 等
    std::string standard = "17";
    bool DryRun = false;  // 显示命令但不执行
    bool OutputGUIExecutable = false;
    bool EnableAllWarnings = false;
    bool SelfUse = false; // 是否为自用模式/添加自用头文件
    int OptimizeLevel = 0;
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
        result += std::format(", DryRun: {}", BoolAlpha(p.DryRun));
        result += std::format(", OutputGUIExecutable: {}", BoolAlpha(p.OutputGUIExecutable));
        result += std::format(", EnableAllWarnings: {}", BoolAlpha(p.EnableAllWarnings));
        result += std::format(", OptimizeLevel: O{}", p.OptimizeLevel);
        result += std::format(", Mode: {}", p.Mode == CompileMode::Compile ? "Compile" : "CheckSyntax");
        result += "]";
        return std::formatter<std::string>::format(result, ctx);
    }
};

int main() {
    // 注意保留引用符号 &，这样得到的是可修改的单例引用。
    // 写成 auto param = Param::GetInstance() 会尝试复制，无法编译。
    auto& param = Param::GetInstance();
    param.Path = "example.cpp";
    param.ResourceFilePath = std::filesystem::path("app.rc");
    param.CustomParamFile = "extra.h";
    param.Link.push_back("user32");
    param.Link.push_back("gdi32");
    param.Link.push_back("shell32");
    param.standard = "20";
    param.EnableAllWarnings = true;
    param.OptimizeLevel = 2;
    param.Mode = CompileMode::Compile;

    std::cout << std::format("Result Param: {}\n", param);

    // 也可以显式写成 Param&，两次调用得到同一个对象。
    Param& sameParam = Param::GetInstance();
    sameParam.DryRun = true;

    std::cout << std::format("Result Param: {}\n", param);

    return 0;
}

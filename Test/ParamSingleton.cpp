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

// C++17 单例类，保留原结构体中的公开配置成员。
class Param final {
public:
    // 返回非 const 引用，调用者可以直接修改配置。
    static Param& GetInstance() {
        // 从 C++11 开始，局部静态变量的初始化是线程安全的。
        // 初始化完成后，对配置的并发读写仍需调用者自行同步。
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
    std::vector<std::string> Link; // 从文件获取的链接库
    std::string standard = "17";
    bool DryRun = false;  // 显示命令但不执行
    bool OutputGUIExecutable = false;
    bool EnableAllWarnings = false;
    int OptimizeLevel = 0;
    CompileMode Mode = CompileMode::Compile;

private:
    // 禁止外部构造和销毁，统一由 GetInstance 管理生命周期。
    Param() = default;
    ~Param() = default;
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

    // 也可以显式写成 Param&，两次调用得到同一个对象。
    Param& sameParam = Param::GetInstance();
    sameParam.DryRun = true;

    std::cout << std::boolalpha
              << "Same instance: " << (&param == &sameParam) << '\n'
              << "C++ standard: " << param.standard << '\n'
              << "Dry run: " << param.DryRun << '\n';
    return 0;
}

# Compile Script Maker

## Release Version 1.2.1.1

`GCC` Compile Script Maker

这是一个命令行程序, 它可以根据你的输入提供对应的编译脚本

支持使用 `options.txt` 进行配置

目前支持的配置有 (不区分大小写)

`CompilerPath` -> `gcc 编译器地址` **需要是字符串**

`WindresPath` -> `windres (编译资源文件) 编译器地址` **需要是字符串**

`RunSound` -> `是否在编译完成后播放 MediaPath 中的提示音` **bool 类型, true 或 false**

`MediaPath` -> `在 RunSound 为 true 的情况下, 编译完成后播放的提示音` **字符串**

`AutoCurrectSyntax` -> `在 .cpp 后缀未检测到时进行后缀的自动修正` **true or false**

`AdditionalOption` -> `编译时额外添加的编译项, 这里写的会在每一次编译时都带上` **字符串, Eg. -ntdll**

可以在 `Output-cmdline.log` 中看到本程序的日志

不知道怎么写的话可以使用 `Compiler.exe --help` 查看相关选项

#### Update. 2026.9.2
1. 代码结构优化, 使用了更多现代 C++ 特性

2. 新加了 `--config` 参数, 使用后可以打开程序读取的配置文件 `options.txt`

<!-- 嗯对我是笨蛋 -->
# v1.3 Creating..

Current Project Structure:
- src
    - cc.cpp -> for gcc script generator.
    - cmdline.h -> https://github.com/tanakh/cmdline
    - Compiler_Cmdline.cpp -> **NOT** updated. Old version of scriptmaker.
    - MakeFile -> Executable Generator.
    - Oxygen.cpp -> Custom Lib Source Code. Will be **ABANDONED**.
    - Oxygen.h -> Custom Header File. Will be **ABANDONED**.
- Test
    - cmdline.h -> Same as before.
    - commandLineParse.cpp -> Parse Input Command to `param`
    - ConfigParse.cpp -> Read From `config.cfg` to get user's custom config.
    - ConfigParse2Param.cpp -> Based on `ConfigParse.cpp`, But applied result to `Param Instance`.
    - filesystemTest.cpp -> Just an attempt on `std::filesystem`.
    - ParamDesign.cpp, ParamSingleton.cpp -> Use singleton mode to design param result.
    - ProcessRunner.cpp -> wtf is this, just a `ShellExecuteW` usage adventure.
- README.md -> EVERYONE KNOWS WHAT THE FK IS THIS.
- .gitignore -> ? Why asking me. Try chatgpt.
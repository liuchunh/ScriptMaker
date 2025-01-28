# Compile Script Maker

## Release Version 1.1.4.12

### zh-cn

这是一个可以生成编译脚本的程序

是一个控制台程序，所以需要先运行 `cmd.exe` 或者 `powershell.exe` 再运行

如果是 `Mingw g++` 编译器的话可以使用，`MSVC` 建议使用 `Visual Studio`

必须要配置 `options.txt`！！！

下面有 `options.txt` 的样例

更多内容请输入 `Compiler.exe -help` 来查看

日志会输出到 `log.txt` 中

如果你不想编译的话 `Release` 中有编译好的，我将它打包成了 7z 的自解压程序

两个额外的 dll 文件是我的编译器编译时链接的 必须要带

~~说真的应该用在oi上好吧awa~~

### en-us

~~Translated by myself~~

**It is A Compile Script Maker**

**It is a Console Application, so you have to run cmd.exe or powershell.exe firstly!**

It can help you spawn compile script if you use `Mingw g++`

You have to configure `options.txt`

For example:

```
WindresPath=D:\\Mingw64\\bin\\Windres.exe
CompilerPath=D:\\Mingw64\\bin\\g++.exe
PlaySound=true
AutoCurrectSyntax=true
```

Replace the content behind `=` with your own path

------------

Here are some instances:

**1.**

**`Compiler.exe -f TestFile.cpp -s 14 -o 2 -l -h -w -a -n -e -d`**

**`= g++ TestFile.cpp -o2 -o TestFile.exe -std=c++14 -leasyx -mwindows -Wall -lshlwapi -lwinmm -lole32 -lgdi32`**

**2.**

**`Compiler.exe -f TestFile.cpp -c`**

**`= g++ TestFile.cpp -o TestFile.exe -fsyntax-only`**

**3.**

**`3.  Compiler.exe -f TestFile.cpp -r resource.rc`**

**`= windres resource.rc -O coff resource.o && g++ TestFile.cpp resource.o -o TestFile.exe`**

```
// Instance 4 means that you can use your own header files
```

**`4.  Compiler.exe -f TestFile.cpp -i Oxygen.cpp`**

**`= g++ TestFile.cpp Oxygen.cpp -o TestFile.exe`**

**`5. Compiler.exe -help`**

------------

You can see more information in `log.txt`

You can download executive files in `Releases`

It is a 7-Zip Self Extracter.

Two extra dlls are necessities.

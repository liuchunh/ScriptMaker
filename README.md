# ScriptMaker

**A Compile Script Maker**

It can help you spawn compile script if you use `Mingw g++`

You have to configure `options.txt`

For example:

```
WindresPath=D:\\Mingw64\\bin\\Windres.exe
CompilerPath=D:\\Mingw64\\bin\\g++.exe
PlaySound=true
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

**`4.  Compiler.exe -f TestFile.cpp -i Oxygen.cpp`**

**`= g++ TestFile.cpp Oxygen.cpp -o TestFile.exe`**

**`5. Compiler.exe -help`**

------------

You can see more information in `log.txt`

You can download [here]([https://luogu.oss-cn-hangzhou.aliyuncs.com/upload/problem_attachment/5trp52p5.exe?response-content-disposition=attachment%3B%20filename%3D%22CompilerSelfExtracter.exe%22&OSSAccessKeyId=LTAI4FsiWjpNs1epYQp3d1Ag&Expires=1716517149&Signature=WfgQT6ageatujO9J3fhQ6HuLLPY%3D](https://luogu.oss-cn-hangzhou.aliyuncs.com/upload/problem_attachment/7dqumbsa.exe?response-content-disposition=attachment%3B%20filename%3D%22CompilerSelfExtracter.exe%22&OSSAccessKeyId=LTAI4FsiWjpNs1epYQp3d1Ag&Expires=1716517358&Signature=VSWxArlmzl5ucIxmbXbRnd5joLA%3D))

It is a 7-Zip Self Extracter.

Two extra dlls are necessities.

`Sha1: d3d6fc617dd7ed536294786a6c4fbd319babe0bf` (x64)

`Sha256: b77e2cfed680a930acbbc6fbcdf623085000b8481ae8a0a9d845e716d89f4db2` (x64)

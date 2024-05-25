# Compile Script Maker

**It is A Compile Script Maker**

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

`Sha1: d3d6fc617dd7ed536294786a6c4fbd319babe0bf` (x64)

`Sha256: b77e2cfed680a930acbbc6fbcdf623085000b8481ae8a0a9d845e716d89f4db2` (x64)

# 编译器配置
CXX = D:\mingw64\bin\g++.exe
WINDRES = D:\mingw64\bin\windres.exe

# 编译标准
CXX_STANDARD = -std=c++20

# 链接库
LDFLAGS = -lwinmm -lshlwapi -lole32 -luuid -lgdi32 -lcomdlg32 -lpsapi -lntdll

# 编译选项
CXXFLAGS = $(CXX_STANDARD) -Wall -O2 -Wextra

# 目标文件
TARGET = cpc.exe
TARGET_DIR = D:\Executive\OvO

# 源文件
CPP_SOURCES = Compiler_Cmdline.cpp Oxygen.cpp
HEADERS = cmdline.h Oxygen.h
RESOURCES = resource.rc

# 目标文件（编译生成的.o文件）
CPP_OBJECTS = $(CPP_SOURCES:.cpp=.o)
RES_OBJECTS = $(RESOURCES:.rc=.o)

# 所有目标文件
OBJECTS = $(CPP_OBJECTS) $(RES_OBJECTS)

# 最终目标
all: $(TARGET)

# 链接生成可执行文件
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# 编译 C++ 源文件
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译资源文件
%.o: %.rc
	$(WINDRES) $< -o $@

# 拷贝到目标目录
install: $(TARGET)
	@if not exist "$(TARGET_DIR)" mkdir "$(TARGET_DIR)"
	copy /Y $(TARGET) "$(TARGET_DIR)\$(TARGET)"

# 清理编译产物
clean:
	del /Q *.o $(TARGET) 2>nul

# 完整构建并安装
build: clean all install

.PHONY: all install clean build
SHELL := cmd.exe
.SHELLFLAGS := /C

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
OBJ_DIR = ../resource
BIN_DIR = ../Binary
TARGET_NAME = cpc.exe
TARGET = $(BIN_DIR)/$(TARGET_NAME)
TARGET_DIR = D:\Executive\OvO

# 源文件
CPP_SOURCES = Compiler_Cmdline.cpp Oxygen.cpp
HEADERS = cmdline.h Oxygen.h
RESOURCES = $(OBJ_DIR)/resource.rc

# 目标文件（编译生成的.o文件）
CPP_OBJECTS = $(addprefix $(OBJ_DIR)/,$(CPP_SOURCES:.cpp=.o))
RES_OBJECTS = $(RESOURCES:.rc=.o)

# 所有目标文件
OBJECTS = $(CPP_OBJECTS) $(RES_OBJECTS)

# 最终目标
all: $(TARGET)

# 链接生成可执行文件
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) -o "$@" $(LDFLAGS)

# 编译 C++ 源文件
$(OBJ_DIR)/%.o: %.cpp $(HEADERS) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c "$<" -o "$@"

# 编译资源文件
$(OBJ_DIR)/%.o: $(OBJ_DIR)/%.rc | $(OBJ_DIR)
	$(WINDRES) "$<" -o "$@"

# 创建输出目录（仅在目录不存在时执行）
$(OBJ_DIR) $(BIN_DIR):
	@if not exist "$(subst /,\,$@)" mkdir "$(subst /,\,$@)"

# 拷贝到目标目录
install: $(TARGET)
	@if not exist "$(TARGET_DIR)" mkdir "$(TARGET_DIR)"
	copy /Y "$(subst /,\,$(TARGET))" "$(TARGET_DIR)\$(TARGET_NAME)"

# 清理编译产物
clean:
	del /Q "$(subst /,\,$(OBJ_DIR))\*.o" "$(subst /,\,$(TARGET))" 2>nul

# 完整构建并安装
build: clean all install

.PHONY: all install clean build
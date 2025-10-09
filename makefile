# Makefile for Typinger C++ project
# ---------------------------------
# Use g++ from MSYS2 / MinGW-w64

# Compiler
CXX := g++
CXXFLAGS := -Wall -O2 -std=c++17

# Target executable
TARGET := main.exe

# Source files
SRCS := main.cpp 

# Object files
OBJS := $(SRCS:.cpp=.o) helper/WinAPI/terminal.o helper/json_helper.o


# Default target
all: tmpdir $(TARGET)

# tmpdir: プロジェクト内のtmpディレクトリを作成
tmpdir:
	mkdir -p tmp


# Link
$(TARGET): $(OBJS)
	TMPDIR=./tmp $(CXX) $(CXXFLAGS) -o $@ $^


# Compile
%.o: %.cpp
	TMPDIR=./tmp $(CXX) $(CXXFLAGS) -c $< -o $@


# Clean
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf tmp

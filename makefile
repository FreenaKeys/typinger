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
OBJS := $(SRCS:.cpp=.o) helper/WinAPI/terminal.o

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJS) $(TARGET)

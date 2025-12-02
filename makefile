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
OBJS := $(SRCS:.cpp=.o) helper/WinAPI/terminal.o helper/WinAPI/timer.o helper/json_helper.o core/input_recorder.o core/romaji_converter.o core/typing_judge.o core/statistics.o core/csv_logger.o


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
 
# Tests
typing-test: tests/typing_judge_test.cpp core/typing_judge.o
	TMPDIR=./tmp $(CXX) $(CXXFLAGS) -o typing_judge_test.exe $^

romaji-test: tests/romaji_converter_test.cpp core/romaji_converter.o
	TMPDIR=./tmp $(CXX) $(CXXFLAGS) -o romaji_converter_test.exe $^

statistics-test: tests/statistics_test.cpp core/statistics.o
	TMPDIR=./tmp $(CXX) $(CXXFLAGS) -o statistics_test.exe $^

csv-logger-test: tests/csv_logger_test.cpp core/csv_logger.o core/input_recorder.o helper/WinAPI/timer.o
	TMPDIR=./tmp $(CXX) $(CXXFLAGS) -o csv_logger_test.exe $^

experiment-session-test: tests/experiment_session_test.cpp core/experiment_session.o
	TMPDIR=./tmp $(CXX) $(CXXFLAGS) -o experiment_session_test.exe $^


#include "terminal.h"
#include <windows.h>
#include <iostream>
#include <string>

namespace Terminal {
    TerminalSize getTerminalSize() {
        int width, height;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
            width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        } else {
            width = 0;
            height = 0;
        }
        return TerminalSize{ width, height };
    }

    // カーソル位置を指定して文字列を上書き
    void overwriteString(int x, int y, const std::string& str) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD pos = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
        SetConsoleCursorPosition(hConsole, pos);
        DWORD written;
        WriteConsoleA(hConsole, str.c_str(), static_cast<DWORD>(str.size()), &written, nullptr);
    }

    // 画面をリセット（クリア）
    void clearScreen() {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        DWORD consoleSize = csbi.dwSize.X * csbi.dwSize.Y;
        DWORD written;
        COORD topLeft = { 0, 0 };
        FillConsoleOutputCharacter(hConsole, ' ', consoleSize, topLeft, &written);
        FillConsoleOutputAttribute(hConsole, csbi.wAttributes, consoleSize, topLeft, &written);
        SetConsoleCursorPosition(hConsole, topLeft);
    }
    std::string Value_to_Blank(int value, const std::string& str) {
        std::string result;
        for (int i = 0; i < value; ++i) {
            result += str;
        }
        return result;
    }
}

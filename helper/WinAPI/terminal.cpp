#include "terminal.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

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

    // 文字列の表示幅を計算（全角2、半角1）
    int getDisplayWidth(const std::string& str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        std::wstring wstr = conv.from_bytes(str);
        int width = 0;
        for (wchar_t wc : wstr) {
            // 全角判定（簡易版）
            if ((wc >= 0x1100 && wc <= 0x115F) ||
                (wc >= 0x2E80 && wc <= 0xA4CF) ||
                (wc >= 0xAC00 && wc <= 0xD7A3) ||
                (wc >= 0xF900 && wc <= 0xFAFF) ||
                (wc >= 0xFE10 && wc <= 0xFE19) ||
                (wc >= 0xFE30 && wc <= 0xFE6F) ||
                (wc >= 0xFF00 && wc <= 0xFF60) ||
                (wc >= 0xFFE0 && wc <= 0xFFE6)) {
                width += 2;
            } else {
                width += 1;
            }
        }
        return width;
    }

    // 指定位置に部分的に文字列を上書き
    void overwritePartial(int x, int y, const std::string& str) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD pos = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
        SetConsoleCursorPosition(hConsole, pos);

        // UTF-8→UTF-16変換（日本語対応）
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        std::wstring wstr = conv.from_bytes(str);

        DWORD written;
        WriteConsoleW(hConsole, wstr.c_str(), static_cast<DWORD>(wstr.size()), &written, nullptr);
    }
}

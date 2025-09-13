#pragma once
#include <string>


namespace Terminal {
    struct TerminalSize {
        int width;
        int height;
    };

    TerminalSize getTerminalSize();
    void overwriteString(int x, int y, const std::string& str);
    void SetConsoleCursorPosition(int x, int y);
    void clearScreen();
    std::string Value_to_Blank(int value, const std::string& str);
    int getDisplayWidth(const std::string& str);
}

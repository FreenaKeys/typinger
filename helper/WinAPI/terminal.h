#pragma once
#include <string>


namespace Terminal {
    struct TerminalSize {
        int width;
        int height;
    };

    // 色定数（Windows Console Color Attributes）
    enum Color {
        BLACK = 0,
        BLUE = 1,
        GREEN = 2,
        CYAN = 3,
        RED = 4,
        MAGENTA = 5,
        YELLOW = 6,
        WHITE = 7,
        GRAY = 8,
        LIGHT_BLUE = 9,
        LIGHT_GREEN = 10,
        LIGHT_CYAN = 11,
        LIGHT_RED = 12,
        LIGHT_MAGENTA = 13,
        LIGHT_YELLOW = 14,
        BRIGHT_WHITE = 15
    };

    TerminalSize getTerminalSize();
    void overwriteString(int x, int y, const std::string& str);
    void SetConsoleCursorPosition(int x, int y);
    void clearScreen();
    std::string Value_to_Blank(int value, const std::string& str);
    int getDisplayWidth(const std::string& str);
    
    // 色設定関数
    void setTextColor(Color foreground, Color background = BLACK);
    void resetTextColor();
}

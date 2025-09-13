#include "terminal.h"
#include <windows.h>
#include <iostream>

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
}

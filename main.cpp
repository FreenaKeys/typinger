#include <iostream>
#include "helper/WinAPI/terminal.h"
int main() {
    std::cout << "Hello, World!" << std::endl;
    
    auto size = Terminal::getTerminalSize();
    std::cout << "Width: " << size.width << ", Height: " << size.height << std::endl;

    return 0;
}

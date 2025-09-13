#include <iostream>
#include <string>
#include <windows.h>
#include "helper/WinAPI/terminal.h"

int main() {
    // UTF-8 出力対応
    SetConsoleOutputCP(CP_UTF8);
    
    std::cout << "Hello, World!" << std::endl;

    // ターミナルサイズ取得
    auto size = Terminal::getTerminalSize();
    std::cout << "Width: " << size.width << ", Height: " << size.height << std::endl;

    // 画面全体クリア
    Terminal::clearScreen();

    // 上書き表示
    Terminal::overwriteString(0, 0, u8"#============");
    Terminal::overwriteString(0, 1, u8"# Typinger(仮) Version 0.1  ");
    Terminal::overwriteString(0, 2, u8"#============");
    Terminal::overwriteString(0, 3, Terminal::Value_to_Blank(5, "1"));
    Terminal::overwriteString(0, 4, "12345\n");

    // 繰り返し表示例
    for (int i = 0; i < 4; ++i) {
        std::cout << "Width: " << size.width << ", Height: " << size.height << std::endl;
    }

    // キー入力ループ
    while (true) {
        for (int key = 8; key <= 255; ++key) {
            if (GetAsyncKeyState(key) & 0x8000) { // 押された瞬間のみ
                std::string msg = u8"入力されたキー: ";
                msg += static_cast<char>(key);

                // 最下行に上書き表示
                Terminal::overwriteString(0, size.height - 1, msg);

                // Q/qで終了
                if (key == 'Q' || key == 'q') {
                    // 終了前にバッファクリア
                    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
                    FlushConsoleInputBuffer(hStdin);
                    return 0;
                }

                Sleep(100); // 連続判定防止
            }
        }
    }
    // 終了前にバッファクリア
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    FlushConsoleInputBuffer(hStdin);
    return 0;
}

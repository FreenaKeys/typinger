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




    // 繰り返し表示例
    std::string msg = "Welcome to Typinger!";
    std::string blank = Terminal::Value_to_Blank((size.width - msg.size()) / 2, " ");
    msg = blank + msg + blank;
    Terminal::overwriteString(0, size.height - 2, "\x1b[7;37m" + msg + "\x1b[0m"); // 反転表示
    // 日本語メッセージ表示
    msg = u8"/!\\ これは開発中のタイピングソフトです、まだコード書いてる途中!　Qで終了";      
    int dispWidth = Terminal::getDisplayWidth(msg); // ← 修正
    blank = Terminal::Value_to_Blank((size.width - dispWidth) / 2, " ");
    msg =  msg + blank + blank;
    Terminal::overwriteString(0, size.height - 1, "\x1b[7;37m" + msg + "\x1b[0m");

    // 上書き表示
    msg = u8"Typinger(仮) Version 0.1";
    dispWidth = Terminal::getDisplayWidth(msg);
    blank = Terminal::Value_to_Blank((size.width - dispWidth) / 2, " ");
    msg =  blank + msg + blank;
    Terminal::overwriteString(0, 0, "\x1b[7;37m" + msg + "\x1b[0m");

    Terminal::overwriteString(0, 3, Terminal::Value_to_Blank(5, "1"));
    Terminal::overwriteString(0, 4, "12345\n");








    // キー入力ループ
    while (true) {
        for (int key = 8; key <= 255; ++key) {
            if (GetAsyncKeyState(key) & 0x8000) { // 押された瞬間のみ
                msg = u8"入力されたキー: ";
                msg += static_cast<char>(key);

                // 最下行に上書き表示
                Terminal::overwriteString(0, size.height - 3, msg); // 1

                // Q/qで終了
                if (key == 'Q' || key == 'q') {
                    // 終了前にバッファクリア
                    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
                    FlushConsoleInputBuffer(hStdin);
                    return 0;
                }

                Sleep(10); // 連続判定防止
            }
        }
    }
    // 終了前にバッファクリア
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    FlushConsoleInputBuffer(hStdin);
    return 0;
}

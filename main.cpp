#include <iostream>
#include <string>
#include <windows.h>
#include "helper/WinAPI/terminal.h"
#include <vector>

struct Cursor {
    int x;
    int y;
};

int initialized_UI(const char* version) {
    std::string Char_version = "Error! Cannot get version info";
    if (version){
        Char_version = std::string(version);
    }else{ return -1;}
    // ターミナルサイズ取得
    auto size = Terminal::getTerminalSize();

    // 繰り返し表示例
    std::string msg = "Welcome to Typinger! ";
    std::string blank = Terminal::Value_to_Blank((size.width - msg.size()) / 2, " ");
    msg = blank + msg + Terminal::Value_to_Blank(((size.width - msg.size()) / 2) - ("TerminalSize:" + std::to_string(size.width) + "x" + std::to_string(size.height)).size() , " ") + "TerminalSize:" + std::to_string(size.width) + "x" + std::to_string(size.height);
    Terminal::overwriteString(0, size.height - 2, "\x1b[7;37m" + msg + "\x1b[0m"); // 反転表示
    // 日本語メッセージ表示
    msg = u8"/!\\ これは開発中のタイピングソフトです、まだコード書いてる途中!　Qで終了";      
    int dispWidth = Terminal::getDisplayWidth(msg); // ← 修正
    blank = Terminal::Value_to_Blank((size.width - dispWidth) / 2, " ");
    msg =  msg + blank + blank;
    Terminal::overwriteString(0, size.height - 1, "\x1b[7;37m" + msg + "\x1b[0m");

    // 上書き表示
    msg = std::string(u8"Typinger(仮) Version ") + Char_version;
    dispWidth = Terminal::getDisplayWidth(msg);
    blank = Terminal::Value_to_Blank((size.width - dispWidth) / 2, " ");
    msg =  blank + msg + blank;
    Terminal::overwriteString(0, 0, "\x1b[7;37m" + msg + "\x1b[0m");

    Terminal::overwriteString(0, 1, "カーソルキーで移動、文字入力、BackSpaceで削除、Enterで改行");

    Terminal::overwriteString(0, 2, "#" + Terminal::Value_to_Blank(size.width - 2, "=") + "#"); // 入力エリアクリア

    std::string type_word = u8" ここに練習用のテキスト入力(Japanese or English!)";
    dispWidth = Terminal::getDisplayWidth(type_word);
    Terminal::overwriteString(0, 3, "#" + type_word + Terminal::Value_to_Blank(size.width - dispWidth - 2, " ") + "#");

    std::string type_word_rubi = u8"kokoni renshuuyou no tekisuto nyuuryoku";
    dispWidth = Terminal::getDisplayWidth(type_word_rubi);
    Terminal::overwriteString(0, 4, "#" + type_word_rubi + Terminal::Value_to_Blank(size.width - dispWidth - 2, " ") + "#");

    Terminal::overwriteString(0, 5,"#" + Terminal::Value_to_Blank(size.width - 3, "=") + "=#"); // 入力エリアクリア
    return 0;

}


int main() {
    //初期化開始
        // UTF-8 出力対応
        SetConsoleOutputCP(CP_UTF8);
        // ターミナルサイズ取得
        auto size = Terminal::getTerminalSize();

        // 画面全体クリア
        Terminal::clearScreen();
        // 入力エリアの先頭にカーソルを初期化
        Cursor cursor {0, 6};
        //UIの呼び出し
        const char* version = "0.0.0 - Dummy Version"; // ここでバージョンを指定
        initialized_UI(version); // 小数点以下切り捨てで整数化

    //初期化終了


    std::string line;
    // 複数行バッファ
    std::vector<std::string> lines(size.height, "");
    cursor.x = 0;
    cursor.y = 6; // 入力開始行

    DWORD lastMoveTime = 0;
    const DWORD repeatDelay = 300; // 最初のリピート待ち(ms)
    const DWORD repeatInterval = 50; // リピート間隔(ms)
    int lastMoveKey = 0;

    while (true) {
        bool updated = false;
        bool backspaceHeld = false;


        // Q/qで終了
        if ((GetAsyncKeyState('Q') & 0x8000) || (GetAsyncKeyState('q') & 0x8000)) {
            HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
            FlushConsoleInputBuffer(hStdin);
            return 0;
        }

        DWORD now = GetTickCount();

        // バックスペース
        if (GetAsyncKeyState(VK_BACK) & 0x8000) {
            auto& line = lines[cursor.y];
            if (cursor.x > 0 && cursor.x <= (int)line.size()) {
                line.erase(cursor.x - 1, 1);
                cursor.x--;
                updated = true;
            } else if (cursor.x == 0 && cursor.y > 6) {
                // 行頭でバックスペース：前の行と結合
                int prevY = cursor.y - 1;
                auto& prevLine = lines[prevY];
                int prevLen = (int)prevLine.size();
                prevLine += line;
                // 現在行を削除
                lines.erase(lines.begin() + cursor.y);
                // 空行を末尾に追加して高さ維持
                lines.push_back("");
                cursor.y = prevY;
                cursor.x = prevLen;
                updated = true;
            }
            while (GetAsyncKeyState(VK_BACK) & 0x8000) Sleep(1);
        }
        // 上
        else if (GetAsyncKeyState(VK_UP) & 0x8000 && cursor.y > 6) {
            cursor.y--;
            cursor.x = std::min(cursor.x, (int)lines[cursor.y].size());
            updated = true;
            while (GetAsyncKeyState(VK_UP) & 0x8000) Sleep(1);
        }
        // 下
        else if (GetAsyncKeyState(VK_DOWN) & 0x8000 && cursor.y < (int)lines.size() - 3) {
            cursor.y++;
            cursor.x = std::min(cursor.x, (int)lines[cursor.y].size());
            updated = true;
            while (GetAsyncKeyState(VK_DOWN) & 0x8000) Sleep(1);
        }
        // 左
        else if (GetAsyncKeyState(VK_LEFT) & 0x8000 && cursor.x > 0) {
            cursor.x--;
            updated = true;
            while (GetAsyncKeyState(VK_LEFT) & 0x8000) Sleep(1);
        }
        // 右
        else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && cursor.x < (int)lines[cursor.y].size()) {
            cursor.x++;
            updated = true;
            while (GetAsyncKeyState(VK_RIGHT) & 0x8000) Sleep(1);
        }
        // Enterキー（改行：下の行に移動）
        else if (GetAsyncKeyState(VK_RETURN) & 0x8000 && cursor.y < (int)lines.size() - 2) {
            cursor.y++;
            cursor.x = 0;
            updated = true;
            while (GetAsyncKeyState(VK_RETURN) & 0x8000) Sleep(1);
        }

        // 文字入力（ASCII範囲のみ、特殊キー除外）
        for (int key = 32; key <= 126; ++key) {
            if (key == VK_LEFT || key == VK_RIGHT || key == VK_UP || key == VK_DOWN || key == VK_RETURN || key == VK_BACK) continue;
            if (GetAsyncKeyState(key) & 0x8000) {
                auto& line = lines[cursor.y];
                char ch = static_cast<char>(key);
                if (ch >= 'A' && ch <= 'Z') {
                    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                    if (!shift) ch += 32; // 小文字化
                }
                // line.size()の範囲内ならどこでも挿入可能
                if (cursor.x < 0) cursor.x = 0;
                if (cursor.x > (int)line.size()) cursor.x = (int)line.size();
                line.insert(cursor.x, 1, ch);
                cursor.x++;
                updated = true;
                while (GetAsyncKeyState(key) & 0x8000) Sleep(1);
                break;
            }
        }

        // 入力があった時だけ描画
        if (updated) {
            if (backspaceHeld) {
                backspaceHeld = false;
                if (cursor.x < 1) {
                    if (cursor.y > 1) {
                        cursor.y = cursor.y - 1;
                    }
                } else {
                    cursor.x = cursor.x - 1;
                }
            }
            // 入力エリア全体を再描画（ゴーストレター対策）
            for (int y = 6; y < size.height - 2; ++y) {
                Terminal::overwriteString(0, y, Terminal::Value_to_Blank(size.width, " "));
                Terminal::overwriteString(0, y, lines[y]);
            }
            Terminal::SetConsoleCursorPosition(cursor.x, cursor.y);
        }
        Sleep(2);
    }

    // 終了前にバッファクリア
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    FlushConsoleInputBuffer(hStdin);
    return 0;
}

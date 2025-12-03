#include "waiting_screen.h"
#include "../helper/WinAPI/terminal.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace WaitingScreen {

    std::string getLayoutName(const std::string& layoutType) {
        if (layoutType == "new") {
            return "新配列";
        } else if (layoutType == "old") {
            return "旧配列";
        } else {
            return "不明";
        }
    }

    void showMessage(const std::string& message) {
        Terminal::clearScreen();
        Terminal::SetConsoleCursorPosition(0, 5);
        std::cout << "\n";
        std::cout << "  " << message << "\n";
        std::cout << "\n";
    }

    void showCountdown(int testNumber, const std::string& layoutType, int durationSeconds) {
        // 画面をクリア
        Terminal::clearScreen();
        
        // メッセージを表示
        Terminal::SetConsoleCursorPosition(0, 3);
        std::cout << "\n";
        std::cout << "  ========================================\n";
        std::cout << "  次のテスト準備中...\n";
        std::cout << "  ========================================\n";
        std::cout << "\n";
        std::cout << "  テスト番号: " << testNumber << "/6\n";
        std::cout << "  配列タイプ: " << getLayoutName(layoutType) << "\n";
        std::cout << "\n";
        
        // カウントダウン位置を記録
        int countdownY = 10;
        
        // カウントダウン
        for (int i = durationSeconds; i > 0; i--) {
            Terminal::SetConsoleCursorPosition(0, countdownY);
            std::cout << "  開始まで: " << i << " 秒  ";
            std::cout.flush();
            
            // 1秒待機
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        // 開始メッセージ
        Terminal::SetConsoleCursorPosition(0, countdownY);
        std::cout << "  開始！          ";
        std::cout.flush();
        
        // 少し待機
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    void clear() {
        Terminal::clearScreen();
    }

} // namespace WaitingScreen

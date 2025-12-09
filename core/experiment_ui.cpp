#include "experiment_ui.h"
#include "experiment_session.h"
#include "../helper/WinAPI/terminal.h"
#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <conio.h>

namespace ExperimentUI {

    Mode selectMode() {
        Terminal::clearScreen();
        
        // タイトル表示
        Terminal::SetConsoleCursorPosition(0, 2);
        std::cout << "\n";
        std::cout << "  ========================================\n";
        std::cout << "  Typinger - モード選択\n";
        std::cout << "  ========================================\n";
        std::cout << "\n";
        std::cout << "  使用するモードを選択してください：\n";
        std::cout << "\n";
        std::cout << "  [1] 通常モード（練習用）\n";
        std::cout << "  [2] 実験モード（データ収集用）\n";
        std::cout << "  [3] 入力モード（入力確認用）\n";
        std::cout << "  [ESC] 終了\n";
        std::cout << "\n";
        
        // キー入力待ち
        while (true) {
            if (GetAsyncKeyState('1') & 0x8000) {
                while (GetAsyncKeyState('1') & 0x8000) Sleep(1);
                return Mode::NORMAL;
            }
            if (GetAsyncKeyState('2') & 0x8000) {
                while (GetAsyncKeyState('2') & 0x8000) Sleep(1);
                return Mode::EXPERIMENT;
            }
            if (GetAsyncKeyState('3') & 0x8000) {
                while (GetAsyncKeyState('3') & 0x8000) Sleep(1);
                return Mode::INPUT_TEST;
            }
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                while (GetAsyncKeyState(VK_ESCAPE) & 0x8000) Sleep(1);
                return Mode::QUIT;
            }
            Sleep(10);
        }
    }

    std::string inputSubjectID() {
        Terminal::clearScreen();
        
        Terminal::SetConsoleCursorPosition(0, 2);
        std::cout << "\n";
        std::cout << "  ========================================\n";
        std::cout << "  実験モード - 被験者ID入力\n";
        std::cout << "  ========================================\n";
        std::cout << "\n";
        std::cout << "  被験者IDを選択してください：\n";
        std::cout << "\n";
        std::cout << "  [1] SK\n";
        std::cout << "  [2] MT\n";
        std::cout << "  [3] IK\n";
        std::cout << "  [4] KS\n";
        std::cout << "  [5] KM\n";
        std::cout << "  [ESC] キャンセル\n";
        std::cout << "\n";
        
        // キー入力待ち
        while (true) {
            if (GetAsyncKeyState('1') & 0x8000) {
                while (GetAsyncKeyState('1') & 0x8000) Sleep(1);
                return "SK";
            }
            if (GetAsyncKeyState('2') & 0x8000) {
                while (GetAsyncKeyState('2') & 0x8000) Sleep(1);
                return "MT";
            }
            if (GetAsyncKeyState('3') & 0x8000) {
                while (GetAsyncKeyState('3') & 0x8000) Sleep(1);
                return "IK";
            }
            if (GetAsyncKeyState('4') & 0x8000) {
                while (GetAsyncKeyState('4') & 0x8000) Sleep(1);
                return "KS";
            }
            if (GetAsyncKeyState('5') & 0x8000) {
                while (GetAsyncKeyState('5') & 0x8000) Sleep(1);
                return "KM";
            }
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                while (GetAsyncKeyState(VK_ESCAPE) & 0x8000) Sleep(1);
                return "";  // キャンセル
            }
            Sleep(10);
        }
    }

    int inputSessionNumber() {
        Terminal::clearScreen();
        
        Terminal::SetConsoleCursorPosition(0, 2);
        std::cout << "\n";
        std::cout << "  ========================================\n";
        std::cout << "  実験モード - セッション番号入力\n";
        std::cout << "  ========================================\n";
        std::cout << "\n";
        std::cout << "  セッション番号を選択してください：\n";
        std::cout << "\n";
        std::cout << "  [1] セッション 1\n";
        std::cout << "  [2] セッション 2\n";
        std::cout << "  [3] セッション 3\n";
        std::cout << "  [4] セッション 4\n";
        std::cout << "  [5] セッション 5\n";
        std::cout << "  [6] セッション 6\n";
        std::cout << "  [ESC] キャンセル\n";
        std::cout << "\n";
        
        // キー入力待ち
        while (true) {
            for (int key = '1'; key <= '6'; ++key) {
                if (GetAsyncKeyState(key) & 0x8000) {
                    int session = key - '0';
                    while (GetAsyncKeyState(key) & 0x8000) Sleep(1);
                    return session;
                }
            }
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                while (GetAsyncKeyState(VK_ESCAPE) & 0x8000) Sleep(1);
                return -1;  // キャンセル
            }
            Sleep(10);
        }
    }

    ExperimentSession inputExperimentSession() {
        ExperimentSession session;
        
        // 被験者ID入力
        std::string subjectID = inputSubjectID();
        if (subjectID.empty()) {
            // キャンセルされた場合は空のセッションを返す
            session.subject_id = "";
            return session;
        }
        session.subject_id = subjectID;
        
        // セッション番号入力
        int sessionNumber = inputSessionNumber();
        if (sessionNumber < 0) {
            // キャンセルされた場合は空のセッションを返す
            session.subject_id = "";
            return session;
        }
        session.session_number = sessionNumber;
        
        // 初期値設定（test_orderとlayout_typeは後で設定）
        session.test_order = 1;
        // 最初の配列タイプは"new"と仮定（セッション番号の奇数/偶数で判定）
        std::string first_layout = (sessionNumber % 2 == 1) ? "new" : "old";
        session.layout_type = first_layout;
        
        // 日付は空（後で設定される想定）
        session.date = "";
        
        return session;
    }

    void showError(const std::string& message) {
        Terminal::clearScreen();
        
        Terminal::SetConsoleCursorPosition(0, 5);
        std::cout << "\n";
        Terminal::setTextColor(Terminal::LIGHT_RED);
        std::cout << "  *** エラー ***\n";
        Terminal::resetTextColor();
        std::cout << "\n";
        std::cout << "  " << message << "\n";
        std::cout << "\n";
        std::cout << "  (3秒後に戻ります...)\n";
        
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    void showConfirmation(const ExperimentSession& session) {
        Terminal::clearScreen();
        
        Terminal::SetConsoleCursorPosition(0, 3);
        std::cout << "\n";
        std::cout << "  ========================================\n";
        std::cout << "  実験セッション確認\n";
        std::cout << "  ========================================\n";
        std::cout << "\n";
        std::cout << "  被験者ID: " << session.subject_id << "\n";
        std::cout << "  セッション番号: " << session.session_number << "\n";
        std::cout << "  最初のテスト: Test 1 (" 
                  << (session.layout_type == "new" ? "新配列" : "旧配列") << ")\n";
        std::cout << "\n";
        std::cout << "  開始するには 'y' を入力してください (Press 'y' to start)\n";
        
        // 入力バッファクリア
        while (_kbhit()) _getch();

        // キー入力待ち
        while (true) {
            int key = _getch();
            if (key == 'y' || key == 'Y') {
                break;
            }
        }
    }

} // namespace ExperimentUI

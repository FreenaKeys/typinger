#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <algorithm>
#include <random>
#include "helper/WinAPI/terminal.h"
#include "helper/WinAPI/timer.h"
#include "helper/json_helper.h"
#include "core/input_recorder.h"
#include "core/typing_judge.h"
#include "core/romaji_converter.h"
#include "core/statistics.h"
#include "core/csv_logger.h"
#include "core/experiment_ui.h"
#include "core/experiment_session.h"
#include "core/scenario_selector.h"
#include "core/countdown_timer.h"
#include "core/test_loop_controller.h"
#include "core/waiting_screen.h"
#include "core/input_mode.h"
#include <vector>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace fs = std::filesystem;
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

    Terminal::overwriteString(0, 1, "カーソルキーで移動、文字入力、BackSpaceで削除、Enterで改行、Escで終了");

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

int typing_mode() {
    // ターミナルサイズ取得
    auto size = Terminal::getTerminalSize();
    
    // Phase 2-3: scenarioファイルからtext/rubiを読み込み
    std::string scenarioPath = "scenario/scenarioexample.json";
    auto scenarioData = JsonHelper::loadJsonFromFile(scenarioPath);
    
    // 最初のエントリ（"1"）を取得
    std::string targetText = "こんにちは";  // デフォルト
    std::string targetRubi = "konnichiha";  // デフォルト
    
    if (scenarioData.isObject() && scenarioData["entries"].isObject()) {
        auto entries = scenarioData["entries"];
        if (entries["1"].isObject()) {
            auto entry = entries["1"];
            if (entry["text"].isString()) {
                targetText = entry["text"].asString();
            }
            if (entry["rubi"].isString()) {
                targetRubi = entry["rubi"].asString();
            }
        }
    }
    
    // Phase 2-2: タイピング判定クラスの初期化
    TypingJudge::Judge judge(targetText, targetRubi);
    
    Cursor cursor;
    std::string line;
    // 複数行バッファ
    std::vector<std::string> lines(size.height, "");
    cursor.x = 0;
    cursor.y = 6; // 入力開始行

    // InputRecorder統合（フェーズ1-3）
    InputRecorder::Recorder recorder;
    recorder.startSession();
    
    // Phase 3-3: Statistics統合
    Statistics::Calculator statsCalc;
    uint64_t startTime = WinTimer::now_us();
    statsCalc.startSession(startTime);
    
    // Phase 3-4: かな入力追跡用
    RomajiConverter::Converter romajiConv;
    std::string currentRomajiBuffer;  // 現在入力中のローマ字バッファ
    uint64_t kanaStartTime = 0;       // かな入力開始時刻
    
    // Phase 3-4: キーリピート防止用（前回のキー状態を記録）
    bool lastKeyStates[256] = {false};  // 全てのキーの前回の状態
    
    // Phase 2-3: 目標テキストとルビを表示
    Terminal::overwriteString(0, 4, "Target: " + targetText + " [" + targetRubi + "]");

    while (true) {
        bool updated = false;


        // Escで終了
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            // InputRecorder: セッション終了
            recorder.endSession();
            
            // Phase 3-3: 統計計算（途中終了でも表示）
            uint64_t endTime = WinTimer::now_us();
            statsCalc.endSession(endTime);
            
            const auto& events = recorder.getEvents();
            for (const auto& event : events) {
                if (event.type == InputRecorder::EventType::KEY_DOWN) {
                    statsCalc.recordKeyDown(event.timestamp_us, event.vk_code, event.character);
                } else if (event.type == InputRecorder::EventType::KEY_UP) {
                    statsCalc.recordKeyUp(event.timestamp_us, event.vk_code);
                } else if (event.type == InputRecorder::EventType::BACKSPACE) {
                    statsCalc.recordBackspace(event.timestamp_us);
                }
            }
            
            auto stats = statsCalc.calculate(judge.getCorrectCount(), judge.getIncorrectCount());
            double accuracy = (stats.correctKeyCount + stats.incorrectKeyCount > 0) 
                ? static_cast<double>(stats.correctKeyCount) / (stats.correctKeyCount + stats.incorrectKeyCount) 
                : 0.0;
            
            // Phase 4: CSV出力（イベント + サマリ）
            std::string eventCsvPath = CSVLogger::writeEventCSV(recorder, "output");
            std::string summaryCsvPath = CSVLogger::writeSummaryCSV(stats, "output");
            
            // 画面クリア（統計情報表示エリア）
            for (int y = 0; y < size.height; ++y) {
                Terminal::overwriteString(0, y, Terminal::Value_to_Blank(size.width, " "));
            }
            
            // 統計情報の表示
            Terminal::overwriteString(0, size.height - 10, "=== Typing Statistics (Interrupted) ===");
            Terminal::overwriteString(0, size.height - 9, 
                "Accuracy: " + std::to_string(static_cast<int>(accuracy * 100)) + "%" +
                " | Correct: " + std::to_string(stats.correctKeyCount) +
                " | Incorrect: " + std::to_string(stats.incorrectKeyCount));
            Terminal::overwriteString(0, size.height - 8, 
                "WPM: " + std::to_string(static_cast<int>(stats.wpmCorrect)) +
                " | CPM: " + std::to_string(static_cast<int>(stats.cpmCorrect)));
            Terminal::overwriteString(0, size.height - 7, 
                "Avg Inter-key: " + std::to_string(static_cast<int>(stats.avgInterKeyInterval)) + " ms");
            Terminal::overwriteString(0, size.height - 6, 
                "Backspaces: " + std::to_string(stats.backspaceCount));
            Terminal::overwriteString(0, size.height - 4, 
                "Session ended. Events: " + std::to_string(recorder.getEventCount()) + 
                " | Duration: " + std::to_string(stats.totalDuration / 1000) + " ms");
            
            // Phase 4: CSV出力結果を表示
            if (!eventCsvPath.empty()) {
                Terminal::overwriteString(0, size.height - 3, "Event CSV saved: " + eventCsvPath);
            }
            if (!summaryCsvPath.empty()) {
                Terminal::overwriteString(0, size.height - 2, "Summary CSV saved: " + summaryCsvPath);
            }
            
            Sleep(3000);  // 3秒表示
            
            HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
            FlushConsoleInputBuffer(hStdin);
            return 0;
        }

        // バックスペース
        if (GetAsyncKeyState(VK_BACK) & 0x8000) {
            recorder.recordBackspace();  // Backspace記録
            
            // Phase 3-4: バックスペース時はかなバッファをクリア
            currentRomajiBuffer.clear();
            kanaStartTime = 0;
            
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
            
            bool keyPressed = (GetAsyncKeyState(key) & 0x8000) != 0;
            
            // Phase 3-4: キーが新しく押された場合のみ処理（リピート防止）
            if (keyPressed && !lastKeyStates[key]) {
                lastKeyStates[key] = true;
                
                auto& line = lines[cursor.y];
                char ch = static_cast<char>(key);
                if (ch >= 'A' && ch <= 'Z') {
                    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                    if (!shift) ch += 32; // 小文字化
                }
                
                // InputRecorder: キーダウン記録
                recorder.recordKeyDown(key, 0, ch);
                
                // Phase 3-4: かな入力追跡
                uint64_t keyDownTime = WinTimer::now_us();
                
                // Phase 2-3: タイピング判定
                auto result = judge.judgeChar(ch);
                
                // Phase 3-3: 統計データ記録
                recorder.setLastEventCorrectness(result == TypingJudge::JudgeResult::CORRECT);
                
                // Phase 3-4: かな確定検知
                if (result == TypingJudge::JudgeResult::CORRECT) {
                    // バッファが空なら、かな入力開始
                    if (currentRomajiBuffer.empty()) {
                        kanaStartTime = keyDownTime;
                    }
                    
                    // ローマ字バッファに追加
                    currentRomajiBuffer += ch;
                    
                    // かな変換を試行
                    auto convertResult = romajiConv.convert(currentRomajiBuffer);
                    if (convertResult.status == RomajiConverter::ConvertStatus::MATCHED) {
                        // かな確定！統計に記録
                        uint64_t keyUpTime = WinTimer::now_us();
                        statsCalc.recordKanaInput(convertResult.kana, currentRomajiBuffer, 
                                                  kanaStartTime, keyUpTime);
                        
                        // バッファをクリア
                        currentRomajiBuffer.clear();
                        kanaStartTime = 0;
                    }
                    // PARTIAL（入力途中）の場合は何もせず、次の文字を待つ
                } else if (result == TypingJudge::JudgeResult::INCORRECT) {
                    // 誤入力時はバッファをクリア
                    currentRomajiBuffer.clear();
                    kanaStartTime = 0;
                }
                
                // 判定結果を画面に表示（デバッグ用）
                std::string resultStr;
                if (result == TypingJudge::JudgeResult::CORRECT) {
                    resultStr = "CORRECT";
                } else if (result == TypingJudge::JudgeResult::INCORRECT) {
                    resultStr = "INCORRECT";
                } else {
                    resultStr = "ALREADY_DONE";
                }
                Terminal::overwriteString(0, 5, "Result: " + resultStr + " | Progress: " + 
                    std::to_string(judge.getCurrentPosition()) + "/" + std::to_string(judge.getTargetLength()) +
                    " | Remaining: [" + judge.getRemainingRubi() + "]");
                
                // Phase 2-3: 完了判定（自動終了）
                if (judge.isCompleted()) {
                    // セッション終了
                    recorder.endSession();
                    
                    // Phase 3-3: 統計計算
                    uint64_t endTime = WinTimer::now_us();
                    statsCalc.endSession(endTime);
                    
                    const auto& events = recorder.getEvents();
                    for (const auto& event : events) {
                        if (event.type == InputRecorder::EventType::KEY_DOWN) {
                            statsCalc.recordKeyDown(event.timestamp_us, event.vk_code, event.character);
                        } else if (event.type == InputRecorder::EventType::KEY_UP) {
                            statsCalc.recordKeyUp(event.timestamp_us, event.vk_code);
                        } else if (event.type == InputRecorder::EventType::BACKSPACE) {
                            statsCalc.recordBackspace(event.timestamp_us);
                        }
                    }
                    
                    auto stats = statsCalc.calculate(judge.getCorrectCount(), judge.getIncorrectCount());
                    double accuracy = (stats.correctKeyCount + stats.incorrectKeyCount > 0) 
                        ? static_cast<double>(stats.correctKeyCount) / (stats.correctKeyCount + stats.incorrectKeyCount) 
                        : 0.0;
                    
                    // Phase 4: CSV出力（イベント + サマリ）
                    std::string eventCsvPath = CSVLogger::writeEventCSV(recorder, "output");
                    std::string summaryCsvPath = CSVLogger::writeSummaryCSV(stats, "output");
                    
                    // 画面クリア（統計情報表示エリア）
                    for (int y = 0; y < size.height; ++y) {
                        Terminal::overwriteString(0, y, Terminal::Value_to_Blank(size.width, " "));
                    }
                    
                    // 統計情報の表示
                    Terminal::overwriteString(0, size.height - 10, "=== Typing Statistics ===");
                    Terminal::overwriteString(0, size.height - 9, 
                        "Accuracy: " + std::to_string(static_cast<int>(accuracy * 100)) + "%" +
                        " | Correct: " + std::to_string(stats.correctKeyCount) +
                        " | Incorrect: " + std::to_string(stats.incorrectKeyCount));
                    Terminal::overwriteString(0, size.height - 8, 
                        "WPM: " + std::to_string(static_cast<int>(stats.wpmCorrect)) +
                        " | CPM: " + std::to_string(static_cast<int>(stats.cpmCorrect)));
                    Terminal::overwriteString(0, size.height - 7, 
                        "Avg Inter-key: " + std::to_string(static_cast<int>(stats.avgInterKeyInterval)) + " ms");
                    Terminal::overwriteString(0, size.height - 6, 
                        "Backspaces: " + std::to_string(stats.backspaceCount));
                    
                    // かな別入力時間の表示（上位5件）
                    int displayLine = size.height - 5;
                    Terminal::overwriteString(0, displayLine++, "Top 5 Kana Input Times:");
                    int count = 0;
                    for (const auto& [kana, time] : stats.kanaInputTime) {
                        if (count >= 5) break;
                        std::ostringstream oss;
                        oss << "  " << kana << ": " << std::fixed << std::setprecision(0) << time << " ms";
                        Terminal::overwriteString(0, displayLine++, oss.str());
                        count++;
                    }
                    
                    Terminal::overwriteString(0, size.height - 3, "*** COMPLETED! *** Press ESC to exit...");
                    
                    // Phase 4: CSV出力結果を表示
                    if (!eventCsvPath.empty()) {
                        Terminal::overwriteString(0, size.height - 2, "Event CSV saved: " + eventCsvPath);
                    }
                    if (!summaryCsvPath.empty()) {
                        Terminal::overwriteString(0, size.height - 1, "Summary CSV saved: " + summaryCsvPath);
                    }
                    
                    // バッファクリア（ESC待機前）
                    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
                    FlushConsoleInputBuffer(hStdin);
                    
                    // ESCキー待ち
                    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
                        Sleep(100);
                    }
                    
                    // ESCキー待ち（キーが離されるまで）
                    while (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                        Sleep(10);
                    }
                    
                    // Phase 3-4: 終了前に画面をクリア
                    for (int y = 0; y < size.height; ++y) {
                        Terminal::overwriteString(0, y, Terminal::Value_to_Blank(size.width, " "));
                    }
                    Terminal::overwriteString(0, 0, "Thank you for using Typinger!");
                    
                    FlushConsoleInputBuffer(hStdin);
                    return 0;
                }
                
                // line.size()の範囲内ならどこでも挿入可能
                if (cursor.x < 0) cursor.x = 0;
                if (cursor.x > (int)line.size()) cursor.x = (int)line.size();
                line.insert(cursor.x, 1, ch);
                cursor.x++;
                updated = true;
                
                // Phase 3-4: キーアップ待ちを削除（高速入力対応）
                // キーアップ記録は次のループで処理
                recorder.recordKeyUp(key, 0);
                
                break;
            } else if (!keyPressed && lastKeyStates[key]) {
                // キーが離された
                lastKeyStates[key] = false;
            }
        }
        
        // Phase 2-3: ハイフンキーの処理 (VK_OEM_MINUS = 0xBD)
        bool hyphenKeyPressed = (GetAsyncKeyState(0xBD) & 0x8000) != 0;
        
        // Phase 3-4: キーが新しく押された場合のみ処理（リピート防止）
        if (hyphenKeyPressed && !lastKeyStates[0xBD]) {
            lastKeyStates[0xBD] = true;
            
            auto& line = lines[cursor.y];
            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            char ch = shift ? '_' : '-';
            
            // InputRecorder: キーダウン記録
            recorder.recordKeyDown(0xBD, 0, ch);
            
            // Phase 3-4: かな入力追跡
            uint64_t keyDownTime = WinTimer::now_us();
            
            // Phase 2-3: タイピング判定
            auto result = judge.judgeChar(ch);
            
            // Phase 3-3: 統計データ記録
            recorder.setLastEventCorrectness(result == TypingJudge::JudgeResult::CORRECT);
            
            // Phase 3-4: かな確定検知
            if (result == TypingJudge::JudgeResult::CORRECT) {
                // バッファが空なら、かな入力開始
                if (currentRomajiBuffer.empty()) {
                    kanaStartTime = keyDownTime;
                }
                
                // ローマ字バッファに追加
                currentRomajiBuffer += ch;
                
                // かな変換を試行
                auto convertResult = romajiConv.convert(currentRomajiBuffer);
                if (convertResult.status == RomajiConverter::ConvertStatus::MATCHED) {
                    // かな確定！統計に記録
                    uint64_t keyUpTime = WinTimer::now_us();
                    statsCalc.recordKanaInput(convertResult.kana, currentRomajiBuffer, 
                                              kanaStartTime, keyUpTime);
                    
                    // バッファをクリア
                    currentRomajiBuffer.clear();
                    kanaStartTime = 0;
                }
                // PARTIAL（入力途中）の場合は何もせず、次の文字を待つ
            } else if (result == TypingJudge::JudgeResult::INCORRECT) {
                // 誤入力時はバッファをクリア
                currentRomajiBuffer.clear();
                kanaStartTime = 0;
            }
            
            // 判定結果を画面に表示
            std::string resultStr;
            if (result == TypingJudge::JudgeResult::CORRECT) {
                resultStr = "CORRECT";
            } else if (result == TypingJudge::JudgeResult::INCORRECT) {
                resultStr = "INCORRECT";
            } else {
                resultStr = "ALREADY_DONE";
            }
            Terminal::overwriteString(0, 5, "Result: " + resultStr + " | Progress: " + 
                std::to_string(judge.getCurrentPosition()) + "/" + std::to_string(judge.getTargetLength()) +
                " | Remaining: [" + judge.getRemainingRubi() + "]");
            
            // Phase 2-3: 完了判定（自動終了）
            if (judge.isCompleted()) {
                // セッション終了
                recorder.endSession();
                
                // Phase 3-3: 統計計算
                uint64_t endTime = WinTimer::now_us();
                statsCalc.endSession(endTime);
                
                const auto& events = recorder.getEvents();
                for (const auto& event : events) {
                    if (event.type == InputRecorder::EventType::KEY_DOWN) {
                        statsCalc.recordKeyDown(event.timestamp_us, event.vk_code, event.character);
                    } else if (event.type == InputRecorder::EventType::KEY_UP) {
                        statsCalc.recordKeyUp(event.timestamp_us, event.vk_code);
                    } else if (event.type == InputRecorder::EventType::BACKSPACE) {
                        statsCalc.recordBackspace(event.timestamp_us);
                    }
                }
                
                auto stats = statsCalc.calculate(judge.getCorrectCount(), judge.getIncorrectCount());
                double accuracy = (stats.correctKeyCount + stats.incorrectKeyCount > 0) 
                    ? static_cast<double>(stats.correctKeyCount) / (stats.correctKeyCount + stats.incorrectKeyCount) 
                    : 0.0;
                
                // Phase 4: CSV出力（イベント + サマリ）
                std::string eventCsvPath = CSVLogger::writeEventCSV(recorder, "output");
                std::string summaryCsvPath = CSVLogger::writeSummaryCSV(stats, "output");
                
                // 画面クリア（統計情報表示エリア）
                for (int y = 0; y < size.height; ++y) {
                    Terminal::overwriteString(0, y, Terminal::Value_to_Blank(size.width, " "));
                }
                
                // 統計情報の表示
                Terminal::overwriteString(0, size.height - 10, "=== Typing Statistics ===");
                Terminal::overwriteString(0, size.height - 9, 
                    "Accuracy: " + std::to_string(static_cast<int>(accuracy * 100)) + "%" +
                    " | Correct: " + std::to_string(stats.correctKeyCount) +
                    " | Incorrect: " + std::to_string(stats.incorrectKeyCount));
                Terminal::overwriteString(0, size.height - 8, 
                    "WPM: " + std::to_string(static_cast<int>(stats.wpmCorrect)) +
                    " | CPM: " + std::to_string(static_cast<int>(stats.cpmCorrect)));
                Terminal::overwriteString(0, size.height - 7, 
                    "Avg Inter-key: " + std::to_string(static_cast<int>(stats.avgInterKeyInterval)) + " ms");
                Terminal::overwriteString(0, size.height - 6, 
                    "Backspaces: " + std::to_string(stats.backspaceCount));
                
                // かな別入力時間の表示（上位5件）
                int displayLine = size.height - 5;
                Terminal::overwriteString(0, displayLine++, "Top 5 Kana Input Times:");
                int count = 0;
                for (const auto& [kana, time] : stats.kanaInputTime) {
                    if (count >= 5) break;
                    std::ostringstream oss;
                    oss << "  " << kana << ": " << std::fixed << std::setprecision(0) << time << " ms";
                    Terminal::overwriteString(0, displayLine++, oss.str());
                    count++;
                }
                
                Terminal::overwriteString(0, size.height - 3, "*** COMPLETED! *** Press ESC to exit...");
                
                // Phase 4: CSV出力結果を表示
                if (!eventCsvPath.empty()) {
                    Terminal::overwriteString(0, size.height - 2, "Event CSV saved: " + eventCsvPath);
                }
                if (!summaryCsvPath.empty()) {
                    Terminal::overwriteString(0, size.height - 1, "Summary CSV saved: " + summaryCsvPath);
                }
                
                // バッファクリア（ESC待機前）
                HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
                FlushConsoleInputBuffer(hStdin);
                
                // ESCキー待ち
                while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
                    Sleep(100);
                }
                
                // ESCキー待ち（キーが離されるまで）
                while (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                    Sleep(10);
                }
                
                // Phase 3-4: 終了前に画面をクリア
                for (int y = 0; y < size.height; ++y) {
                    Terminal::overwriteString(0, y, Terminal::Value_to_Blank(size.width, " "));
                }
                Terminal::overwriteString(0, 0, "Thank you for using Typinger!");
                
                FlushConsoleInputBuffer(hStdin);
                return 0;
            }
            
            // line.size()の範囲内ならどこでも挿入可能
            if (cursor.x < 0) cursor.x = 0;
            if (cursor.x > (int)line.size()) cursor.x = (int)line.size();
            line.insert(cursor.x, 1, ch);
            cursor.x++;
            updated = true;
            
            // Phase 3-4: キーアップ待ちを削除（高速入力対応）
            // キーアップ記録は次のループで処理
            recorder.recordKeyUp(0xBD, 0);
        } else if (!hyphenKeyPressed && lastKeyStates[0xBD]) {
            // キーが離された
            lastKeyStates[0xBD] = false;
        }

        // 入力があった時だけ描画
        if (updated) {
            // 入力エリア全体を再描画（ゴーストレター対策）
            for (int y = 6; y < size.height - 2; ++y) {
                Terminal::overwriteString(0, y, Terminal::Value_to_Blank(size.width, " "));
                Terminal::overwriteString(0, y, lines[y]);
            }
            Terminal::SetConsoleCursorPosition(cursor.x, cursor.y);
        }
        Sleep(1);  // Phase 3-4: ポーリング頻度を上げて高速入力に対応
    }

    // 終了前にバッファクリア
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    FlushConsoleInputBuffer(hStdin);
    return 0;
}

int view_mode(const std::string& filePath) {
    // ターミナルサイズ取得
    auto size = Terminal::getTerminalSize();
    
    Cursor cursor;
    // 複数行バッファ
    std::vector<std::string> lines(size.height, "");
    cursor.x = 0;
    cursor.y = 6; // 入力開始行

    // ファイル読み込み
    std::ifstream file(filePath);
    if (!file.is_open()) {
        Terminal::overwriteString(0, 6, "Error: Cannot open file " + filePath);
        Sleep(2000);
        return -1;
    }
    std::string line;
    int y = 6;
    while (std::getline(file, line) && y < size.height - 2) {
        lines[y] = line;
        y++;
    }
    file.close();

    // 内容を表示
    for (int i = 6; i < y; ++i) {
        Terminal::overwriteString(0, i, lines[i]); //選択したファイルの中身を書き出す
    }

    while (true) {
        // Escで戻る
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
            FlushConsoleInputBuffer(hStdin);
            return 0;
        }

        // 上
        if (GetAsyncKeyState(VK_UP) & 0x8000 && cursor.y > 6) {
            cursor.y--;
            cursor.x = std::min(cursor.x, (int)lines[cursor.y].size());
            Terminal::SetConsoleCursorPosition(cursor.x, cursor.y);
            while (GetAsyncKeyState(VK_UP) & 0x8000) Sleep(1);
        }
        // 下
        else if (GetAsyncKeyState(VK_DOWN) & 0x8000 && cursor.y < y - 1) {
            cursor.y++;
            cursor.x = std::min(cursor.x, (int)lines[cursor.y].size());
            Terminal::SetConsoleCursorPosition(cursor.x, cursor.y);
            while (GetAsyncKeyState(VK_DOWN) & 0x8000) Sleep(1);
        }
        // 左
        else if (GetAsyncKeyState(VK_LEFT) & 0x8000 && cursor.x > 0) {
            cursor.x--;
            Terminal::SetConsoleCursorPosition(cursor.x, cursor.y);
            while (GetAsyncKeyState(VK_LEFT) & 0x8000) Sleep(1);
        }
        // 右
        else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && cursor.x < (int)lines[cursor.y].size()) {
            cursor.x++;
            Terminal::SetConsoleCursorPosition(cursor.x, cursor.y);
            while (GetAsyncKeyState(VK_RIGHT) & 0x8000) Sleep(1);
        }

        Sleep(2);
    }

    return 0;
}

std::string select_file(const fs::path& exeDir) {
    std::vector<fs::path> files;
    if(fs::exists(exeDir) && fs::is_directory(exeDir)) {
        for (auto& entry : fs::directory_iterator(exeDir)) {
            if (fs::is_regular_file(entry.path())) {
                files.push_back(entry.path());
            }
        }
    }

    if (files.empty()) {
        return "";
    }

    auto size = Terminal::getTerminalSize();
    Terminal::clearScreen();
    Terminal::overwriteString(0, 0, "Select a file to view at startup (press number, or ESC to skip):");
    for (size_t i = 0; i < files.size() && i < static_cast<size_t>(size.height - 2); ++i) {
        Terminal::overwriteString(0, i + 1, std::to_string(i + 1) + ": " + files[i].filename().string());
    }

    while (true) {
        for (int key = '1'; key <= '9'; ++key) {
            if (GetAsyncKeyState(key) & 0x8000) {
                int num = key - '0';
                if (num >= 1 && num <= (int)files.size()) {
                    while (GetAsyncKeyState(key) & 0x8000) Sleep(1);
                    return files[num - 1].string();
                }
            }
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            while (GetAsyncKeyState(VK_ESCAPE) & 0x8000) Sleep(1);
            return "";
        }
        Sleep(2);
    }
}


// 実験モード: 1テスト実行
int run_single_experiment_test(const ExperimentSession& session, int testNumber) {
    // シナリオファイルパスを取得
    std::string scenarioPath = ScenarioSelector::getScenarioPath(session.session_number, testNumber);
    
    // シナリオファイルの存在確認
    if (!ScenarioSelector::scenarioExists(scenarioPath)) {
        ExperimentUI::showError("シナリオファイルが見つかりません: " + scenarioPath);
        return -1;
    }
    
    // シナリオ読み込み
    auto scenarioData = JsonHelper::loadJsonFromFile(scenarioPath);
    
    // タイマー初期化（120秒 = 2分）
    CountdownTimer::Timer timer(120);
    
    // 入力記録開始
    InputRecorder::Recorder recorder;
    recorder.startSession();
    
    // 統計計算初期化
    Statistics::Calculator statsCalc;
    uint64_t startTime = WinTimer::now_us();
    statsCalc.startSession(startTime);
    
    // かな入力追跡
    RomajiConverter::Converter romajiConv;
    std::string currentRomajiBuffer;
    uint64_t kanaStartTime = 0;
    
    // キーリピート防止
    bool lastKeyStates[256] = {false};
    
    // タイマー開始
    timer.start();
    
    // 画面初期化
    Terminal::clearScreen();
    auto size = Terminal::getTerminalSize();
    
    // 現在のエントリインデックス
    int currentEntryIndex = 0;  // 配列は0から開始
    int totalEntries = 0;
    
    // エントリ配列を取得
    std::vector<JsonHelper::JsonValue> entries;
    if (scenarioData.isObject() && scenarioData["entries"].isArray()) {
        entries = scenarioData["entries"].asArray();
        totalEntries = entries.size();
    }

    // エントリをシャッフル（ランダム出題）
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(entries.begin(), entries.end(), g);
    
    // 最初のエントリを取得
    std::string targetText = "";
    std::string targetRubi = "";
    
    if (totalEntries > 0) {
        auto entry = entries[currentEntryIndex];
        if (entry["text"].isString()) {
            targetText = entry["text"].asString();
        }
        if (entry["ruby"].isString()) {  // "ruby"フィールド
            std::string rawRubi = entry["ruby"].asString();
            // かな→ローマ字変換
            targetRubi = RomajiConverter::Converter::toRomaji(rawRubi);
        }
    }
    
    // タイピング判定初期化
    TypingJudge::Judge judge(targetText, targetRubi);
    
    // メインループ
    while (!timer.isTimeUp()) {
        // 画面更新
        Terminal::SetConsoleCursorPosition(0, 0);
        
        // ヘッダー情報
        Terminal::setTextColor(Terminal::LIGHT_CYAN);
        std::cout << "実験モード - テスト " << testNumber << "/6";
        Terminal::resetTextColor();
        std::cout << " | 被験者: " << session.subject_id 
                  << " | セッション: " << session.session_number << std::endl;
        
        // 配列タイプ（色付き）
        std::cout << "配列: ";
        if (session.layout_type == "new") {
            Terminal::setTextColor(Terminal::LIGHT_GREEN);
        } else {
            Terminal::setTextColor(Terminal::LIGHT_BLUE);
        }
        std::cout << WaitingScreen::getLayoutName(session.layout_type);
        Terminal::resetTextColor();
        std::cout << std::endl;
        
        // タイマー表示
        int remaining = timer.getRemainingSeconds();
        std::cout << "残り時間: " << remaining << " 秒" << std::endl;
        
        // 進捗表示
        std::cout << "単語: " << (currentEntryIndex + 1) << "/" << totalEntries << std::endl;
        std::cout << std::endl;
        
        // 目標テキスト表示
        std::cout << "目標: " << targetText << Terminal::Value_to_Blank(size.width - 10 - Terminal::getDisplayWidth(targetText), " ") << std::endl;
        std::cout << "ルビ: " << targetRubi << Terminal::Value_to_Blank(size.width - 10 - targetRubi.length(), " ") << std::endl;
        
        // 進捗表示（色分け）
        std::cout << "進捗: ";
        std::string typedPart = targetRubi.substr(0, judge.getCurrentPosition());
        std::string remainingPart = targetRubi.substr(judge.getCurrentPosition());
        
        Terminal::setTextColor(Terminal::GRAY);
        std::cout << typedPart;
        Terminal::resetTextColor();
        std::cout << remainingPart;
        
        // 行末クリア
        std::cout << Terminal::Value_to_Blank(size.width - 10 - targetRubi.length(), " ") << std::endl;
        std::cout << std::endl;
        
        // ESCで中断
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
        
        // 文字入力処理（簡略版 - typing_modeから移植）
        for (int key = 32; key <= 126; ++key) {
            if (key == VK_ESCAPE) continue;
            
            bool keyPressed = (GetAsyncKeyState(key) & 0x8000) != 0;
            
            if (keyPressed && !lastKeyStates[key]) {
                lastKeyStates[key] = true;
                
                char ch = static_cast<char>(key);
                if (ch >= 'A' && ch <= 'Z') {
                    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                    if (!shift) ch += 32;
                }
                
                recorder.recordKeyDown(key, 0, ch);
                uint64_t keyDownTime = WinTimer::now_us();
                
                auto result = judge.judgeChar(ch);
                recorder.setLastEventCorrectness(result == TypingJudge::JudgeResult::CORRECT);
                
                if (result == TypingJudge::JudgeResult::CORRECT) {
                    if (currentRomajiBuffer.empty()) {
                        kanaStartTime = keyDownTime;
                    }
                    currentRomajiBuffer += ch;
                    
                    auto convertResult = romajiConv.convert(currentRomajiBuffer);
                    if (convertResult.status == RomajiConverter::ConvertStatus::MATCHED) {
                        uint64_t keyUpTime = WinTimer::now_us();
                        statsCalc.recordKanaInput(convertResult.kana, currentRomajiBuffer, 
                                                  kanaStartTime, keyUpTime);
                        currentRomajiBuffer.clear();
                        kanaStartTime = 0;
                    }
                } else if (result == TypingJudge::JudgeResult::INCORRECT) {
                    currentRomajiBuffer.clear();
                    kanaStartTime = 0;
                }
                
                recorder.recordKeyUp(key, 0);
                
                // 単語完了 → 次の単語へ
                if (judge.isCompleted()) {
                    currentEntryIndex++;
                    if (currentEntryIndex >= totalEntries) {
                        break; // 全単語完了
                    }
                    
                    // 次の単語を取得
                    auto entry = entries[currentEntryIndex];
                    if (entry["text"].isString() && entry["ruby"].isString()) {
                        targetText = entry["text"].asString();
                        std::string rawRubi = entry["ruby"].asString();
                        targetRubi = RomajiConverter::Converter::toRomaji(rawRubi);
                        judge = TypingJudge::Judge(targetText, targetRubi);
                    } else {
                        // エラー：テキスト取得失敗
                        targetText = "Error: Invalid Entry";
                        targetRubi = "error";
                        judge = TypingJudge::Judge(targetText, targetRubi);
                    }
                }
                
                break;
            } else if (!keyPressed && lastKeyStates[key]) {
                lastKeyStates[key] = false;
            }
        }
        
        // ハイフンキー処理
        bool hyphenKeyPressed = (GetAsyncKeyState(0xBD) & 0x8000) != 0;
        if (hyphenKeyPressed && !lastKeyStates[0xBD]) {
            lastKeyStates[0xBD] = true;
            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            char ch = shift ? '_' : '-';
            
            recorder.recordKeyDown(0xBD, 0, ch);
            uint64_t keyDownTime = WinTimer::now_us();
            
            auto result = judge.judgeChar(ch);
            recorder.setLastEventCorrectness(result == TypingJudge::JudgeResult::CORRECT);
            
            if (result == TypingJudge::JudgeResult::CORRECT) {
                if (currentRomajiBuffer.empty()) {
                    kanaStartTime = keyDownTime;
                }
                currentRomajiBuffer += ch;
                
                auto convertResult = romajiConv.convert(currentRomajiBuffer);
                if (convertResult.status == RomajiConverter::ConvertStatus::MATCHED) {
                    uint64_t keyUpTime = WinTimer::now_us();
                    statsCalc.recordKanaInput(convertResult.kana, currentRomajiBuffer, 
                                              kanaStartTime, keyUpTime);
                    currentRomajiBuffer.clear();
                    kanaStartTime = 0;
                }
            } else if (result == TypingJudge::JudgeResult::INCORRECT) {
                currentRomajiBuffer.clear();
                kanaStartTime = 0;
            }
            
            recorder.recordKeyUp(0xBD, 0);
            
            if (judge.isCompleted()) {
                currentEntryIndex++;
                if (currentEntryIndex >= totalEntries) {
                    break;
                }
                
                auto entry = entries[currentEntryIndex];
                if (entry["text"].isString() && entry["ruby"].isString()) {
                    targetText = entry["text"].asString();
                    std::string rawRubi = entry["ruby"].asString();
                    targetRubi = RomajiConverter::Converter::toRomaji(rawRubi);
                    judge = TypingJudge::Judge(targetText, targetRubi);
                }
            }
        } else if (!hyphenKeyPressed && lastKeyStates[0xBD]) {
            lastKeyStates[0xBD] = false;
        }
        
        Sleep(10);
    }
    
    // セッション終了
    timer.stop();
    recorder.endSession();
    
    uint64_t endTime = WinTimer::now_us();
    statsCalc.endSession(endTime);
    
    // 統計計算
    const auto& events = recorder.getEvents();
    for (const auto& event : events) {
        if (event.type == InputRecorder::EventType::KEY_DOWN) {
            statsCalc.recordKeyDown(event.timestamp_us, event.vk_code, event.character);
        } else if (event.type == InputRecorder::EventType::KEY_UP) {
            statsCalc.recordKeyUp(event.timestamp_us, event.vk_code);
        } else if (event.type == InputRecorder::EventType::BACKSPACE) {
            statsCalc.recordBackspace(event.timestamp_us);
        }
    }
    
    auto stats = statsCalc.calculate(judge.getCorrectCount(), judge.getIncorrectCount());

    // CSV出力（実験データ用）
    CSVLogger::ExperimentData expData;
    expData.subject_id = session.subject_id;
    expData.session_number = session.session_number;
    expData.test_number = testNumber;
    expData.layout_type = session.layout_type;
    
    std::string csvPath = CSVLogger::appendSummaryCSV(stats, expData, "output");
    
    // 結果表示
    Terminal::clearScreen();
    Terminal::SetConsoleCursorPosition(0, 3);
    std::cout << "\n";
    std::cout << "  テスト " << testNumber << " 完了\n";
    std::cout << "\n";
    std::cout << "  正解率: " << static_cast<int>((double)stats.correctKeyCount / 
        (stats.correctKeyCount + stats.incorrectKeyCount) * 100) << "%\n";
    std::cout << "  WPM: " << static_cast<int>(stats.wpmCorrect) << "\n";
    std::cout << "  完了単語数: " << currentEntryIndex << "/" << totalEntries << "\n";
    std::cout << "\n";
    
    if (csvPath.empty()) {
        std::cout << "  CSV保存: (エラーまたは重複データのため保存されませんでした)\n";
    } else {
        std::cout << "  CSV保存: " << csvPath << "\n";
    }
    std::cout << "\n";
    
    Sleep(2000);
    
    return 0;
}

// 実験モード: 6テストループ
int experiment_mode() {
    // セッション情報入力
    ExperimentSession session = ExperimentUI::inputExperimentSession();
    
    // キャンセルされた場合
    if (session.subject_id.empty()) {
        return 0;
    }
    
    // 確認画面
    ExperimentUI::showConfirmation(session);
    
    // テストループコントローラー初期化
    // 最初の配列タイプを取得（奇数セッション=new、偶数セッション=old）
    std::string firstLayout = (session.session_number % 2 == 1) ? "new" : "old";
    TestLoopController::Controller controller(firstLayout, session.session_number);
    
    // 6テストループ
    while (!controller.isComplete()) {
        int testNumber = controller.getCurrentTestNumber();
        
        // 配列タイプを取得
        std::string layoutType = controller.getCurrentLayout();
        
        // セッション情報更新
        session.test_order = testNumber;
        session.layout_type = layoutType;
        
        // 2回目以降はEnterキー待機
        if (testNumber > 1) {
            Terminal::clearScreen();
            Terminal::SetConsoleCursorPosition(0, 5);
            std::cout << "\n";
            std::cout << "  次のテストの準備ができました。\n";
            std::cout << "  Enterキーを押して開始してください...\n";
            
            // バッファクリア
            while (_kbhit()) _getch();
            
            // Enterキー待ち
            while (true) {
                int key = _getch();
                if (key == 13) break; // Enter
            }
        }
        
        // 待機画面（3秒カウントダウン）
        WaitingScreen::showCountdown(testNumber, layoutType, 3);
        
        // 1テスト実行
        int result = run_single_experiment_test(session, testNumber);
        
        if (result != 0) {
            // エラーが発生した場合は中断
            ExperimentUI::showError("テスト実行中にエラーが発生しました");
            return -1;
        }
        
        // 次のテストへ
        controller.next();
    }
    
    // 全テスト完了メッセージ
    Terminal::clearScreen();
    Terminal::SetConsoleCursorPosition(0, 5);
    std::cout << "\n";
    Terminal::setTextColor(Terminal::LIGHT_GREEN);
    std::cout << "  *** 全テスト完了！ ***\n";
    Terminal::resetTextColor();
    std::cout << "\n";
    std::cout << "  被験者ID: " << session.subject_id << "\n";
    std::cout << "  セッション番号: " << session.session_number << "\n";
    std::cout << "  完了テスト数: 6/6\n";
    std::cout << "\n";
    std::cout << "  データは output/typing_summary_" << session.subject_id << ".csv に保存されました\n";
    std::cout << "\n";
    std::cout << "  お疲れ様でした！\n";
    std::cout << "\n";
    std::cout << "  (何かキーを押して終了...)\n";
    
    // キー入力待ち
    while (_kbhit()) _getch(); // バッファクリア
    _getch();
    
    return 0;
}


// 判定確認モード（フリータイピング）
int run_input_logic_test() {
    Terminal::clearScreen();
    
    std::cout << "=== 判定確認モード (Free Typing) ===\n";
    std::cout << "ローマ字を入力すると、リアルタイムでかなに変換されます\n";
    std::cout << "ESCキーで終了します\n";
    std::cout << "\n";
    std::cout << "入力開始:\n";
    std::cout << "────────────────────────────────────\n";
    
    // かな変換用
    RomajiConverter::Converter converter;
    std::string romajiBuffer;  // 現在入力中のローマ字バッファ
    std::string convertedKana; // 確定したかな
    
    // キーリピート防止
    bool lastKeyStates[256] = {false};
    
    int displayLine = 6; // 表示開始行
    
    while (true) {
        // ESCで終了
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            Terminal::clearScreen();
            std::cout << "判定確認モードを終了します\n";
            Sleep(1000);
            return 0;
        }
        
        // Backspace処理
        if (GetAsyncKeyState(VK_BACK) & 0x8000) {
            if (!romajiBuffer.empty()) {
                // ローマ字バッファから1文字削除
                romajiBuffer.pop_back();
            } else if (!convertedKana.empty()) {
                // 確定済みかなから削除（最後の文字を取得してローマ字に戻す）
                // 簡易実装: 最後の1文字（UTF-8で3バイト）を削除
                if (convertedKana.length() >= 3) {
                    convertedKana.erase(convertedKana.length() - 3);
                }
            }
            
            // 画面更新
            Terminal::SetConsoleCursorPosition(0, displayLine);
            std::cout << "かな: " << convertedKana;
            Terminal::setTextColor(Terminal::GRAY);
            std::cout << romajiBuffer;
            Terminal::resetTextColor();
            std::cout << "        \n"; // 行末クリア（余分な文字を消す）
            
            Terminal::SetConsoleCursorPosition(0, displayLine + 1);
            std::cout << "ローマ字: " << convertedKana << romajiBuffer << "        \n";
            
            // キーが離されるまで待つ
            while (GetAsyncKeyState(VK_BACK) & 0x8000) Sleep(10);
        }
        
        // キー入力処理
        for (int key = 32; key <= 126; ++key) {
            if (key == VK_ESCAPE) continue;
            
            bool keyPressed = (GetAsyncKeyState(key) & 0x8000) != 0;
            
            if (keyPressed && !lastKeyStates[key]) {
                lastKeyStates[key] = true;
                
                char ch = static_cast<char>(key);
                if (ch >= 'A' && ch <= 'Z') {
                    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                    if (!shift) ch += 32;
                }
                
                // ローマ字バッファに追加
                romajiBuffer += ch;
                
                // かな変換を試行
                auto result = converter.convert(romajiBuffer);
                
                if (result.status == RomajiConverter::ConvertStatus::MATCHED) {
                    // かな確定
                    convertedKana += result.kana;
                    romajiBuffer = result.remaining;
                }
                // PARTIAL（入力途中）の場合は何もしない
                
                // 画面更新
                Terminal::SetConsoleCursorPosition(0, displayLine);
                std::cout << "かな: " << convertedKana;
                Terminal::setTextColor(Terminal::GRAY);
                std::cout << romajiBuffer; // 入力途中のローマ字をグレーで表示
                Terminal::resetTextColor();
                std::cout << "   \n"; // 行末クリア
                
                Terminal::SetConsoleCursorPosition(0, displayLine + 1);
                std::cout << "ローマ字: " << convertedKana;
                // ローマ字を再構築（表示用、簡易版）
                std::cout << romajiBuffer << "   \n";
                
            } else if (!keyPressed && lastKeyStates[key]) {
                lastKeyStates[key] = false;
            }
        }
        
        // ハイフンキー処理
        bool hyphenKeyPressed = (GetAsyncKeyState(0xBD) & 0x8000) != 0;
        if (hyphenKeyPressed && !lastKeyStates[0xBD]) {
            lastKeyStates[0xBD] = true;
            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            char ch = shift ? '_' : '-';
            
            romajiBuffer += ch;
            
            auto result = converter.convert(romajiBuffer);
            if (result.status == RomajiConverter::ConvertStatus::MATCHED) {
                convertedKana += result.kana;
                romajiBuffer = result.remaining;
            }
            
            Terminal::SetConsoleCursorPosition(0, displayLine);
            std::cout << "かな: " << convertedKana;
            Terminal::setTextColor(Terminal::GRAY);
            std::cout << romajiBuffer;
            Terminal::resetTextColor();
            std::cout << "   \n";
            
            Terminal::SetConsoleCursorPosition(0, displayLine + 1);
            std::cout << "ローマ字: " << convertedKana << romajiBuffer << "   \n";
            
        } else if (!hyphenKeyPressed && lastKeyStates[0xBD]) {
            lastKeyStates[0xBD] = false;
        }
        
        Sleep(10);
    }
    
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

    //初期化終了
    
    // モード選択
    ExperimentUI::Mode mode = ExperimentUI::selectMode();
    
    if (mode == ExperimentUI::Mode::QUIT) {
        // 終了
        Terminal::clearScreen();
        std::cout << "終了します。\n";
        return 0;
    } else if (mode == ExperimentUI::Mode::EXPERIMENT) {
        // 実験モード
        experiment_mode();
        return 0;
    } else if (mode == ExperimentUI::Mode::INPUT_TEST) {
        // 判定確認モード
        run_input_logic_test();
        return 0;
    }
    
    // 通常モード（既存の動作）
    const char* version = "0.0.0 - Dummy Version";
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    fs::path exePath(path);
    fs::path exeDir = exePath.parent_path();
    fs::path scenarioDir = exeDir / "scenario";

    // 起動時にファイル選択
    std::string selectedFile = select_file(scenarioDir);
    if (!selectedFile.empty()) {
        Terminal::overwriteString(0, 0, "Returned from viewing file: " + fs::path(selectedFile).string());
        Terminal::overwriteString(0, 1, "Returning from viewing filepath: " + fs::path(selectedFile).filename().string());
        view_mode(selectedFile);
        
        // 画面クリア
        Terminal::clearScreen();
        // Escキーが離されるまで待つ
        while (GetAsyncKeyState(VK_ESCAPE) & 0x8000) Sleep(1);
    }

    // UI再描画
    initialized_UI(version);

    typing_mode();

    return 0;
}

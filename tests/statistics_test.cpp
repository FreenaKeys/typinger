// statistics_test.cpp
// 統計計算モジュールのユニットテスト

#include "../core/statistics.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace Statistics;

// テスト用のヘルパー関数
bool doubleEquals(double a, double b, double epsilon = 0.01) {
    return std::abs(a - b) < epsilon;
}

// テスト1: 基本的なWPM計算
void test_basic_wpm() {
    std::cout << "Test: Basic WPM calculation..." << std::endl;
    
    Calculator calc;
    calc.startSession(0);
    
    // 60秒間に50文字入力（= 10単語）
    for (int i = 0; i < 50; ++i) {
        calc.recordKeyDown(i * 1200000, 'A' + (i % 26), 'a' + (i % 26));
        calc.recordKeyUp(i * 1200000 + 100000, 'A' + (i % 26));
    }
    
    calc.endSession(60000000);  // 60秒
    
    auto data = calc.calculate(50, 0);
    
    // 50文字 / 5 = 10単語、60秒 = 1分 → 10 WPM
    assert(doubleEquals(data.wpmTotal, 10.0));
    assert(doubleEquals(data.wpmCorrect, 10.0));
    assert(data.totalKeyCount == 50);
    assert(data.correctKeyCount == 50);
    
    std::cout << "  PASS" << std::endl;
}

// テスト2: CPM計算
void test_cpm_calculation() {
    std::cout << "Test: CPM calculation..." << std::endl;
    
    Calculator calc;
    calc.startSession(0);
    
    // 30秒間に100文字入力
    for (int i = 0; i < 100; ++i) {
        calc.recordKeyDown(i * 300000, 'A', 'a');
        calc.recordKeyUp(i * 300000 + 50000, 'A');
    }
    
    calc.endSession(30000000);  // 30秒
    
    auto data = calc.calculate(100, 0);
    
    // 100文字 / 0.5分 = 200 CPM
    assert(doubleEquals(data.cpmTotal, 200.0));
    assert(doubleEquals(data.cpmCorrect, 200.0));
    
    std::cout << "  PASS" << std::endl;
}

// テスト3: 正解・誤入力の分離
void test_correct_incorrect_separation() {
    std::cout << "Test: Correct/Incorrect separation..." << std::endl;
    
    Calculator calc;
    calc.startSession(0);
    
    // 60秒間に100文字入力（80正解、20誤入力）
    for (int i = 0; i < 100; ++i) {
        calc.recordKeyDown(i * 600000, 'A', 'a');
        calc.recordKeyUp(i * 600000 + 50000, 'A');
    }
    
    calc.endSession(60000000);  // 60秒
    
    auto data = calc.calculate(80, 20);
    
    // 総WPM: 100/5/1 = 20
    assert(doubleEquals(data.wpmTotal, 20.0));
    // 正答WPM: 80/5/1 = 16
    assert(doubleEquals(data.wpmCorrect, 16.0));
    assert(data.correctKeyCount == 80);
    assert(data.incorrectKeyCount == 20);
    
    std::cout << "  PASS" << std::endl;
}

// テスト4: キー間隔の計算
void test_inter_key_interval() {
    std::cout << "Test: Inter-key interval..." << std::endl;
    
    Calculator calc;
    calc.startSession(0);
    
    // 3つのキーを100ms間隔で入力
    calc.recordKeyDown(0, 'A', 'a');
    calc.recordKeyUp(50000, 'A');
    
    calc.recordKeyDown(100000, 'B', 'b');  // +100ms
    calc.recordKeyUp(150000, 'B');
    
    calc.recordKeyDown(200000, 'C', 'c');  // +100ms
    calc.recordKeyUp(250000, 'C');
    
    calc.endSession(300000);
    
    auto data = calc.calculate(3, 0);
    
    // 平均: 100ms、最小: 100ms、最大: 100ms
    assert(doubleEquals(data.avgInterKeyInterval, 100.0));
    assert(doubleEquals(data.minInterKeyInterval, 100.0));
    assert(doubleEquals(data.maxInterKeyInterval, 100.0));
    
    std::cout << "  PASS" << std::endl;
}

// テスト5: 不均等なキー間隔
void test_uneven_intervals() {
    std::cout << "Test: Uneven intervals..." << std::endl;
    
    Calculator calc;
    calc.startSession(0);
    
    calc.recordKeyDown(0, 'A', 'a');
    calc.recordKeyDown(50000, 'B', 'b');   // +50ms
    calc.recordKeyDown(200000, 'C', 'c');  // +150ms
    calc.recordKeyDown(250000, 'D', 'd');  // +50ms
    
    calc.endSession(300000);
    
    auto data = calc.calculate(4, 0);
    
    // 間隔: 50ms, 150ms, 50ms
    // 平均: (50+150+50)/3 = 83.33ms
    assert(doubleEquals(data.avgInterKeyInterval, 83.33, 0.1));
    assert(doubleEquals(data.minInterKeyInterval, 50.0));
    assert(doubleEquals(data.maxInterKeyInterval, 150.0));
    
    std::cout << "  PASS" << std::endl;
}

// テスト6: キー押下時間の計算
void test_key_press_duration() {
    std::cout << "Test: Key press duration..." << std::endl;
    
    Calculator calc;
    calc.startSession(0);
    
    // 'a'を3回入力（50ms, 100ms, 150ms押下）
    calc.recordKeyDown(0, 'A', 'a');
    calc.recordKeyUp(50000, 'A');          // 50ms
    
    calc.recordKeyDown(100000, 'A', 'a');
    calc.recordKeyUp(200000, 'A');         // 100ms
    
    calc.recordKeyDown(250000, 'A', 'a');
    calc.recordKeyUp(400000, 'A');         // 150ms
    
    calc.endSession(500000);
    
    auto data = calc.calculate(3, 0);
    
    // 平均押下時間: (50+100+150)/3 = 100ms
    assert(data.avgKeyPressDuration.count('a') == 1);
    assert(doubleEquals(data.avgKeyPressDuration.at('a'), 100.0));
    
    std::cout << "  PASS" << std::endl;
}

// テスト7: Backspaceカウント
void test_backspace_count() {
    std::cout << "Test: Backspace count..." << std::endl;
    
    Calculator calc;
    calc.startSession(0);
    
    calc.recordKeyDown(0, 'A', 'a');
    calc.recordKeyDown(100000, 'B', 'b');
    calc.recordBackspace(200000);
    calc.recordKeyDown(300000, 'C', 'c');
    calc.recordBackspace(400000);
    calc.recordBackspace(500000);
    
    calc.endSession(600000);
    
    auto data = calc.calculate(3, 0);
    
    assert(data.totalKeyCount == 3);
    assert(data.backspaceCount == 3);
    
    std::cout << "  PASS" << std::endl;
}

// テスト8: 空データ（イベントなし）
void test_empty_data() {
    std::cout << "Test: Empty data..." << std::endl;
    
    Calculator calc;
    calc.startSession(0);
    calc.endSession(1000000);
    
    auto data = calc.calculate(0, 0);
    
    assert(data.totalKeyCount == 0);
    assert(data.correctKeyCount == 0);
    assert(data.wpmTotal == 0.0);
    assert(data.avgInterKeyInterval == 0.0);
    
    std::cout << "  PASS" << std::endl;
}

// テスト9: リセット機能
void test_reset() {
    std::cout << "Test: Reset..." << std::endl;
    
    Calculator calc;
    calc.startSession(0);
    calc.recordKeyDown(0, 'A', 'a');
    calc.recordKeyDown(100000, 'B', 'b');
    calc.endSession(200000);
    
    assert(calc.getEventCount() == 2);
    
    calc.reset();
    
    assert(calc.getEventCount() == 0);
    
    std::cout << "  PASS" << std::endl;
}

int main() {
    std::cout << "=== Statistics Calculator Unit Tests ===" << std::endl;
    std::cout << std::endl;
    
    test_basic_wpm();
    test_cpm_calculation();
    test_correct_incorrect_separation();
    test_inter_key_interval();
    test_uneven_intervals();
    test_key_press_duration();
    test_backspace_count();
    test_empty_data();
    test_reset();
    
    std::cout << std::endl;
    std::cout << "All tests passed! ✓" << std::endl;
    
    return 0;
}

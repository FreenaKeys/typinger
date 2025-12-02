#include "../core/countdown_timer.h"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>

// 浮動小数点の比較用ヘルパー
bool isClose(double a, double b, double epsilon = 0.1) {
    return std::abs(a - b) < epsilon;
}

void test_TimerConstruction() {
    std::cout << "Testing timer construction..." << std::endl;
    
    // デフォルト（120秒）
    CountdownTimer::Timer timer1;
    assert(timer1.running() == false);
    assert(isClose(timer1.getRemainingSeconds(), 120.0, 0.5));
    
    // カスタム時間（60秒）
    CountdownTimer::Timer timer2(60);
    assert(timer2.running() == false);
    assert(isClose(timer2.getRemainingSeconds(), 60.0, 0.5));
    
    // カスタム時間（10秒）
    CountdownTimer::Timer timer3(10);
    assert(timer3.running() == false);
    assert(isClose(timer3.getRemainingSeconds(), 10.0, 0.5));
    
    std::cout << "✓ All timer construction tests passed" << std::endl;
}

void test_TimerStartStop() {
    std::cout << "Testing timer start/stop..." << std::endl;
    
    CountdownTimer::Timer timer(10);
    
    // 開始前
    assert(timer.running() == false);
    assert(timer.getElapsedSeconds() == 0.0);
    
    // 開始
    timer.start();
    assert(timer.running() == true);
    
    // 少し待機
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 経過時間が増えていることを確認
    double elapsed = timer.getElapsedSeconds();
    assert(elapsed > 0.0);
    assert(elapsed < 1.0);  // 100msなので1秒未満
    
    // 停止
    timer.stop();
    assert(timer.running() == false);
    
    std::cout << "✓ All timer start/stop tests passed" << std::endl;
}

void test_ElapsedAndRemaining() {
    std::cout << "Testing elapsed and remaining time..." << std::endl;
    
    CountdownTimer::Timer timer(5);  // 5秒タイマー
    timer.start();
    
    // 1秒待機
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    double elapsed = timer.getElapsedSeconds();
    double remaining = timer.getRemainingSeconds();
    
    std::cout << "  Elapsed: " << elapsed << "s, Remaining: " << remaining << "s" << std::endl;
    
    // 約1秒経過しているはず
    assert(isClose(elapsed, 1.0, 0.2));
    
    // 約4秒残っているはず
    assert(isClose(remaining, 4.0, 0.2));
    
    // 経過時間 + 残り時間 ≈ 制限時間
    assert(isClose(elapsed + remaining, 5.0, 0.3));
    
    std::cout << "✓ All elapsed and remaining time tests passed" << std::endl;
}

void test_Progress() {
    std::cout << "Testing progress..." << std::endl;
    
    CountdownTimer::Timer timer(4);  // 4秒タイマー
    timer.start();
    
    // 1秒待機
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    double progress = timer.getProgress();
    std::cout << "  Progress after 1s: " << (progress * 100) << "%" << std::endl;
    
    // 約25%進んでいるはず（1秒/4秒）
    assert(isClose(progress, 0.25, 0.1));
    
    // さらに1秒待機
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    progress = timer.getProgress();
    std::cout << "  Progress after 2s: " << (progress * 100) << "%" << std::endl;
    
    // 約50%進んでいるはず（2秒/4秒）
    assert(isClose(progress, 0.5, 0.1));
    
    std::cout << "✓ All progress tests passed" << std::endl;
}

void test_TimeUp() {
    std::cout << "Testing time up detection..." << std::endl;
    
    CountdownTimer::Timer timer(2);  // 2秒タイマー
    timer.start();
    
    // 開始直後はタイムアップしていない
    assert(timer.isTimeUp() == false);
    
    // 1秒待機
    std::this_thread::sleep_for(std::chrono::seconds(1));
    assert(timer.isTimeUp() == false);
    
    // さらに1.5秒待機（合計2.5秒）
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    
    // タイムアップしているはず
    assert(timer.isTimeUp() == true);
    
    // 残り時間は0
    double remaining = timer.getRemainingSeconds();
    std::cout << "  Remaining after time up: " << remaining << "s" << std::endl;
    assert(remaining == 0.0);
    
    // 進捗率は100%
    double progress = timer.getProgress();
    std::cout << "  Progress after time up: " << (progress * 100) << "%" << std::endl;
    assert(isClose(progress, 1.0, 0.05));
    
    std::cout << "✓ All time up detection tests passed" << std::endl;
}

void test_Reset() {
    std::cout << "Testing timer reset..." << std::endl;
    
    CountdownTimer::Timer timer(10);
    timer.start();
    
    // 1秒待機
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 経過時間があることを確認
    assert(timer.getElapsedSeconds() > 0.0);
    assert(timer.running() == true);
    
    // リセット
    timer.reset();
    
    // 停止して初期状態に戻る
    assert(timer.running() == false);
    assert(timer.getElapsedSeconds() == 0.0);
    
    std::cout << "✓ All timer reset tests passed" << std::endl;
}

void test_MultipleStarts() {
    std::cout << "Testing multiple starts..." << std::endl;
    
    CountdownTimer::Timer timer(5);
    
    // 1回目の開始
    timer.start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    double elapsed1 = timer.getElapsedSeconds();
    
    std::cout << "  First elapsed: " << elapsed1 << "s" << std::endl;
    
    // 1秒以上経過していることを確認
    assert(elapsed1 >= 1.0);
    
    // 2回目の開始（再スタート）
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    double elapsed2 = timer.getElapsedSeconds();
    
    std::cout << "  Second elapsed (after restart): " << elapsed2 << "s" << std::endl;
    
    // 2回目は時間がリセットされているので、約0.5秒のはず
    assert(isClose(elapsed2, 0.5, 0.2));
    // 2回目は1回目より明らかに短いはず
    assert(elapsed2 < 0.8);
    
    std::cout << "✓ All multiple starts tests passed" << std::endl;
}

int main() {
    std::cout << "=== Countdown Timer Tests ===" << std::endl;
    std::cout << "Note: These tests involve actual time delays and may take ~10 seconds" << std::endl;
    std::cout << std::endl;
    
    test_TimerConstruction();
    test_TimerStartStop();
    test_ElapsedAndRemaining();
    test_Progress();
    test_TimeUp();
    test_Reset();
    test_MultipleStarts();
    
    std::cout << "\n✓✓✓ All tests passed! ✓✓✓" << std::endl;
    return 0;
}

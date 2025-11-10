#include <iostream>
#include <thread>
#include <chrono>
#include "../helper/WinAPI/timer.h"

// タイマーの簡易スモークテスト
// 目的: 100ms/300ms の経過時間が、おおよそ±10msの範囲で測定できることを確認
int main() {
    WinTimer::init();

    // 1) now_us() が単調増加するかの簡易確認
    auto t1 = WinTimer::now_us();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    auto t2 = WinTimer::now_us();
    std::cout << "now_us monotonic: " << (t2 > t1 ? "OK" : "NG")
              << " (" << t1 << " -> " << t2 << ")\n";

    // 2) Stopwatch で 100ms を計測
    WinTimer::Stopwatch sw1;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto e1 = sw1.elapsed_us();
    std::cout << "100ms expected, measured(us): " << e1 << "\n";

    // 3) Stopwatch で 300ms を計測
    WinTimer::Stopwatch sw2;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    auto e2 = sw2.elapsed_us();
    std::cout << "300ms expected, measured(us): " << e2 << "\n";

    // しきい値±20ms（=20000us）の簡易判定
    // 注: Windows環境ではスレッドスリープの精度が約15.6msのため、余裕を持たせる
    auto within = [](std::uint64_t actual_us, std::uint64_t expect_ms) {
        const std::uint64_t expect_us = expect_ms * 1000ULL;
        const std::uint64_t tol = 20000ULL; // 20ms 許容
        return (actual_us > expect_us - tol) && (actual_us < expect_us + tol);
    };

    bool pass = (t2 > t1) && within(e1, 100) && within(e2, 300);
    std::cout << (pass ? "RESULT: PASS" : "RESULT: FAIL") << "\n";
    return pass ? 0 : 1;
}

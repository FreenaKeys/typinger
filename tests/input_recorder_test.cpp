#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>  // VK_BACK などの定数のため
#include "../core/input_recorder.h"

// InputRecorder の簡易スモークテスト
// 目的: イベント記録、タイムスタンプ、キー間隔の計算が正しく動作するか確認
int main() {
    using namespace InputRecorder;
    
    Recorder recorder;
    
    // テスト1: セッション開始と基本記録
    std::cout << "Test 1: Session start and basic recording\n";
    recorder.startSession();
    
    if (!recorder.isRecording()) {
        std::cout << "  FAIL: Not recording after startSession\n";
        return 1;
    }
    std::cout << "  OK: Recording started\n";
    
    // テスト2: キーダウン/キーアップの記録
    std::cout << "\nTest 2: KeyDown/KeyUp recording\n";
    recorder.recordKeyDown('A', 30, 'a');  // VK_A = 65
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    recorder.recordKeyUp('A', 30);
    
    if (recorder.getEventCount() != 2) {
        std::cout << "  FAIL: Expected 2 events, got " << recorder.getEventCount() << "\n";
        return 1;
    }
    std::cout << "  OK: Recorded 2 events (KeyDown + KeyUp)\n";
    
    // テスト3: キー間隔の計算
    std::cout << "\nTest 3: Inter-key timing\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    recorder.recordKeyDown('B', 48, 'b');
    recorder.recordKeyUp('B', 48);
    
    const auto& events = recorder.getEvents();
    if (events.size() >= 3) {
        uint64_t inter_key = events[2].inter_key_time_us;  // 3つ目のイベント（2番目のKeyDown）
        std::cout << "  Inter-key time: " << inter_key << " us (expected ~50000)\n";
        
        // 30ms〜70msの範囲を許容
        if (inter_key < 30000 || inter_key > 70000) {
            std::cout << "  FAIL: Inter-key time out of range\n";
            return 1;
        }
        std::cout << "  OK: Inter-key time within range\n";
    }
    
    // テスト4: Backspace記録
    std::cout << "\nTest 4: Backspace recording\n";
    recorder.recordBackspace();
    recorder.recordCorrection(5000);  // 5ms の修正時間
    
    if (recorder.getEventCount() != 6) {
        std::cout << "  FAIL: Expected 6 events, got " << recorder.getEventCount() << "\n";
        return 1;
    }
    std::cout << "  OK: Backspace and correction recorded\n";
    
    // テスト5: セッション終了
    std::cout << "\nTest 5: Session end\n";
    recorder.endSession();
    
    if (recorder.isRecording()) {
        std::cout << "  FAIL: Still recording after endSession\n";
        return 1;
    }
    
    uint64_t duration = recorder.getSessionDuration();
    std::cout << "  Session duration: " << duration << " us\n";
    std::cout << "  OK: Session ended\n";
    
    // テスト6: イベントの詳細確認
    std::cout << "\nTest 6: Event details\n";
    std::cout << "  Total events: " << events.size() << "\n";
    for (size_t i = 0; i < events.size(); ++i) {
        std::cout << "  Event " << i << ": ";
        switch (events[i].type) {
            case EventType::KEY_DOWN: std::cout << "KEY_DOWN"; break;
            case EventType::KEY_UP: std::cout << "KEY_UP"; break;
            case EventType::BACKSPACE: std::cout << "BACKSPACE"; break;
            case EventType::CORRECTION: std::cout << "CORRECTION"; break;
        }
        std::cout << " vk=" << events[i].vk_code 
                  << " char='" << events[i].character 
                  << "' ts=" << events[i].timestamp_us << "\n";
    }
    
    std::cout << "\nRESULT: PASS\n";
    return 0;
}

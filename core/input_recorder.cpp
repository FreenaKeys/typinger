#include "input_recorder.h"
#include "../helper/WinAPI/timer.h"
#include <windows.h>  // VK_BACK などの定数のため

namespace InputRecorder {

    // InputEvent のコンストラクタ（初期化）
    InputEvent::InputEvent() 
        : type(EventType::KEY_DOWN)
        , timestamp_us(0)
        , vk_code(0)
        , scan_code(0)
        , character('\0')
        , is_correct(false)
        , inter_key_time_us(0)
        , note("")
    {}

    InputEvent::InputEvent(EventType t, uint64_t ts, int vk, int scan, char ch)
        : type(t)
        , timestamp_us(ts)
        , vk_code(vk)
        , scan_code(scan)
        , character(ch)
        , is_correct(false)
        , inter_key_time_us(0)
        , note("")
    {}

    // Recorder のコンストラクタ
    Recorder::Recorder()
        : session_start_us_(0)
        , last_keyup_time_us_(0)
        , recording_(false)
    {
        WinTimer::init();  // タイマーを初期化
    }

    void Recorder::startSession() {
        clear();  // 前のデータをクリア
        session_start_us_ = WinTimer::now_us();  // 現在時刻を記録
        last_keyup_time_us_ = session_start_us_;
        recording_ = true;
    }

    void Recorder::endSession() {
        recording_ = false;
    }

    void Recorder::recordKeyDown(int vk, int scan, char ch) {
        if (!recording_) return;  // 記録中でなければ何もしない

        uint64_t now = WinTimer::now_us();
        InputEvent evt(EventType::KEY_DOWN, now, vk, scan, ch);

        // 直前のキーアップからの経過時間（キー間隔）を計算
        if (last_keyup_time_us_ > 0) {
            evt.inter_key_time_us = now - last_keyup_time_us_;
        }

        events_.push_back(evt);
    }

    void Recorder::recordKeyUp(int vk, int scan) {
        if (!recording_) return;

        uint64_t now = WinTimer::now_us();
        InputEvent evt(EventType::KEY_UP, now, vk, scan);
        events_.push_back(evt);

        // 最後のキーアップ時刻を更新
        last_keyup_time_us_ = now;
    }

    void Recorder::recordBackspace() {
        if (!recording_) return;

        uint64_t now = WinTimer::now_us();
        InputEvent evt(EventType::BACKSPACE, now, VK_BACK, 0);
        events_.push_back(evt);
    }

    void Recorder::recordCorrection(uint64_t correction_time_us) {
        if (!recording_) return;

        uint64_t now = WinTimer::now_us();
        InputEvent evt(EventType::CORRECTION, now, 0, 0);
        evt.inter_key_time_us = correction_time_us;  // 修正にかかった時間を記録
        evt.note = "correction";
        events_.push_back(evt);
    }

    void Recorder::setLastEventCorrectness(bool is_correct) {
        if (!events_.empty()) {
            events_.back().is_correct = is_correct;
        }
    }

    bool Recorder::isRecording() const {
        return recording_;
    }

    size_t Recorder::getEventCount() const {
        return events_.size();
    }

    const std::vector<InputEvent>& Recorder::getEvents() const {
        return events_;
    }

    uint64_t Recorder::getSessionDuration() const {
        if (session_start_us_ == 0) return 0;
        return WinTimer::now_us() - session_start_us_;
    }

    void Recorder::clear() {
        events_.clear();
        session_start_us_ = 0;
        last_keyup_time_us_ = 0;
        recording_ = false;
    }

} // namespace InputRecorder

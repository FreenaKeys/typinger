#pragma once

// countdown_timer.h
// 実験モード用カウントダウンタイマー（Phase 6で追加）
//
// 用途：2分間（120秒）のタイピングテスト時間管理

#include <cstdint>

namespace CountdownTimer {

    // カウントダウンタイマークラス
    class Timer {
    private:
        std::uint64_t startTime_us;      // 開始時刻（マイクロ秒）
        std::uint64_t durationLimit_us;  // 制限時間（マイクロ秒）
        bool isRunning;                  // 実行中フラグ

    public:
        // コンストラクタ
        // durationSeconds: 制限時間（秒）
        Timer(int durationSeconds = 120);

        // タイマーを開始
        void start();

        // タイマーを停止
        void stop();

        // タイマーをリセット
        void reset();

        // 経過時間を取得（秒）
        double getElapsedSeconds() const;

        // 残り時間を取得（秒）
        double getRemainingSeconds() const;

        // タイムアップしたか確認
        bool isTimeUp() const;

        // タイマーが実行中か確認
        bool running() const;

        // 進捗率を取得（0.0～1.0）
        double getProgress() const;
    };

} // namespace CountdownTimer

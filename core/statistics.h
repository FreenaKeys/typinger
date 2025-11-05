#pragma once

// statistics.h
// タイピング統計計算モジュール
//
// 用語解説:
// - WPM (Words Per Minute): 1分あたりの入力単語数（英語では5文字=1単語）
// - CPM (Characters Per Minute): 1分あたりの入力文字数
// - キー間隔 (Inter-Key Interval): 連続するキー入力間の時間差

#include <vector>
#include <string>
#include <map>
#include <cstdint>

namespace Statistics {

    // キーイベントの種類
    enum class EventType {
        KEY_DOWN,
        KEY_UP,
        BACKSPACE
    };

    // キーイベント情報
    struct KeyEvent {
        EventType type;
        uint64_t timestamp;     // マイクロ秒単位
        int virtualKey;         // 仮想キーコード
        char character;         // 実際の文字（ASCII）
        
        KeyEvent(EventType t, uint64_t ts, int vk, char ch)
            : type(t), timestamp(ts), virtualKey(vk), character(ch) {}
    };

    // 統計データ
    struct StatisticsData {
        // 基本情報
        uint64_t totalDuration;         // 総時間（マイクロ秒）
        size_t totalKeyCount;           // 総キー入力数（Backspace含む）
        size_t correctKeyCount;         // 正解キー数
        size_t incorrectKeyCount;       // 誤入力キー数
        size_t backspaceCount;          // Backspace回数
        
        // WPM/CPM
        double wpmTotal;                // 総入力ベースWPM（総文字数/5/分）
        double wpmCorrect;              // 正答ベースWPM（正解文字数/5/分）
        double cpmTotal;                // 総入力ベースCPM
        double cpmCorrect;              // 正答ベースCPM
        
        // キー間隔
        double avgInterKeyInterval;     // 平均キー間隔（ミリ秒）
        double minInterKeyInterval;     // 最小キー間隔（ミリ秒）
        double maxInterKeyInterval;     // 最大キー間隔（ミリ秒）
        
        // キー別平均時間（キー押下時間）
        std::map<char, double> avgKeyPressDuration;  // 各文字の平均押下時間（ミリ秒）
        
        // 50音別入力時間（ローマ字入力での統計）
        std::map<std::string, double> kanaInputTime;  // かな→平均入力時間（ミリ秒）
        
        StatisticsData()
            : totalDuration(0)
            , totalKeyCount(0)
            , correctKeyCount(0)
            , incorrectKeyCount(0)
            , backspaceCount(0)
            , wpmTotal(0.0)
            , wpmCorrect(0.0)
            , cpmTotal(0.0)
            , cpmCorrect(0.0)
            , avgInterKeyInterval(0.0)
            , minInterKeyInterval(0.0)
            , maxInterKeyInterval(0.0)
        {}
    };

    // 統計計算クラス
    class Calculator {
    private:
        std::vector<KeyEvent> events_;
        uint64_t sessionStartTime_;
        uint64_t sessionEndTime_;
        
    public:
        Calculator();
        
        // セッション管理
        void startSession(uint64_t startTime);
        void endSession(uint64_t endTime);
        
        // イベント記録
        void recordKeyDown(uint64_t timestamp, int virtualKey, char character);
        void recordKeyUp(uint64_t timestamp, int virtualKey);
        void recordBackspace(uint64_t timestamp);
        
        // 統計計算（judgeResultを使用）
        StatisticsData calculate(size_t correctCount, size_t incorrectCount);
        
        // イベント数取得
        size_t getEventCount() const { return events_.size(); }
        
        // リセット
        void reset();
        
    private:
        // 内部計算関数
        double calculateWPM(size_t charCount, uint64_t duration) const;
        double calculateCPM(size_t charCount, uint64_t duration) const;
        void calculateInterKeyIntervals(StatisticsData& data) const;
        void calculateKeyPressDuration(StatisticsData& data) const;
    };

} // namespace Statistics

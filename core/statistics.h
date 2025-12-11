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

    // Phase 7: 重要かな文字リスト（新配列検証用・30文字）
    const std::vector<std::string> IMPORTANT_KANA = {
        // K行（5文字）- 左手中指への移動効果
        "か", "き", "く", "け", "こ",
        
        // N行+ん（6文字）- 左手人差指への移動効果
        "な", "に", "ぬ", "ね", "の", "ん",
        
        // 母音（5文字）- 右手集中効果
        "あ", "い", "う", "え", "お",
        
        // 高頻度子音（7文字）- 全体的な改善効果
        "し", "た", "と", "て", "さ", "す", "は",
        
        // 拗音（7文字）- 複合打鍵の最適化効果
        "しゃ", "しゅ", "しょ", "きゃ", "きゅ", "ちゃ", "ちゅ"
    };

    // Phase 7a-3: かなカテゴリー定義（出現頻度分析用）
    const std::map<std::string, std::vector<std::string>> KANA_CATEGORIES = {
        {"母音", {"あ", "い", "う", "え", "お"}},
        {"K行", {"か", "き", "く", "け", "こ"}},
        {"S行", {"さ", "し", "す", "せ", "そ"}},
        {"T行", {"た", "ち", "つ", "て", "と"}},
        {"N行", {"な", "に", "ぬ", "ね", "の"}},
        {"H行", {"は", "ひ", "ふ", "へ", "ほ"}},
        {"M行", {"ま", "み", "む", "め", "も"}},
        {"Y行", {"や", "ゆ", "よ"}},
        {"R行", {"ら", "り", "る", "れ", "ろ"}},
        {"W行", {"わ", "を", "ん"}},
        {"濁音", {"が", "ぎ", "ぐ", "げ", "ご", "ざ", "じ", "ず", "ぜ", "ぞ", "だ", "ぢ", "づ", "で", "ど", "ば", "び", "ぶ", "べ", "ぼ"}},
        {"半濁音", {"ぱ", "ぴ", "ぷ", "ぺ", "ぽ"}},
        {"拗音", {"きゃ", "きゅ", "きょ", "しゃ", "しゅ", "しょ", "ちゃ", "ちゅ", "ちょ", "にゃ", "にゅ", "にょ", "ひゃ", "ひゅ", "ひょ", "みゃ", "みゅ", "みょ", "りゃ", "りゅ", "りょ", "ぎゃ", "ぎゅ", "ぎょ", "じゃ", "じゅ", "じょ", "びゃ", "びゅ", "びょ", "ぴゃ", "ぴゅ", "ぴょ"}}
    };

    // Phase 7a-3: 出現頻度データ
    struct FrequencyData {
        std::string item;           // 項目名（カテゴリー名 or 文字）
        size_t count;               // 出現回数
        double percentage;          // 全体比率（%）
        
        FrequencyData() : item(""), count(0), percentage(0.0) {}
        FrequencyData(const std::string& i, size_t c, double p)
            : item(i), count(c), percentage(p) {}
    };

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
        
        // Phase 7: 重要かな30文字の入力時間
        std::map<std::string, double> importantKanaIntervals;  // 重要かな→平均入力時間（ミリ秒）
        
        // Phase 7a-3: 出現頻度データ
        std::vector<FrequencyData> categoryFrequency;    // カテゴリー別出現頻度
        std::vector<FrequencyData> importantKanaFrequency;  // 重要30文字の出現頻度
        
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

    // かな入力データ（Phase 3-2）
    struct KanaInputData {
        std::string kana;              // 仮名文字（例: "し", "しゅ"）
        std::string romaji;            // 対応するローマ字（例: "shi", "shu"）
        uint64_t startTime;            // 先頭キーダウン（マイクロ秒）
        uint64_t endTime;              // 最後のキーアップ（マイクロ秒）
        uint64_t duration;             // 所要時間（マイクロ秒）
        
        KanaInputData(const std::string& k, const std::string& r, 
                      uint64_t start, uint64_t end)
            : kana(k), romaji(r), startTime(start), endTime(end)
            , duration(end - start)
        {}
    };

    // 統計計算クラス
    class Calculator {
    private:
        std::vector<KeyEvent> events_;
        uint64_t sessionStartTime_;
        uint64_t sessionEndTime_;
        
        // Phase 3-2: 50音別入力時間
        std::vector<KanaInputData> kanaInputs_;
        
    public:
        Calculator();
        
        // セッション管理
        void startSession(uint64_t startTime);
        void endSession(uint64_t endTime);
        
        // イベント記録
        void recordKeyDown(uint64_t timestamp, int virtualKey, char character);
        void recordKeyUp(uint64_t timestamp, int virtualKey);
        void recordBackspace(uint64_t timestamp);
        
        // Phase 3-2: かな入力記録
        void recordKanaInput(const std::string& kana, const std::string& romaji,
                             uint64_t startTime, uint64_t endTime);
        
        // Phase 3-2: 50音別平均入力時間取得
        std::map<std::string, double> getAvgKanaInputTime() const;
        
        // Phase 7: 重要かなの平均入力時間取得（フィルタリング版）
        std::map<std::string, double> getImportantKanaIntervals() const;
        
        // Phase 7a-3: 出現頻度計算
        std::vector<FrequencyData> calculateCategoryFrequency() const;
        std::vector<FrequencyData> calculateImportantKanaFrequency() const;
        
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

// statistics.cpp
// タイピング統計計算モジュールの実装

#include "statistics.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace Statistics {

    Calculator::Calculator()
        : sessionStartTime_(0)
        , sessionEndTime_(0)
    {
    }

    void Calculator::startSession(uint64_t startTime) {
        sessionStartTime_ = startTime;
        events_.clear();
    }

    void Calculator::endSession(uint64_t endTime) {
        sessionEndTime_ = endTime;
    }

    void Calculator::recordKeyDown(uint64_t timestamp, int virtualKey, char character) {
        events_.emplace_back(EventType::KEY_DOWN, timestamp, virtualKey, character);
    }

    void Calculator::recordKeyUp(uint64_t timestamp, int virtualKey) {
        events_.emplace_back(EventType::KEY_UP, timestamp, virtualKey, '\0');
    }

    void Calculator::recordBackspace(uint64_t timestamp) {
        events_.emplace_back(EventType::BACKSPACE, timestamp, 0x08, '\0');
    }

    StatisticsData Calculator::calculate(size_t correctCount, size_t incorrectCount) {
        StatisticsData data;
        
        // 基本情報
        data.totalDuration = sessionEndTime_ - sessionStartTime_;
        data.correctKeyCount = correctCount;
        data.incorrectKeyCount = incorrectCount;
        
        // イベントから統計を集計
        size_t keyDownCount = 0;
        size_t backspaceCount = 0;
        
        for (const auto& event : events_) {
            if (event.type == EventType::KEY_DOWN) {
                keyDownCount++;
            } else if (event.type == EventType::BACKSPACE) {
                backspaceCount++;
            }
        }
        
        data.totalKeyCount = keyDownCount;
        data.backspaceCount = backspaceCount;
        
        // WPM/CPM計算
        data.wpmTotal = calculateWPM(data.totalKeyCount, data.totalDuration);
        data.wpmCorrect = calculateWPM(data.correctKeyCount, data.totalDuration);
        data.cpmTotal = calculateCPM(data.totalKeyCount, data.totalDuration);
        data.cpmCorrect = calculateCPM(data.correctKeyCount, data.totalDuration);
        
        // キー間隔計算
        calculateInterKeyIntervals(data);
        
        // キー押下時間計算
        calculateKeyPressDuration(data);
        
        // Phase 3-2: 50音別入力時間の計算
        data.kanaInputTime = getAvgKanaInputTime();
        
        // Phase 7: 重要かな30文字の入力時間計算
        data.importantKanaIntervals = getImportantKanaIntervals();
        
        // Phase 7a-3: 出現頻度データ計算
        data.categoryFrequency = calculateCategoryFrequency();
        data.importantKanaFrequency = calculateImportantKanaFrequency();
        
        return data;
    }

    void Calculator::reset() {
        events_.clear();
        kanaInputs_.clear();  // Phase 3-2
        sessionStartTime_ = 0;
        sessionEndTime_ = 0;
    }

    // WPM計算（英語基準: 5文字=1単語）
    double Calculator::calculateWPM(size_t charCount, uint64_t duration) const {
        if (duration == 0) return 0.0;
        
        // マイクロ秒→分に変換
        double minutes = static_cast<double>(duration) / 1000000.0 / 60.0;
        if (minutes == 0.0) return 0.0;
        
        // 5文字=1単語として計算
        double words = static_cast<double>(charCount) / 5.0;
        return words / minutes;
    }

    // CPM計算
    double Calculator::calculateCPM(size_t charCount, uint64_t duration) const {
        if (duration == 0) return 0.0;
        
        // マイクロ秒→分に変換
        double minutes = static_cast<double>(duration) / 1000000.0 / 60.0;
        if (minutes == 0.0) return 0.0;
        
        return static_cast<double>(charCount) / minutes;
    }

    // キー間隔の計算
    void Calculator::calculateInterKeyIntervals(StatisticsData& data) const {
        std::vector<uint64_t> keyDownTimestamps;
        
        // KEY_DOWNイベントのタイムスタンプを収集
        for (const auto& event : events_) {
            if (event.type == EventType::KEY_DOWN) {
                keyDownTimestamps.push_back(event.timestamp);
            }
        }
        
        if (keyDownTimestamps.size() < 2) {
            data.avgInterKeyInterval = 0.0;
            data.minInterKeyInterval = 0.0;
            data.maxInterKeyInterval = 0.0;
            return;
        }
        
        // 連続するキー間の時間差を計算
        std::vector<double> intervals;
        for (size_t i = 1; i < keyDownTimestamps.size(); ++i) {
            uint64_t diff = keyDownTimestamps[i] - keyDownTimestamps[i - 1];
            // マイクロ秒→ミリ秒
            intervals.push_back(static_cast<double>(diff) / 1000.0);
        }
        
        // 平均・最小・最大を計算
        double sum = 0.0;
        double minVal = std::numeric_limits<double>::max();
        double maxVal = 0.0;
        
        for (double interval : intervals) {
            sum += interval;
            minVal = std::min(minVal, interval);
            maxVal = std::max(maxVal, interval);
        }
        
        data.avgInterKeyInterval = sum / static_cast<double>(intervals.size());
        data.minInterKeyInterval = minVal;
        data.maxInterKeyInterval = maxVal;
    }

    // キー押下時間の計算
    void Calculator::calculateKeyPressDuration(StatisticsData& data) const {
        // KEY_DOWNとKEY_UPのペアを見つけて押下時間を計算
        std::map<int, uint64_t> keyDownTime;  // virtualKey -> timestamp
        std::map<char, std::vector<double>> pressDurations;  // character -> durations
        
        for (const auto& event : events_) {
            if (event.type == EventType::KEY_DOWN) {
                keyDownTime[event.virtualKey] = event.timestamp;
            } else if (event.type == EventType::KEY_UP) {
                auto it = keyDownTime.find(event.virtualKey);
                if (it != keyDownTime.end()) {
                    uint64_t downTime = it->second;
                    uint64_t duration = event.timestamp - downTime;
                    
                    // 対応するKEY_DOWNの文字を見つける
                    for (const auto& e : events_) {
                        if (e.type == EventType::KEY_DOWN && 
                            e.virtualKey == event.virtualKey && 
                            e.timestamp == downTime) {
                            // マイクロ秒→ミリ秒
                            pressDurations[e.character].push_back(
                                static_cast<double>(duration) / 1000.0
                            );
                            break;
                        }
                    }
                    
                    keyDownTime.erase(it);
                }
            }
        }
        
        // 各文字の平均押下時間を計算
        for (const auto& pair : pressDurations) {
            char ch = pair.first;
            const auto& durations = pair.second;
            
            if (!durations.empty()) {
                double sum = 0.0;
                for (double d : durations) {
                    sum += d;
                }
                data.avgKeyPressDuration[ch] = sum / static_cast<double>(durations.size());
            }
        }
    }

    // Phase 3-2: かな別入力時間の記録
    void Calculator::recordKanaInput(const std::string& kana, const std::string& romaji,
                                      uint64_t startTime, uint64_t endTime) {
        kanaInputs_.emplace_back(kana, romaji, startTime, endTime);
    }

    // Phase 3-2: かな別平均入力時間の計算
    std::map<std::string, double> Calculator::getAvgKanaInputTime() const {
        std::map<std::string, std::vector<uint64_t>> kanaDurations;
        
        // かなごとに入力時間を集計
        for (const auto& kanaInput : kanaInputs_) {
            kanaDurations[kanaInput.kana].push_back(kanaInput.duration);
        }
        
        // 各かなの平均を計算（ミリ秒単位）
        std::map<std::string, double> avgTimes;
        for (const auto& pair : kanaDurations) {
            uint64_t sum = 0;
            for (uint64_t duration : pair.second) {
                sum += duration;
            }
            avgTimes[pair.first] = sum / static_cast<double>(pair.second.size()) / 1000.0;  // μs -> ms
        }
        
        return avgTimes;
    }

    // Phase 7: 重要かなの平均入力時間取得（フィルタリング版）
    std::map<std::string, double> Calculator::getImportantKanaIntervals() const {
        // 全かなの平均入力時間を取得
        auto allKanaTimes = getAvgKanaInputTime();
        
        // 重要かなのみをフィルタリング
        std::map<std::string, double> importantTimes;
        for (const auto& kana : IMPORTANT_KANA) {
            auto it = allKanaTimes.find(kana);
            if (it != allKanaTimes.end()) {
                // データが存在する場合のみ追加
                importantTimes[kana] = it->second;
            } else {
                // データが存在しない場合は0.0を設定
                importantTimes[kana] = 0.0;
            }
        }
        
        return importantTimes;
    }

    // Phase 7a-3: カテゴリー別出現頻度計算
    std::vector<FrequencyData> Calculator::calculateCategoryFrequency() const {
        // 全かなの出現回数を集計
        std::map<std::string, size_t> kanaCount;
        size_t totalCount = 0;
        
        for (const auto& kanaInput : kanaInputs_) {
            kanaCount[kanaInput.kana]++;
            totalCount++;
        }
        
        // カテゴリーごとに集計
        std::vector<FrequencyData> result;
        for (const auto& category : KANA_CATEGORIES) {
            size_t categoryCount = 0;
            for (const auto& kana : category.second) {
                auto it = kanaCount.find(kana);
                if (it != kanaCount.end()) {
                    categoryCount += it->second;
                }
            }
            
            double percentage = (totalCount > 0) 
                ? (static_cast<double>(categoryCount) / totalCount * 100.0) 
                : 0.0;
            
            result.emplace_back(category.first, categoryCount, percentage);
        }
        
        return result;
    }

    // Phase 7a-3: 重要かな30文字の出現頻度計算
    std::vector<FrequencyData> Calculator::calculateImportantKanaFrequency() const {
        // 全かなの出現回数を集計
        std::map<std::string, size_t> kanaCount;
        size_t totalCount = 0;
        
        for (const auto& kanaInput : kanaInputs_) {
            kanaCount[kanaInput.kana]++;
            totalCount++;
        }
        
        // 重要30文字の出現回数を取得
        std::vector<FrequencyData> result;
        for (const auto& kana : IMPORTANT_KANA) {
            size_t count = 0;
            auto it = kanaCount.find(kana);
            if (it != kanaCount.end()) {
                count = it->second;
            }
            
            double percentage = (totalCount > 0) 
                ? (static_cast<double>(count) / totalCount * 100.0) 
                : 0.0;
            
            result.emplace_back(kana, count, percentage);
        }
        
        return result;
    }

} // namespace Statistics

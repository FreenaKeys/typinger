#include "csv_logger.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include <ctime>

namespace fs = std::filesystem;

namespace CSVLogger {

    // 現在時刻からファイル名を生成
    std::string generateFilename(const std::string& prefix) {
        // 現在時刻を取得
        auto now = std::chrono::system_clock::now();
        auto now_t = std::chrono::system_clock::to_time_t(now);
        
        // ローカル時刻に変換
        std::tm tm;
        localtime_s(&tm, &now_t);
        
        // YYYYMMDD_HHMMSS形式でフォーマット
        std::ostringstream oss;
        oss << prefix << "_"
            << std::setfill('0')
            << std::setw(4) << (tm.tm_year + 1900)
            << std::setw(2) << (tm.tm_mon + 1)
            << std::setw(2) << tm.tm_mday
            << "_"
            << std::setw(2) << tm.tm_hour
            << std::setw(2) << tm.tm_min
            << std::setw(2) << tm.tm_sec
            << ".csv";
        
        return oss.str();
    }

    // イベントタイプを文字列に変換
    std::string eventTypeToString(InputRecorder::EventType type) {
        switch (type) {
            case InputRecorder::EventType::KEY_DOWN: return "KEY_DOWN";
            case InputRecorder::EventType::KEY_UP: return "KEY_UP";
            case InputRecorder::EventType::BACKSPACE: return "BACKSPACE";
            case InputRecorder::EventType::CORRECTION: return "CORRECTION";
            default: return "UNKNOWN";
        }
    }

    // 文字をCSVセーフな文字列に変換
    std::string charToString(char ch) {
        if (ch == '\0') return "";
        if (ch == '\n') return "\\n";
        if (ch == '\r') return "\\r";
        if (ch == '\t') return "\\t";
        if (ch == ',') return "\\,";
        if (ch == '"') return "\\\"";
        return std::string(1, ch);
    }

    // イベントCSV出力
    std::string writeEventCSV(const InputRecorder::Recorder& recorder,
                              const std::string& outputDir) {
        // 出力ディレクトリを作成
        try {
            fs::create_directories(outputDir);
        } catch (const std::exception& e) {
            return "";  // ディレクトリ作成失敗
        }
        
        // ファイル名を生成
        std::string filename = generateFilename("typing_events");
        std::string filepath = outputDir + "/" + filename;
        
        // CSVファイルを開く
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return "";  // ファイルオープン失敗
        }
        
        // ヘッダー行を書き込み
        file << "timestamp_us,event_type,vk_code,scan_code,character,is_correct,inter_key_time_us,note\n";
        
        // イベントデータを書き込み
        const auto& events = recorder.getEvents();
        for (const auto& event : events) {
            file << event.timestamp_us << ","
                 << eventTypeToString(event.type) << ","
                 << event.vk_code << ","
                 << event.scan_code << ","
                 << charToString(event.character) << ","
                 << (event.is_correct ? "1" : "0") << ","
                 << event.inter_key_time_us << ","
                 << event.note << "\n";
        }
        
        file.close();
        return filepath;
    }

    // サマリCSV出力（Phase 4-2で実装）
    std::string writeSummaryCSV(const Statistics::StatisticsData& stats,
                                const std::string& outputDir) {
        // Phase 4-2で実装
        return "";
    }

} // namespace CSVLogger

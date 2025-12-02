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

    // サマリCSV出力
    std::string writeSummaryCSV(const Statistics::StatisticsData& stats,
                                const std::string& outputDir) {
        // 出力ディレクトリを作成
        try {
            fs::create_directories(outputDir);
        } catch (const std::exception& e) {
            return "";  // ディレクトリ作成失敗
        }
        
        // ファイル名を生成
        std::string filename = generateFilename("typing_summary");
        std::string filepath = outputDir + "/" + filename;
        
        // CSVファイルを開く
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return "";  // ファイルオープン失敗
        }
        
        // ヘッダー行を書き込み
        file << "metric,value,unit\n";
        
        // 基本情報
        file << "total_duration," << stats.totalDuration << ",microseconds\n";
        file << "total_duration_sec," << (stats.totalDuration / 1000000.0) << ",seconds\n";
        file << "total_key_count," << stats.totalKeyCount << ",keys\n";
        file << "correct_key_count," << stats.correctKeyCount << ",keys\n";
        file << "incorrect_key_count," << stats.incorrectKeyCount << ",keys\n";
        file << "backspace_count," << stats.backspaceCount << ",keys\n";
        
        // 正答率
        double accuracy = (stats.totalKeyCount > 0) 
            ? (static_cast<double>(stats.correctKeyCount) / stats.totalKeyCount * 100.0)
            : 0.0;
        file << "accuracy," << std::fixed << std::setprecision(2) << accuracy << ",percent\n";
        
        // WPM/CPM
        file << "wpm_total," << std::fixed << std::setprecision(2) << stats.wpmTotal << ",words_per_minute\n";
        file << "wpm_correct," << std::fixed << std::setprecision(2) << stats.wpmCorrect << ",words_per_minute\n";
        file << "cpm_total," << std::fixed << std::setprecision(2) << stats.cpmTotal << ",chars_per_minute\n";
        file << "cpm_correct," << std::fixed << std::setprecision(2) << stats.cpmCorrect << ",chars_per_minute\n";
        
        // キー間隔
        file << "avg_inter_key_interval," << std::fixed << std::setprecision(2) << stats.avgInterKeyInterval << ",milliseconds\n";
        file << "min_inter_key_interval," << std::fixed << std::setprecision(2) << stats.minInterKeyInterval << ",milliseconds\n";
        file << "max_inter_key_interval," << std::fixed << std::setprecision(2) << stats.maxInterKeyInterval << ",milliseconds\n";
        
        file.close();
        
        // かな別入力時間を別ファイルに出力
        if (!stats.kanaInputTime.empty()) {
            std::string kanaFilename = generateFilename("typing_kana");
            std::string kanaFilepath = outputDir + "/" + kanaFilename;
            
            std::ofstream kanaFile(kanaFilepath);
            if (kanaFile.is_open()) {
                kanaFile << "kana,avg_input_time_ms\n";
                for (const auto& pair : stats.kanaInputTime) {
                    kanaFile << pair.first << "," 
                            << std::fixed << std::setprecision(2) << pair.second << "\n";
                }
                kanaFile.close();
            }
        }
        
        return filepath;
    }

    // サマリCSV出力（実験データ付き - Phase 6で追加）
    std::string writeSummaryCSV(const Statistics::StatisticsData& stats,
                                const ExperimentData& expData,
                                const std::string& outputDir) {
        // 出力ディレクトリを作成
        try {
            fs::create_directories(outputDir);
        } catch (const std::exception& e) {
            return "";  // ディレクトリ作成失敗
        }
        
        // ファイル名を生成
        std::string filename = generateFilename("typing_summary");
        std::string filepath = outputDir + "/" + filename;
        
        // CSVファイルを開く
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return "";  // ファイルオープン失敗
        }
        
        // ヘッダー行を書き込み（実験データフィールド追加）
        file << "metric,value,unit\n";
        
        // 実験データ（Phase 6で追加）
        file << "subject_id," << expData.subject_id << ",text\n";
        file << "session_number," << expData.session_number << ",number\n";
        file << "layout_type," << expData.layout_type << ",text\n";
        file << "test_number," << expData.test_number << ",number\n";
        
        // 基本情報
        file << "total_duration," << stats.totalDuration << ",microseconds\n";
        file << "total_duration_sec," << (stats.totalDuration / 1000000.0) << ",seconds\n";
        file << "total_key_count," << stats.totalKeyCount << ",keys\n";
        file << "correct_key_count," << stats.correctKeyCount << ",keys\n";
        file << "incorrect_key_count," << stats.incorrectKeyCount << ",keys\n";
        file << "backspace_count," << stats.backspaceCount << ",keys\n";
        
        // 正答率
        double accuracy = (stats.totalKeyCount > 0) 
            ? (static_cast<double>(stats.correctKeyCount) / stats.totalKeyCount * 100.0)
            : 0.0;
        file << "accuracy," << std::fixed << std::setprecision(2) << accuracy << ",percent\n";
        
        // WPM/CPM
        file << "wpm_total," << std::fixed << std::setprecision(2) << stats.wpmTotal << ",words_per_minute\n";
        file << "wpm_correct," << std::fixed << std::setprecision(2) << stats.wpmCorrect << ",words_per_minute\n";
        file << "cpm_total," << std::fixed << std::setprecision(2) << stats.cpmTotal << ",chars_per_minute\n";
        file << "cpm_correct," << std::fixed << std::setprecision(2) << stats.cpmCorrect << ",chars_per_minute\n";
        
        // キー間隔
        file << "avg_inter_key_interval," << std::fixed << std::setprecision(2) << stats.avgInterKeyInterval << ",milliseconds\n";
        file << "min_inter_key_interval," << std::fixed << std::setprecision(2) << stats.minInterKeyInterval << ",milliseconds\n";
        file << "max_inter_key_interval," << std::fixed << std::setprecision(2) << stats.maxInterKeyInterval << ",milliseconds\n";
        
        file.close();
        
        // かな別入力時間を別ファイルに出力
        if (!stats.kanaInputTime.empty()) {
            std::string kanaFilename = generateFilename("typing_kana");
            std::string kanaFilepath = outputDir + "/" + kanaFilename;
            
            std::ofstream kanaFile(kanaFilepath);
            if (kanaFile.is_open()) {
                kanaFile << "kana,avg_input_time_ms\n";
                for (const auto& pair : stats.kanaInputTime) {
                    kanaFile << pair.first << "," 
                            << std::fixed << std::setprecision(2) << pair.second << "\n";
                }
                kanaFile.close();
            }
        }
        
        return filepath;
    }

    // 被験者IDから統合CSVファイル名を生成
    std::string getConsolidatedFilename(const std::string& subjectId) {
        return "typing_summary_" + subjectId + ".csv";
    }

    // 重複セッションチェック
    bool isDuplicateSession(const std::string& filepath,
                           const ExperimentData& expData) {
        // ファイルが存在しない場合は重複なし
        if (!fs::exists(filepath)) {
            return false;
        }
        
        // CSVファイルを開いて読み込み
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;  // ファイルオープン失敗は重複なしとして扱う
        }
        
        std::string line;
        bool inDataSection = false;
        int currentSession = -1;
        std::string currentLayout = "";
        int currentTest = -1;
        
        while (std::getline(file, line)) {
            // ヘッダー行をスキップ
            if (line == "metric,value,unit") {
                inDataSection = true;
                continue;
            }
            
            if (!inDataSection) {
                continue;
            }
            
            // セッション番号を取得
            if (line.find("session_number,") == 0) {
                size_t pos = line.find(',');
                if (pos != std::string::npos) {
                    size_t pos2 = line.find(',', pos + 1);
                    std::string valueStr = line.substr(pos + 1, pos2 - pos - 1);
                    currentSession = std::stoi(valueStr);
                }
            }
            
            // 配列タイプを取得
            if (line.find("layout_type,") == 0) {
                size_t pos = line.find(',');
                if (pos != std::string::npos) {
                    size_t pos2 = line.find(',', pos + 1);
                    currentLayout = line.substr(pos + 1, pos2 - pos - 1);
                }
            }
            
            // テスト番号を取得
            if (line.find("test_number,") == 0) {
                size_t pos = line.find(',');
                if (pos != std::string::npos) {
                    size_t pos2 = line.find(',', pos + 1);
                    std::string valueStr = line.substr(pos + 1, pos2 - pos - 1);
                    currentTest = std::stoi(valueStr);
                    
                    // 3つのフィールドが揃ったらチェック
                    if (currentSession == expData.session_number &&
                        currentLayout == expData.layout_type &&
                        currentTest == expData.test_number) {
                        file.close();
                        return true;  // 重複発見
                    }
                }
            }
            
            // 次のセッションデータに移行（空行で区切られる）
            if (line.empty()) {
                currentSession = -1;
                currentLayout = "";
                currentTest = -1;
            }
        }
        
        file.close();
        return false;  // 重複なし
    }

    // サマリCSV追記保存（実験データ付き - Phase 6で追加）
    std::string appendSummaryCSV(const Statistics::StatisticsData& stats,
                                 const ExperimentData& expData,
                                 const std::string& outputDir) {
        // 出力ディレクトリを作成
        try {
            fs::create_directories(outputDir);
        } catch (const std::exception& e) {
            return "";  // ディレクトリ作成失敗
        }
        
        // 統合ファイル名を生成
        std::string filename = getConsolidatedFilename(expData.subject_id);
        std::string filepath = outputDir + "/" + filename;
        
        // 重複セッションチェック
        if (isDuplicateSession(filepath, expData)) {
            return "";  // 重複エラー（空文字列を返す）
        }
        
        // ファイルが存在するか確認
        bool fileExists = fs::exists(filepath);
        
        // CSVファイルを開く（追記モード）
        std::ofstream file(filepath, std::ios::app);
        if (!file.is_open()) {
            return "";  // ファイルオープン失敗
        }
        
        // ファイルが新規の場合、ヘッダー行を書き込み
        if (!fileExists) {
            file << "metric,value,unit\n";
        }
        
        // 実験データ（Phase 6で追加）
        file << "subject_id," << expData.subject_id << ",text\n";
        file << "session_number," << expData.session_number << ",number\n";
        file << "layout_type," << expData.layout_type << ",text\n";
        file << "test_number," << expData.test_number << ",number\n";
        
        // 基本情報
        file << "total_duration," << stats.totalDuration << ",microseconds\n";
        file << "total_duration_sec," << (stats.totalDuration / 1000000.0) << ",seconds\n";
        file << "total_key_count," << stats.totalKeyCount << ",keys\n";
        file << "correct_key_count," << stats.correctKeyCount << ",keys\n";
        file << "incorrect_key_count," << stats.incorrectKeyCount << ",keys\n";
        file << "backspace_count," << stats.backspaceCount << ",keys\n";
        
        // 正答率
        double accuracy = (stats.totalKeyCount > 0) 
            ? (static_cast<double>(stats.correctKeyCount) / stats.totalKeyCount * 100.0)
            : 0.0;
        file << "accuracy," << std::fixed << std::setprecision(2) << accuracy << ",percent\n";
        
        // WPM/CPM
        file << "wpm_total," << std::fixed << std::setprecision(2) << stats.wpmTotal << ",words_per_minute\n";
        file << "wpm_correct," << std::fixed << std::setprecision(2) << stats.wpmCorrect << ",words_per_minute\n";
        file << "cpm_total," << std::fixed << std::setprecision(2) << stats.cpmTotal << ",chars_per_minute\n";
        file << "cpm_correct," << std::fixed << std::setprecision(2) << stats.cpmCorrect << ",chars_per_minute\n";
        
        // キー間隔
        file << "avg_inter_key_interval," << std::fixed << std::setprecision(2) << stats.avgInterKeyInterval << ",milliseconds\n";
        file << "min_inter_key_interval," << std::fixed << std::setprecision(2) << stats.minInterKeyInterval << ",milliseconds\n";
        file << "max_inter_key_interval," << std::fixed << std::setprecision(2) << stats.maxInterKeyInterval << ",milliseconds\n";
        
        // セッション間の区切り（空行）
        file << "\n";
        
        file.close();
        
        return filepath;
    }

} // namespace CSVLogger

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cassert>
#include <sstream>
#include "../core/csv_logger.h"
#include "../core/input_recorder.h"
#include "../core/statistics.h"

namespace fs = std::filesystem;

// テスト用のディレクトリとファイルをクリーンアップ
void cleanupTestFiles() {
    if (fs::exists("test_output")) {
        fs::remove_all("test_output");
    }
}

// Test 1: ファイル名生成のテスト
void test_filename_generation() {
    std::cout << "Test: Filename generation..." << std::endl;
    
    std::string filename = CSVLogger::generateFilename("typing_events");
    
    // ファイル名が正しい形式かチェック
    assert(filename.find("typing_events_") == 0);
    assert(filename.find(".csv") != std::string::npos);
    assert(filename.length() > 20);  // "typing_events_YYYYMMDD_HHMMSS.csv"は30文字程度
    
    std::cout << "  Generated filename: " << filename << std::endl;
    std::cout << "  PASS" << std::endl;
}

// Test 2: 空のイベントリストでCSV出力
void test_empty_event_csv() {
    std::cout << "Test: Empty event CSV..." << std::endl;
    
    cleanupTestFiles();
    
    InputRecorder::Recorder recorder;
    std::string filepath = CSVLogger::writeEventCSV(recorder, "test_output");
    
    // ファイルが生成されたことを確認
    assert(!filepath.empty());
    assert(fs::exists(filepath));
    
    // ファイル内容を確認（ヘッダーのみ）
    std::ifstream file(filepath);
    std::string line;
    std::getline(file, line);
    assert(line == "timestamp_us,event_type,vk_code,scan_code,character,is_correct,inter_key_time_us,note");
    
    // データ行がないことを確認
    assert(!std::getline(file, line));
    file.close();
    
    std::cout << "  Output file: " << filepath << std::endl;
    std::cout << "  PASS" << std::endl;
}

// Test 3: イベントを含むCSV出力
void test_event_csv_with_data() {
    std::cout << "Test: Event CSV with data..." << std::endl;
    
    cleanupTestFiles();
    
    InputRecorder::Recorder recorder;
    recorder.startSession();
    
    // テストイベントを記録
    recorder.recordKeyDown(65, 0, 'a');  // 'A' key
    recorder.recordKeyUp(65, 0);
    recorder.recordKeyDown(66, 0, 'b');
    recorder.recordKeyUp(66, 0);
    recorder.recordBackspace();
    
    recorder.endSession();
    
    std::string filepath = CSVLogger::writeEventCSV(recorder, "test_output");
    
    // ファイルが生成されたことを確認
    assert(!filepath.empty());
    assert(fs::exists(filepath));
    
    // ファイル内容を確認
    std::ifstream file(filepath);
    std::string line;
    
    // ヘッダー行
    std::getline(file, line);
    assert(line == "timestamp_us,event_type,vk_code,scan_code,character,is_correct,inter_key_time_us,note");
    
    // データ行を確認（5イベント）
    int lineCount = 0;
    while (std::getline(file, line)) {
        lineCount++;
        assert(!line.empty());
    }
    assert(lineCount == 5);  // KEY_DOWN, KEY_UP, KEY_DOWN, KEY_UP, BACKSPACE
    
    file.close();
    
    std::cout << "  Output file: " << filepath << std::endl;
    std::cout << "  Event count: " << lineCount << std::endl;
    std::cout << "  PASS" << std::endl;
}

// Test 4: CSVフォーマット検証
void test_csv_format() {
    std::cout << "Test: CSV format validation..." << std::endl;
    
    cleanupTestFiles();
    
    InputRecorder::Recorder recorder;
    recorder.startSession();
    recorder.recordKeyDown(65, 30, 'a');
    recorder.endSession();
    
    std::string filepath = CSVLogger::writeEventCSV(recorder, "test_output");
    
    // ファイル内容を読み込み
    std::ifstream file(filepath);
    std::string header, dataLine;
    std::getline(file, header);
    std::getline(file, dataLine);
    file.close();
    
    // カンマの数をチェック（8カラムなので7個のカンマ）
    int commaCount = 0;
    for (char ch : dataLine) {
        if (ch == ',') commaCount++;
    }
    assert(commaCount == 7);
    
    // データ行が数値とテキストを含むことを確認
    assert(dataLine.find("KEY_DOWN") != std::string::npos);
    assert(dataLine.find("65") != std::string::npos);  // vk_code
    assert(dataLine.find("a") != std::string::npos);   // character
    
    std::cout << "  CSV format valid" << std::endl;
    std::cout << "  PASS" << std::endl;
}

// Test 5: ディレクトリ自動作成
void test_directory_creation() {
    std::cout << "Test: Directory auto-creation..." << std::endl;
    
    cleanupTestFiles();
    
    // 存在しないディレクトリパスを指定
    InputRecorder::Recorder recorder;
    std::string filepath = CSVLogger::writeEventCSV(recorder, "test_output/subdir");
    
    // ディレクトリが作成されたことを確認
    assert(fs::exists("test_output/subdir"));
    assert(!filepath.empty());
    assert(fs::exists(filepath));
    
    std::cout << "  Directory created: test_output/subdir" << std::endl;
    std::cout << "  PASS" << std::endl;
}

// Test 6: 空の統計データでサマリCSV出力
void test_empty_summary_csv() {
    std::cout << "Test: Empty summary CSV..." << std::endl;
    
    cleanupTestFiles();
    
    Statistics::StatisticsData stats;
    std::string filepath = CSVLogger::writeSummaryCSV(stats, "test_output");
    
    // ファイルが生成されたことを確認
    assert(!filepath.empty());
    assert(fs::exists(filepath));
    
    // ファイル内容を確認
    std::ifstream file(filepath);
    std::string line;
    std::getline(file, line);
    assert(line == "metric,value,unit");
    
    // データ行が存在することを確認
    int lineCount = 0;
    while (std::getline(file, line)) {
        lineCount++;
    }
    assert(lineCount > 0);  // 少なくとも基本メトリクスが出力される
    file.close();
    
    std::cout << "  Output file: " << filepath << std::endl;
    std::cout << "  Metrics count: " << lineCount << std::endl;
    std::cout << "  PASS" << std::endl;
}

// Test 7: 統計データを含むサマリCSV出力
void test_summary_csv_with_data() {
    std::cout << "Test: Summary CSV with data..." << std::endl;
    
    cleanupTestFiles();
    
    // テスト用の統計データを作成
    Statistics::StatisticsData stats;
    stats.totalDuration = 60000000;  // 60秒（マイクロ秒）
    stats.totalKeyCount = 300;
    stats.correctKeyCount = 285;
    stats.incorrectKeyCount = 15;
    stats.backspaceCount = 15;
    stats.wpmTotal = 60.0;
    stats.wpmCorrect = 57.0;
    stats.cpmTotal = 300.0;
    stats.cpmCorrect = 285.0;
    stats.avgInterKeyInterval = 200.0;
    stats.minInterKeyInterval = 50.0;
    stats.maxInterKeyInterval = 500.0;
    
    std::string filepath = CSVLogger::writeSummaryCSV(stats, "test_output");
    
    // ファイルが生成されたことを確認
    assert(!filepath.empty());
    assert(fs::exists(filepath));
    
    // ファイル内容を確認
    std::ifstream file(filepath);
    std::string line;
    
    // ヘッダー行
    std::getline(file, line);
    assert(line == "metric,value,unit");
    
    // 特定のメトリクスを確認
    bool foundWpm = false;
    bool foundCpm = false;
    bool foundAccuracy = false;
    
    while (std::getline(file, line)) {
        if (line.find("wpm_total") != std::string::npos) foundWpm = true;
        if (line.find("cpm_total") != std::string::npos) foundCpm = true;
        if (line.find("accuracy") != std::string::npos) foundAccuracy = true;
    }
    
    assert(foundWpm);
    assert(foundCpm);
    assert(foundAccuracy);
    
    file.close();
    
    std::cout << "  Output file: " << filepath << std::endl;
    std::cout << "  WPM, CPM, Accuracy found" << std::endl;
    std::cout << "  PASS" << std::endl;
}

// Test 8: かな別入力時間を含むサマリCSV出力
void test_summary_csv_with_kana_data() {
    std::cout << "Test: Summary CSV with kana data..." << std::endl;
    
    cleanupTestFiles();
    
    // かな別入力時間を含む統計データ
    Statistics::StatisticsData stats;
    stats.totalDuration = 60000000;
    stats.totalKeyCount = 100;
    stats.correctKeyCount = 95;
    stats.kanaInputTime["あ"] = 150.5;
    stats.kanaInputTime["し"] = 200.3;
    stats.kanaInputTime["しゅ"] = 350.7;
    
    std::string filepath = CSVLogger::writeSummaryCSV(stats, "test_output");
    
    // サマリファイルが生成されたことを確認
    assert(!filepath.empty());
    assert(fs::exists(filepath));
    
    // かな別ファイルが生成されたことを確認
    bool kanaFileFound = false;
    for (const auto& entry : fs::directory_iterator("test_output")) {
        if (entry.path().filename().string().find("typing_kana_") == 0) {
            kanaFileFound = true;
            
            // かなファイルの内容を確認
            std::ifstream kanaFile(entry.path());
            std::string line;
            std::getline(kanaFile, line);
            assert(line == "kana,avg_input_time_ms");
            
            int kanaCount = 0;
            while (std::getline(kanaFile, line)) {
                kanaCount++;
            }
            assert(kanaCount == 3);  // あ, し, しゅ
            kanaFile.close();
            break;
        }
    }
    assert(kanaFileFound);
    
    std::cout << "  Summary file: " << filepath << std::endl;
    std::cout << "  Kana file generated" << std::endl;
    std::cout << "  PASS" << std::endl;
}

// Test 9: サマリCSVフォーマット検証
void test_summary_csv_format() {
    std::cout << "Test: Summary CSV format validation..." << std::endl;
    
    cleanupTestFiles();
    
    Statistics::StatisticsData stats;
    stats.totalDuration = 30000000;
    stats.totalKeyCount = 150;
    stats.correctKeyCount = 145;
    stats.wpmTotal = 50.0;
    
    std::string filepath = CSVLogger::writeSummaryCSV(stats, "test_output");
    
    // ファイル内容を読み込み
    std::ifstream file(filepath);
    std::string line;
    
    // ヘッダー行
    std::getline(file, line);
    
    // 各データ行がmetric,value,unit形式であることを確認
    while (std::getline(file, line)) {
        // カンマの数をチェック（3カラムなので2個のカンマ）
        int commaCount = 0;
        for (char ch : line) {
            if (ch == ',') commaCount++;
        }
        assert(commaCount == 2);
    }
    
    file.close();
    
    std::cout << "  CSV format valid (metric,value,unit)" << std::endl;
    std::cout << "  PASS" << std::endl;
}

int main() {
    std::cout << "=== CSV Logger Unit Tests ===" << std::endl;
    std::cout << std::endl;
    
    try {
        // イベントCSVテスト
        test_filename_generation();
        test_empty_event_csv();
        test_event_csv_with_data();
        test_csv_format();
        test_directory_creation();
        
        // サマリCSVテスト (Phase 4-2)
        test_empty_summary_csv();
        test_summary_csv_with_data();
        test_summary_csv_with_kana_data();
        test_summary_csv_format();
        
        // テスト後のクリーンアップ
        cleanupTestFiles();
        
        std::cout << std::endl;
        std::cout << "All tests passed! ✓" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        cleanupTestFiles();
        return 1;
    }
    
    return 0;
}

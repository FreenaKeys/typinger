#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cassert>
#include "../core/csv_logger.h"
#include "../core/input_recorder.h"

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

int main() {
    std::cout << "=== CSV Logger Unit Tests ===" << std::endl;
    std::cout << std::endl;
    
    try {
        test_filename_generation();
        test_empty_event_csv();
        test_event_csv_with_data();
        test_csv_format();
        test_directory_creation();
        
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

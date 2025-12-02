#include "../core/csv_logger.h"
#include "../core/statistics.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// テスト用の統計データを作成
Statistics::StatisticsData createTestStats() {
    Statistics::StatisticsData stats;
    stats.totalDuration = 10000000;  // 10秒
    stats.totalKeyCount = 50;
    stats.correctKeyCount = 45;
    stats.incorrectKeyCount = 5;
    stats.backspaceCount = 3;
    stats.wpmTotal = 60.0;
    stats.wpmCorrect = 54.0;
    stats.cpmTotal = 300.0;
    stats.cpmCorrect = 270.0;
    stats.avgInterKeyInterval = 200.0;
    stats.minInterKeyInterval = 100.0;
    stats.maxInterKeyInterval = 500.0;
    
    // かな別入力時間
    stats.kanaInputTime["こ"] = 180.5;
    stats.kanaInputTime["れ"] = 150.3;
    stats.kanaInputTime["は"] = 200.7;
    
    return stats;
}

void test_ExperimentDataConstructor() {
    std::cout << "Testing ExperimentData constructor..." << std::endl;
    
    // デフォルトコンストラクタ
    CSVLogger::ExperimentData data1;
    assert(data1.subject_id == "");
    assert(data1.session_number == 0);
    assert(data1.layout_type == "");
    assert(data1.test_number == 0);
    
    // パラメータ付きコンストラクタ
    CSVLogger::ExperimentData data2("SK", 3, "new", 2);
    assert(data2.subject_id == "SK");
    assert(data2.session_number == 3);
    assert(data2.layout_type == "new");
    assert(data2.test_number == 2);
    
    std::cout << "✓ All ExperimentData constructor tests passed" << std::endl;
}

void test_WriteSummaryCSVWithExperimentData() {
    std::cout << "Testing writeSummaryCSV with experiment data..." << std::endl;
    
    // テスト用ディレクトリを作成
    std::string testDir = "test_output_exp";
    fs::create_directories(testDir);
    
    // テストデータ作成
    Statistics::StatisticsData stats = createTestStats();
    CSVLogger::ExperimentData expData("MT", 5, "old", 4);
    
    // CSV出力
    std::string filepath = CSVLogger::writeSummaryCSV(stats, expData, testDir);
    assert(!filepath.empty());
    
    // ファイルが存在することを確認
    assert(fs::exists(filepath));
    
    // ファイル内容を読み込んで検証
    std::ifstream file(filepath);
    assert(file.is_open());
    
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    
    // ヘッダー行を確認
    assert(lines[0] == "metric,value,unit");
    
    // 実験データフィールドを確認
    assert(lines[1] == "subject_id,MT,text");
    assert(lines[2] == "session_number,5,number");
    assert(lines[3] == "layout_type,old,text");
    assert(lines[4] == "test_number,4,number");
    
    // 基本データが続くことを確認
    assert(lines[5].find("total_duration,") == 0);
    
    // テストディレクトリを削除
    fs::remove_all(testDir);
    
    std::cout << "✓ All writeSummaryCSV with experiment data tests passed" << std::endl;
}

void test_WriteSummaryCSVBackwardCompatibility() {
    std::cout << "Testing backward compatibility (without experiment data)..." << std::endl;
    
    // テスト用ディレクトリを作成
    std::string testDir = "test_output_compat";
    fs::create_directories(testDir);
    
    // テストデータ作成（実験データなし）
    Statistics::StatisticsData stats = createTestStats();
    
    // CSV出力（従来の方法）
    std::string filepath = CSVLogger::writeSummaryCSV(stats, testDir);
    assert(!filepath.empty());
    
    // ファイルが存在することを確認
    assert(fs::exists(filepath));
    
    // ファイル内容を読み込んで検証
    std::ifstream file(filepath);
    assert(file.is_open());
    
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    
    // ヘッダー行を確認
    assert(lines[0] == "metric,value,unit");
    
    // 実験データフィールドがないことを確認
    assert(lines[1].find("total_duration,") == 0);  // 最初のデータ行
    
    // テストディレクトリを削除
    fs::remove_all(testDir);
    
    std::cout << "✓ All backward compatibility tests passed" << std::endl;
}

void test_ExperimentDataEdgeCases() {
    std::cout << "Testing experiment data edge cases..." << std::endl;
    
    // テスト用ディレクトリを作成
    std::string testDir = "test_output_edge";
    fs::create_directories(testDir);
    
    // テストデータ作成
    Statistics::StatisticsData stats = createTestStats();
    
    // エッジケース1: セッション1、テスト1
    CSVLogger::ExperimentData data1("SK", 1, "new", 1);
    std::string filepath1 = CSVLogger::writeSummaryCSV(stats, data1, testDir);
    assert(!filepath1.empty());
    assert(fs::exists(filepath1));
    
    // エッジケース2: セッション14、テスト6
    CSVLogger::ExperimentData data2("KM", 14, "old", 6);
    std::string filepath2 = CSVLogger::writeSummaryCSV(stats, data2, testDir);
    assert(!filepath2.empty());
    assert(fs::exists(filepath2));
    
    // テストディレクトリを削除
    fs::remove_all(testDir);
    
    std::cout << "✓ All edge case tests passed" << std::endl;
}

int main() {
    std::cout << "=== CSV Logger Experiment Data Tests ===" << std::endl;
    
    test_ExperimentDataConstructor();
    test_WriteSummaryCSVWithExperimentData();
    test_WriteSummaryCSVBackwardCompatibility();
    test_ExperimentDataEdgeCases();
    
    std::cout << "\n✓✓✓ All tests passed! ✓✓✓" << std::endl;
    return 0;
}

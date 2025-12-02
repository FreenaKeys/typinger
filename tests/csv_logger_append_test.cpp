#include "../core/csv_logger.h"
#include "../core/statistics.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

// テスト用の統計データを作成
Statistics::StatisticsData createTestStats(int seed) {
    Statistics::StatisticsData stats;
    stats.totalDuration = 10000000 + seed * 1000;
    stats.totalKeyCount = 50 + seed;
    stats.correctKeyCount = 45 + seed;
    stats.incorrectKeyCount = 5;
    stats.backspaceCount = 3 + seed;
    stats.wpmTotal = 60.0 + seed;
    stats.wpmCorrect = 54.0 + seed;
    stats.cpmTotal = 300.0 + seed;
    stats.cpmCorrect = 270.0 + seed;
    stats.avgInterKeyInterval = 200.0 + seed;
    stats.minInterKeyInterval = 100.0;
    stats.maxInterKeyInterval = 500.0 + seed;
    
    return stats;
}

void test_GetConsolidatedFilename() {
    std::cout << "Testing getConsolidatedFilename..." << std::endl;
    
    assert(CSVLogger::getConsolidatedFilename("SK") == "typing_summary_SK.csv");
    assert(CSVLogger::getConsolidatedFilename("MT") == "typing_summary_MT.csv");
    assert(CSVLogger::getConsolidatedFilename("IK") == "typing_summary_IK.csv");
    assert(CSVLogger::getConsolidatedFilename("KS") == "typing_summary_KS.csv");
    assert(CSVLogger::getConsolidatedFilename("KM") == "typing_summary_KM.csv");
    
    std::cout << "✓ All getConsolidatedFilename tests passed" << std::endl;
}

void test_AppendSummaryCSV_NewFile() {
    std::cout << "Testing appendSummaryCSV (new file)..." << std::endl;
    
    // テスト用ディレクトリを作成
    std::string testDir = "test_output_append_new";
    fs::create_directories(testDir);
    
    // テストデータ作成
    Statistics::StatisticsData stats = createTestStats(0);
    CSVLogger::ExperimentData expData("SK", 1, "new", 1);
    
    // CSV追記保存（新規ファイル）
    std::string filepath = CSVLogger::appendSummaryCSV(stats, expData, testDir);
    assert(!filepath.empty());
    assert(filepath == testDir + "/typing_summary_SK.csv");
    
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
    assert(lines[1] == "subject_id,SK,text");
    assert(lines[2] == "session_number,1,number");
    assert(lines[3] == "layout_type,new,text");
    assert(lines[4] == "test_number,1,number");
    
    // 最後に空行があることを確認
    assert(lines[lines.size() - 1] == "");
    
    // テストディレクトリを削除
    fs::remove_all(testDir);
    
    std::cout << "✓ All appendSummaryCSV (new file) tests passed" << std::endl;
}

void test_AppendSummaryCSV_ExistingFile() {
    std::cout << "Testing appendSummaryCSV (existing file)..." << std::endl;
    
    // テスト用ディレクトリを作成
    std::string testDir = "test_output_append_exist";
    fs::create_directories(testDir);
    
    // 1回目: セッション1、テスト1
    Statistics::StatisticsData stats1 = createTestStats(0);
    CSVLogger::ExperimentData expData1("MT", 1, "new", 1);
    std::string filepath1 = CSVLogger::appendSummaryCSV(stats1, expData1, testDir);
    assert(!filepath1.empty());
    
    // 2回目: セッション1、テスト2（同じファイルに追記）
    Statistics::StatisticsData stats2 = createTestStats(1);
    CSVLogger::ExperimentData expData2("MT", 1, "old", 2);
    std::string filepath2 = CSVLogger::appendSummaryCSV(stats2, expData2, testDir);
    assert(!filepath2.empty());
    assert(filepath1 == filepath2);  // 同じファイル
    
    // ファイル内容を読み込んで検証
    std::ifstream file(filepath1);
    assert(file.is_open());
    
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    
    // ヘッダーは1回だけ
    int headerCount = 0;
    for (const auto& l : lines) {
        if (l == "metric,value,unit") {
            headerCount++;
        }
    }
    assert(headerCount == 1);
    
    // 2つのセッションデータが含まれることを確認
    int sessionCount = 0;
    for (const auto& l : lines) {
        if (l.find("session_number,") == 0) {
            sessionCount++;
        }
    }
    assert(sessionCount == 2);
    
    // テストディレクトリを削除
    fs::remove_all(testDir);
    
    std::cout << "✓ All appendSummaryCSV (existing file) tests passed" << std::endl;
}

void test_IsDuplicateSession() {
    std::cout << "Testing isDuplicateSession..." << std::endl;
    
    // テスト用ディレクトリを作成
    std::string testDir = "test_output_duplicate";
    fs::create_directories(testDir);
    
    // セッション1、テスト1を追加
    Statistics::StatisticsData stats1 = createTestStats(0);
    CSVLogger::ExperimentData expData1("IK", 2, "new", 1);
    std::string filepath = CSVLogger::appendSummaryCSV(stats1, expData1, testDir);
    assert(!filepath.empty());
    
    // 同じセッション・テストを再度追加しようとする（重複）
    CSVLogger::ExperimentData expData2("IK", 2, "new", 1);
    assert(CSVLogger::isDuplicateSession(filepath, expData2) == true);
    
    // 追記保存も失敗することを確認
    std::string filepath2 = CSVLogger::appendSummaryCSV(stats1, expData2, testDir);
    assert(filepath2.empty());  // 重複エラーで空文字列
    
    // 異なるテスト番号は重複しない
    CSVLogger::ExperimentData expData3("IK", 2, "new", 2);
    assert(CSVLogger::isDuplicateSession(filepath, expData3) == false);
    
    // 異なる配列タイプは重複しない
    CSVLogger::ExperimentData expData4("IK", 2, "old", 1);
    assert(CSVLogger::isDuplicateSession(filepath, expData4) == false);
    
    // 異なるセッション番号は重複しない
    CSVLogger::ExperimentData expData5("IK", 3, "new", 1);
    assert(CSVLogger::isDuplicateSession(filepath, expData5) == false);
    
    // テストディレクトリを削除
    fs::remove_all(testDir);
    
    std::cout << "✓ All isDuplicateSession tests passed" << std::endl;
}

void test_MultipleSubjects() {
    std::cout << "Testing multiple subjects..." << std::endl;
    
    // テスト用ディレクトリを作成
    std::string testDir = "test_output_multi";
    fs::create_directories(testDir);
    
    // 被験者SKのデータ
    Statistics::StatisticsData stats1 = createTestStats(0);
    CSVLogger::ExperimentData expDataSK("SK", 1, "new", 1);
    std::string filepathSK = CSVLogger::appendSummaryCSV(stats1, expDataSK, testDir);
    assert(!filepathSK.empty());
    assert(filepathSK == testDir + "/typing_summary_SK.csv");
    
    // 被験者MTのデータ
    Statistics::StatisticsData stats2 = createTestStats(1);
    CSVLogger::ExperimentData expDataMT("MT", 1, "new", 1);
    std::string filepathMT = CSVLogger::appendSummaryCSV(stats2, expDataMT, testDir);
    assert(!filepathMT.empty());
    assert(filepathMT == testDir + "/typing_summary_MT.csv");
    
    // 2つの異なるファイルが作成される
    assert(filepathSK != filepathMT);
    assert(fs::exists(filepathSK));
    assert(fs::exists(filepathMT));
    
    // テストディレクトリを削除
    fs::remove_all(testDir);
    
    std::cout << "✓ All multiple subjects tests passed" << std::endl;
}

int main() {
    std::cout << "=== CSV Logger Append Tests ===" << std::endl;
    
    test_GetConsolidatedFilename();
    test_AppendSummaryCSV_NewFile();
    test_AppendSummaryCSV_ExistingFile();
    test_IsDuplicateSession();
    test_MultipleSubjects();
    
    std::cout << "\n✓✓✓ All tests passed! ✓✓✓" << std::endl;
    return 0;
}

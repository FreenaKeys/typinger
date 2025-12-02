#include "../core/scenario_selector.h"
#include <cassert>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

void test_IsValidSessionNumber() {
    std::cout << "Testing isValidSessionNumber..." << std::endl;
    
    // 有効な範囲
    assert(ScenarioSelector::isValidSessionNumber(1) == true);
    assert(ScenarioSelector::isValidSessionNumber(3) == true);
    assert(ScenarioSelector::isValidSessionNumber(6) == true);
    
    // 無効な範囲
    assert(ScenarioSelector::isValidSessionNumber(0) == false);
    assert(ScenarioSelector::isValidSessionNumber(7) == false);
    assert(ScenarioSelector::isValidSessionNumber(-1) == false);
    assert(ScenarioSelector::isValidSessionNumber(100) == false);
    
    std::cout << "✓ All isValidSessionNumber tests passed" << std::endl;
}

void test_IsValidTestNumber() {
    std::cout << "Testing isValidTestNumber..." << std::endl;
    
    // 有効な範囲
    assert(ScenarioSelector::isValidTestNumber(1) == true);
    assert(ScenarioSelector::isValidTestNumber(3) == true);
    assert(ScenarioSelector::isValidTestNumber(6) == true);
    
    // 無効な範囲
    assert(ScenarioSelector::isValidTestNumber(0) == false);
    assert(ScenarioSelector::isValidTestNumber(7) == false);
    assert(ScenarioSelector::isValidTestNumber(-1) == false);
    
    std::cout << "✓ All isValidTestNumber tests passed" << std::endl;
}

void test_GetScenarioPath() {
    std::cout << "Testing getScenarioPath..." << std::endl;
    
    // 正常なパス生成
    assert(ScenarioSelector::getScenarioPath(1, 1) == "scenario/experiment/session01_test1.json");
    assert(ScenarioSelector::getScenarioPath(1, 6) == "scenario/experiment/session01_test6.json");
    assert(ScenarioSelector::getScenarioPath(6, 1) == "scenario/experiment/session06_test1.json");
    assert(ScenarioSelector::getScenarioPath(6, 6) == "scenario/experiment/session06_test6.json");
    assert(ScenarioSelector::getScenarioPath(3, 4) == "scenario/experiment/session03_test4.json");
    
    // 無効な番号の場合は空文字列
    assert(ScenarioSelector::getScenarioPath(0, 1) == "");
    assert(ScenarioSelector::getScenarioPath(1, 0) == "");
    assert(ScenarioSelector::getScenarioPath(7, 1) == "");
    assert(ScenarioSelector::getScenarioPath(1, 7) == "");
    assert(ScenarioSelector::getScenarioPath(-1, 1) == "");
    
    std::cout << "✓ All getScenarioPath tests passed" << std::endl;
}

void test_ScenarioExists() {
    std::cout << "Testing scenarioExists..." << std::endl;
    
    // 実際のシナリオファイルの存在確認
    // session01_test1.json は存在するはず
    std::string path1 = ScenarioSelector::getScenarioPath(1, 1);
    bool exists1 = ScenarioSelector::scenarioExists(path1);
    std::cout << "  Checking " << path1 << ": " << (exists1 ? "exists" : "not found") << std::endl;
    assert(exists1 == true);  // 実際のファイルが存在することを確認
    
    // session06_test6.json も存在するはず
    std::string path2 = ScenarioSelector::getScenarioPath(6, 6);
    bool exists2 = ScenarioSelector::scenarioExists(path2);
    std::cout << "  Checking " << path2 << ": " << (exists2 ? "exists" : "not found") << std::endl;
    assert(exists2 == true);
    
    // 存在しないファイル
    assert(ScenarioSelector::scenarioExists("scenario/experiment/nonexistent.json") == false);
    
    std::cout << "✓ All scenarioExists tests passed" << std::endl;
}

void test_AllScenarioFiles() {
    std::cout << "Testing all 36 scenario files..." << std::endl;
    
    int totalFiles = 0;
    int existingFiles = 0;
    
    for (int session = 1; session <= 6; session++) {
        for (int test = 1; test <= 6; test++) {
            totalFiles++;
            std::string path = ScenarioSelector::getScenarioPath(session, test);
            if (ScenarioSelector::scenarioExists(path)) {
                existingFiles++;
            } else {
                std::cout << "  ✗ Missing: " << path << std::endl;
            }
        }
    }
    
    std::cout << "  Found " << existingFiles << " / " << totalFiles << " scenario files" << std::endl;
    assert(existingFiles == 36);  // 全36ファイルが存在することを確認
    
    std::cout << "✓ All 36 scenario files exist" << std::endl;
}

int main() {
    std::cout << "=== Scenario Selector Tests ===" << std::endl;
    
    test_IsValidSessionNumber();
    test_IsValidTestNumber();
    test_GetScenarioPath();
    test_ScenarioExists();
    test_AllScenarioFiles();
    
    std::cout << "\n✓✓✓ All tests passed! ✓✓✓" << std::endl;
    return 0;
}

#include "scenario_selector.h"
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace fs = std::filesystem;

namespace ScenarioSelector {

    // セッション番号の有効範囲
    const int MIN_SESSION = 1;
    const int MAX_SESSION = 6;

    // テスト番号の有効範囲
    const int MIN_TEST = 1;
    const int MAX_TEST = 6;

    bool isValidSessionNumber(int sessionNumber) {
        return sessionNumber >= MIN_SESSION && sessionNumber <= MAX_SESSION;
    }

    bool isValidTestNumber(int testNumber) {
        return testNumber >= MIN_TEST && testNumber <= MAX_TEST;
    }

    std::string getScenarioPath(int sessionNumber, int testNumber) {
        // 範囲チェック
        if (!isValidSessionNumber(sessionNumber) || !isValidTestNumber(testNumber)) {
            return "";  // 無効な番号の場合は空文字列
        }

        // ファイル名を生成: sessionXX_testY.json
        std::ostringstream oss;
        oss << "scenario/experiment/session"
            << std::setfill('0') << std::setw(2) << sessionNumber
            << "_test" << testNumber << ".json";

        return oss.str();
    }

    bool scenarioExists(const std::string& filepath) {
        return fs::exists(filepath);
    }

} // namespace ScenarioSelector

#pragma once

// scenario_selector.h
// 実験モード用シナリオ選択システム（Phase 6で追加）
//
// 用途：セッション番号とテスト番号から適切なシナリオファイルを選択

#include <string>

namespace ScenarioSelector {

    // シナリオファイルパスを生成
    // sessionNumber: セッション番号 (1-6)
    // testNumber: テスト番号 (1-6)
    // 戻り値: "scenario/experiment/sessionXX_testY.json"
    std::string getScenarioPath(int sessionNumber, int testNumber);

    // セッション番号が有効か確認
    // sessionNumber: セッション番号 (1-6)
    // 戻り値: 有効ならtrue、無効ならfalse
    bool isValidSessionNumber(int sessionNumber);

    // テスト番号が有効か確認
    // testNumber: テスト番号 (1-6)
    // 戻り値: 有効ならtrue、無効ならfalse
    bool isValidTestNumber(int testNumber);

    // シナリオファイルが存在するか確認
    // filepath: シナリオファイルパス
    // 戻り値: 存在するならtrue、存在しないならfalse
    bool scenarioExists(const std::string& filepath);

} // namespace ScenarioSelector

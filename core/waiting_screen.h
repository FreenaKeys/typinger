#pragma once

// waiting_screen.h
// 実験モード用待機画面（Phase 6で追加）
//
// 用途：テスト間の3秒待機とカウントダウン表示

#include <string>

namespace WaitingScreen {

    // 待機画面を表示してカウントダウン
    // testNumber: 次のテスト番号 (1-6)
    // layoutType: 次の配列タイプ ("new" or "old")
    // durationSeconds: 待機時間（秒、デフォルト: 3）
    void showCountdown(int testNumber, const std::string& layoutType, int durationSeconds = 3);

    // 待機画面のメッセージを表示（カウントダウンなし）
    // message: 表示するメッセージ
    void showMessage(const std::string& message);

    // 待機画面をクリア
    void clear();

    // 配列タイプの日本語名を取得
    // layoutType: "new" or "old"
    // 戻り値: "新配列" or "旧配列"
    std::string getLayoutName(const std::string& layoutType);

} // namespace WaitingScreen

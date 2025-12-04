#pragma once
#include <string>
#include "experiment_session.h"

namespace ExperimentUI {
    // モード選択結果
    enum class Mode {
        NORMAL,      // 通常モード
        EXPERIMENT,  // 実験モード
        QUIT         // 終了
    };

    // モード選択画面を表示
    Mode selectMode();

    // 被験者ID入力画面を表示
    std::string inputSubjectID();

    // セッション番号入力画面を表示
    int inputSessionNumber();

    // 実験セッション情報入力（ID + セッション番号）
    ExperimentSession inputExperimentSession();

    // エラーメッセージ表示
    void showError(const std::string& message);

    // 確認メッセージ表示
    void showConfirmation(const ExperimentSession& session);

} // namespace ExperimentUI

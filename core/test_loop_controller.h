#pragma once

// test_loop_controller.h
// 実験モード用テストループ制御（Phase 6で追加）
//
// 用途：1セッション6テストの自動ループ管理

#include <string>
#include "experiment_session.h"

namespace TestLoopController {

    // テストループ状態
    struct LoopState {
        int currentTestNumber;       // 現在のテスト番号 (1-6)
        int totalTests;              // 総テスト数 (デフォルト: 6)
        std::string currentLayout;   // 現在の配列タイプ ("new" or "old")
        bool isComplete;             // 全テスト完了フラグ
        
        LoopState() 
            : currentTestNumber(1), 
              totalTests(6), 
              currentLayout(""), 
              isComplete(false) {}
    };

    // テストループコントローラークラス
    class Controller {
    private:
        LoopState state;
        std::string firstLayout;     // 最初の配列タイプ
        int sessionNumber;           // セッション番号

    public:
        // コンストラクタ
        // firstLayoutType: 最初にテストする配列タイプ ("new" or "old")
        // session: セッション番号 (1-14)
        Controller(const std::string& firstLayoutType, int session);

        // 次のテストに進む
        // 戻り値: 次のテストに進めた場合true、全テスト完了の場合false
        bool next();

        // 現在のテスト番号を取得
        int getCurrentTestNumber() const;

        // 現在の配列タイプを取得
        std::string getCurrentLayout() const;

        // 全テスト完了したか確認
        bool isComplete() const;

        // 残りテスト数を取得
        int getRemainingTests() const;

        // 進捗率を取得 (0.0～1.0)
        double getProgress() const;

        // リセット
        void reset();

        // 現在の状態を取得
        const LoopState& getState() const;
    };

} // namespace TestLoopController

#pragma once

// typing_judge.h
// タイピング判定ロジック
//
// 用語解説:
// - rubi（ルビ）: 目標となるローマ字入力列（例: "konnichiwa"）
// - 逐次判定（Incremental Judgment）: 1文字ずつ入力を照合
// - 正誤フラグ（Correct/Incorrect Flag）: 各入力が正しいか間違っているか

#include <string>
#include <vector>

namespace TypingJudge {

    // 判定結果
    enum class JudgeResult {
        CORRECT,        // 正解
        INCORRECT,      // 不正解
        ALREADY_DONE    // すでに完了済み（追加入力不要）
    };

    // タイピング判定クラス
    class Judge {
    private:
        std::string targetText_;        // 目標テキスト（日本語）
        std::string targetRubi_;        // 目標ルビ（ローマ字、小文字正規化済み）
        size_t currentPosition_;        // 現在の判定位置（0-based index）
        size_t correctCount_;           // 正解数
        size_t incorrectCount_;         // 不正解数
        std::vector<char> inputHistory_; // 入力履歴（デバッグ用）

    public:
        // コンストラクタ
        // targetText: 目標テキスト（日本語）
        // targetRubi: 目標ルビ（ローマ字）
        Judge(const std::string& targetText, const std::string& targetRubi);

        // 1文字判定
        // input: 入力文字
        // 戻り値: 判定結果（CORRECT/INCORRECT/ALREADY_DONE）
        JudgeResult judgeChar(char input);

        // 現在位置の取得
        size_t getCurrentPosition() const { return currentPosition_; }

        // 目標ルビ長の取得
        size_t getTargetLength() const { return targetRubi_.length(); }

        // 正解数の取得
        size_t getCorrectCount() const { return correctCount_; }

        // 不正解数の取得
        size_t getIncorrectCount() const { return incorrectCount_; }

        // 完了判定
        // 戻り値: すべて入力完了したかどうか
        bool isCompleted() const { return currentPosition_ >= targetRubi_.length(); }

        // 正解率の取得
        // 戻り値: 正解率（0.0 ～ 1.0）
        double getAccuracy() const;

        // 残りルビの取得
        // 戻り値: 未入力のルビ部分
        std::string getRemainingRubi() const;

        // 目標テキストの取得
        const std::string& getTargetText() const { return targetText_; }

        // 目標ルビの取得
        const std::string& getTargetRubi() const { return targetRubi_; }

        // リセット
        void reset();
    };

} // namespace TypingJudge
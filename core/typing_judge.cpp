// typing_judge.cpp
// タイピング判定ロジックの実装

#include "typing_judge.h"
#include <algorithm>

namespace TypingJudge {

    // コンストラクタ
    Judge::Judge(const std::string& targetText, const std::string& targetRubi)
        : targetText_(targetText)
        , targetRubi_(targetRubi)
        , currentPosition_(0)
        , correctCount_(0)
        , incorrectCount_(0)
    {
        // targetRubiを小文字に正規化
        std::transform(targetRubi_.begin(), targetRubi_.end(), targetRubi_.begin(), ::tolower);
    }

    // 1文字判定
    JudgeResult Judge::judgeChar(char input) {
        // すでに完了している場合
        if (isCompleted()) {
            return JudgeResult::ALREADY_DONE;
        }

        // 入力を小文字に正規化
        char normalizedInput = tolower(input);

        // 入力履歴に追加
        inputHistory_.push_back(normalizedInput);

        // 期待される文字と照合
        char expected = targetRubi_[currentPosition_];

        if (normalizedInput == expected) {
            // 正解
            correctCount_++;
            currentPosition_++;
            return JudgeResult::CORRECT;
        } else {
            // 不正解
            incorrectCount_++;
            return JudgeResult::INCORRECT;
        }
    }

    // 正解率の取得
    double Judge::getAccuracy() const {
        size_t totalInputs = correctCount_ + incorrectCount_;
        if (totalInputs == 0) {
            return 0.0;
        }
        return static_cast<double>(correctCount_) / static_cast<double>(totalInputs);
    }

    // 残りルビの取得
    std::string Judge::getRemainingRubi() const {
        if (isCompleted()) {
            return "";
        }
        return targetRubi_.substr(currentPosition_);
    }

    // リセット
    void Judge::reset() {
        currentPosition_ = 0;
        correctCount_ = 0;
        incorrectCount_ = 0;
        inputHistory_.clear();
    }

} // namespace TypingJudge
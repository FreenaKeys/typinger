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
        } 
        // 柔軟な入力対応 (Flexible Input)
        else {
            // "shi" -> "si" (skip 'h')
            // 前の文字が's'、現在が'h'、入力が'i'の場合
            if (currentPosition_ > 0 && targetRubi_[currentPosition_-1] == 's' && expected == 'h' && normalizedInput == 'i') {
                correctCount_++;
                currentPosition_ += 2; // 'h'と'i'を消費
                return JudgeResult::CORRECT;
            }
            
            // "tsu" -> "tu" (skip 's')
            // 前の文字が't'、現在が's'、入力が'u'の場合
            if (currentPosition_ > 0 && targetRubi_[currentPosition_-1] == 't' && expected == 's' && normalizedInput == 'u') {
                correctCount_++;
                currentPosition_ += 2; // 's'と'u'を消費
                return JudgeResult::CORRECT;
            }

            // "chi" -> "ti"
            // 現在が"chi"の先頭'c'で、入力が't'の場合
            if (expected == 'c' && normalizedInput == 't') {
                if (targetRubi_.length() >= currentPosition_ + 3 && targetRubi_.substr(currentPosition_, 3) == "chi") {
                    correctCount_++;
                    currentPosition_ += 2; // 'c'と'h'を消費（次は'i'）
                    return JudgeResult::CORRECT;
                }
            }

            // "fu" -> "hu"
            // 現在が"fu"の先頭'f'で、入力が'h'の場合
            if (expected == 'f' && normalizedInput == 'h') {
                if (targetRubi_.length() >= currentPosition_ + 2 && targetRubi_.substr(currentPosition_, 2) == "fu") {
                    correctCount_++;
                    currentPosition_ += 1; // 'f'を消費（次は'u'）
                    return JudgeResult::CORRECT;
                }
            }

            // "ji" -> "zi"
            // 現在が"ji"の先頭'j'で、入力が'z'の場合
            if (expected == 'j' && normalizedInput == 'z') {
                if (targetRubi_.length() >= currentPosition_ + 2 && targetRubi_.substr(currentPosition_, 2) == "ji") {
                    correctCount_++;
                    currentPosition_ += 1; // 'j'を消費（次は'i'）
                    return JudgeResult::CORRECT;
                }
            }

            // NEW: "ja/ju/jo" -> "zya/zyu/zyo" (Step 1: 'j' -> 'z')
            // Target: ...j[a/u/o]...
            // Input: z
            if (expected == 'j' && normalizedInput == 'z') {
                 correctCount_++;
                 currentPosition_++; // Consume 'j'. Next is vowel.
                 return JudgeResult::CORRECT;
            }

            // NEW: "ja/ju/jo" -> "jya/jyu/jyo" (Insertion of 'y')
            // Target: ...j[a/u/o]...
            // Input: y
            // Context: prev='j', expected is vowel
            if (currentPosition_ > 0 && targetRubi_[currentPosition_-1] == 'j' && normalizedInput == 'y') {
                // Check if expected is a vowel (a, u, o)
                if (std::string("auo").find(expected) != std::string::npos) {
                    correctCount_++;
                    // Do NOT consume the vowel. We just accepted 'y' as an extra char.
                    return JudgeResult::CORRECT;
                }
            }

            // NEW: "sha/shu/sho" -> "sya/syu/syo"
            // Target: ...sh[a/u/o]...
            // Input: y
            // Context: prev='s', expected='h'
            if (currentPosition_ > 0 && targetRubi_[currentPosition_-1] == 's' && expected == 'h' && normalizedInput == 'y') {
                 correctCount_++;
                 currentPosition_++; // Consume 'h'
                 return JudgeResult::CORRECT;
            }

            // NEW: "cha/chu/cho" -> "tya/tyu/tyo" (Step 1: 'c' -> 't')
            // Target: ...ch[a/u/o]...
            // Input: t
            // Context: expected='c', next='h'
            if (expected == 'c' && normalizedInput == 't') {
                if (targetRubi_.length() > currentPosition_ + 1 && targetRubi_[currentPosition_+1] == 'h') {
                    // "cha/chu/cho" -> "tya..." (Note: "chi" -> "ti" is handled above)
                    correctCount_++;
                    currentPosition_++; // Consume 'c'. Next expected is 'h'.
                    return JudgeResult::CORRECT;
                }
            }

            // NEW: "cha/chu/cho" -> "tya/tyu/tyo" (Step 2: 'h' -> 'y')
            // Target: ...ch[a/u/o]... (we are at 'h' now, prev was 'c')
            // Input: y
            // Context: prev='c', expected='h'
            if (currentPosition_ > 0 && targetRubi_[currentPosition_-1] == 'c' && expected == 'h' && normalizedInput == 'y') {
                 correctCount_++;
                 currentPosition_++; // Consume 'h'
                 return JudgeResult::CORRECT;
            }

            // 促音 "cch" (っち) -> "tch" 対応
            // Target: "cch...", Input: 't'
            // 't' is accepted for the first 'c', effectively changing expectation to "tch..."
            if (expected == 'c' && normalizedInput == 't') {
                if (targetRubi_.length() >= currentPosition_ + 3 && targetRubi_.substr(currentPosition_, 3) == "cch") {
                    correctCount_++;
                    currentPosition_ += 1; // 最初の 'c' を消費。次は 'c' (tchのcとしてマッチさせる)
                    return JudgeResult::CORRECT;
                }
            }

            // 撥音 "nn" (ん) -> "n" (省略) 対応
            // Target: "n" (2nd n of nn), Input: Consonant (not n, not vowel)
            if (expected == 'n') {
                if (targetRubi_.length() > currentPosition_ + 1) {
                    char nextTarget = targetRubi_[currentPosition_ + 1];
                    
                    // Case 1: Direct Match (Existing)
                    if (normalizedInput == nextTarget) {
                        std::string vowels = "aiueon"; 
                        if (vowels.find(nextTarget) == std::string::npos && nextTarget != 'y') {
                            correctCount_++;
                            currentPosition_ += 2; // Consume 'n' and the next char (which matched input)
                            return JudgeResult::CORRECT;
                        }
                    }

                    // Case 2: Flexible Match for Next Char
                    // "cha/chu/cho" -> "tya/tyu/tyo" (Step 1: 'c' -> 't')
                    // Target: "nncha" -> "ntya"
                    // expected='n', nextTarget='c', input='t'
                    if (nextTarget == 'c' && normalizedInput == 't') {
                         if (targetRubi_.length() > currentPosition_ + 2 && targetRubi_[currentPosition_+2] == 'h') {
                             // "nncha" -> "ntya"
                             // Consume 'n' (Pos 1) and 'c' (Pos 2).
                             // Next expected will be 'h' (Pos 3).
                             correctCount_++;
                             currentPosition_ += 2; 
                             return JudgeResult::CORRECT;
                         }
                    }
                    
                    // "ja/ju/jo" -> "zya/zyu/zyo" (Step 1: 'j' -> 'z')
                    // Target: "nnja" -> "nzya"
                    // expected='n', nextTarget='j', input='z'
                    if (nextTarget == 'j' && normalizedInput == 'z') {
                         // "nnja" -> "nzya"
                         // Consume 'n' (Pos 1) and 'j' (Pos 2).
                         // Next expected will be vowel (Pos 3).
                         correctCount_++;
                         currentPosition_ += 2;
                         return JudgeResult::CORRECT;
                    }

                    // "fu" -> "hu" (f -> h)
                    // Target: "nnfu" -> "nhu"
                    if (nextTarget == 'f' && normalizedInput == 'h') {
                         if (targetRubi_.length() > currentPosition_ + 2 && targetRubi_[currentPosition_+2] == 'u') {
                             correctCount_++;
                             currentPosition_ += 2; // Consume 'n' and 'f'.
                             return JudgeResult::CORRECT;
                         }
                    }
                }
            }
            
            // 撥音の後の「chi」->「ti」対応
            // 「んち」が「nchi」として登録されている場合、「nti」でも正解とする
            // Target: "c" (after 'n'), Input: 't'
            // Context: 前の文字が'n'で、現在が"chi"の先頭'c'、入力が't'の場合
            if (currentPosition_ > 0 && targetRubi_[currentPosition_-1] == 'n' && expected == 'c' && normalizedInput == 't') {
                if (targetRubi_.length() >= currentPosition_ + 3 && targetRubi_.substr(currentPosition_, 3) == "chi") {
                    correctCount_++;
                    currentPosition_ += 2; // 'c'と'h'を消費（次は'i'）
                    return JudgeResult::CORRECT;
                }
            }

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
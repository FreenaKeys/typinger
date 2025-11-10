// typing_judge_test.cpp
// タイピング判定器のユニットテスト

#include "../core/typing_judge.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace TypingJudge;

void test_basic_judgment() {
    std::cout << "Test: Basic judgment (基本判定)..." << std::endl;
    
    Judge judge("こんにちは", "konnichiwa");
    
    // 正しい入力
    assert(judge.judgeChar('k') == JudgeResult::CORRECT);
    assert(judge.getCurrentPosition() == 1);
    assert(judge.getCorrectCount() == 1);
    assert(judge.getIncorrectCount() == 0);
    
    assert(judge.judgeChar('o') == JudgeResult::CORRECT);
    assert(judge.getCurrentPosition() == 2);
    
    std::cout << "  PASS" << std::endl;
}

void test_incorrect_input() {
    std::cout << "Test: Incorrect input (誤入力)..." << std::endl;
    
    Judge judge("あいうえお", "aiueo");
    
    // 正解
    assert(judge.judgeChar('a') == JudgeResult::CORRECT);
    
    // 不正解（期待は'i'だが'x'を入力）
    assert(judge.judgeChar('x') == JudgeResult::INCORRECT);
    assert(judge.getCurrentPosition() == 1); // 位置は進まない
    assert(judge.getCorrectCount() == 1);
    assert(judge.getIncorrectCount() == 1);
    
    // 再度正解
    assert(judge.judgeChar('i') == JudgeResult::CORRECT);
    assert(judge.getCurrentPosition() == 2);
    
    std::cout << "  PASS" << std::endl;
}

void test_completion() {
    std::cout << "Test: Completion (完了判定)..." << std::endl;
    
    Judge judge("あ", "a");
    
    assert(!judge.isCompleted());
    
    judge.judgeChar('a');
    
    assert(judge.isCompleted());
    assert(judge.getCurrentPosition() == judge.getTargetLength());
    
    // 完了後の入力
    assert(judge.judgeChar('x') == JudgeResult::ALREADY_DONE);
    
    std::cout << "  PASS" << std::endl;
}

void test_accuracy() {
    std::cout << "Test: Accuracy calculation (正答率計算)..." << std::endl;
    
    Judge judge("てすと", "tesuto");
    
    // 初期状態
    assert(judge.getAccuracy() == 0.0);
    
    // 正解3回
    judge.judgeChar('t');
    judge.judgeChar('e');
    judge.judgeChar('s');
    assert(judge.getAccuracy() == 1.0); // 3/3 = 1.0
    
    // 不正解1回
    judge.judgeChar('x');
    assert(std::fabs(judge.getAccuracy() - 0.75) < 0.001); // 3/4 = 0.75
    
    // 正解2回（'u', 't'）
    judge.judgeChar('u');
    judge.judgeChar('t');
    assert(std::fabs(judge.getAccuracy() - (5.0 / 6.0)) < 0.001); // 5/6
    
    std::cout << "  PASS" << std::endl;
}

void test_remaining_rubi() {
    std::cout << "Test: Remaining rubi (残りローマ字)..." << std::endl;
    
    Judge judge("さくら", "sakura");
    
    assert(judge.getRemainingRubi() == "sakura");
    
    judge.judgeChar('s');
    judge.judgeChar('a');
    assert(judge.getRemainingRubi() == "kura");
    
    judge.judgeChar('k');
    judge.judgeChar('u');
    judge.judgeChar('r');
    assert(judge.getRemainingRubi() == "a");
    
    judge.judgeChar('a');
    assert(judge.getRemainingRubi() == "");
    
    std::cout << "  PASS" << std::endl;
}

void test_case_insensitive() {
    std::cout << "Test: Case insensitive (大文字小文字無視)..." << std::endl;
    
    Judge judge("テスト", "test");
    
    // 小文字で入力
    assert(judge.judgeChar('t') == JudgeResult::CORRECT);
    assert(judge.judgeChar('e') == JudgeResult::CORRECT);
    
    // 大文字で入力しても正解
    assert(judge.judgeChar('S') == JudgeResult::CORRECT);
    assert(judge.judgeChar('T') == JudgeResult::CORRECT);
    
    assert(judge.isCompleted());
    
    std::cout << "  PASS" << std::endl;
}

void test_reset() {
    std::cout << "Test: Reset (リセット)..." << std::endl;
    
    Judge judge("りんご", "ringo");
    
    // 途中まで入力
    judge.judgeChar('r');
    judge.judgeChar('i');
    judge.judgeChar('x'); // 誤入力
    assert(judge.getCurrentPosition() == 2);
    assert(judge.getCorrectCount() == 2);
    assert(judge.getIncorrectCount() == 1);
    
    // リセット
    judge.reset();
    assert(judge.getCurrentPosition() == 0);
    assert(judge.getCorrectCount() == 0);
    assert(judge.getIncorrectCount() == 0);
    assert(!judge.isCompleted());
    assert(judge.getRemainingRubi() == "ringo");
    
    std::cout << "  PASS" << std::endl;
}

void test_getters() {
    std::cout << "Test: Getters (取得メソッド)..." << std::endl;
    
    Judge judge("もも", "momo");
    
    assert(judge.getTargetText() == "もも");
    assert(judge.getTargetRubi() == "momo");
    assert(judge.getTargetLength() == 4);
    
    std::cout << "  PASS" << std::endl;
}

void test_full_sequence() {
    std::cout << "Test: Full sequence (完全シーケンス)..." << std::endl;
    
    Judge judge("ありがとう", "arigatou");
    
    // 完全に正しく入力
    assert(judge.judgeChar('a') == JudgeResult::CORRECT);
    assert(judge.judgeChar('r') == JudgeResult::CORRECT);
    assert(judge.judgeChar('i') == JudgeResult::CORRECT);
    assert(judge.judgeChar('g') == JudgeResult::CORRECT);
    assert(judge.judgeChar('a') == JudgeResult::CORRECT);
    assert(judge.judgeChar('t') == JudgeResult::CORRECT);
    assert(judge.judgeChar('o') == JudgeResult::CORRECT);
    assert(judge.judgeChar('u') == JudgeResult::CORRECT);
    
    assert(judge.isCompleted());
    assert(judge.getCorrectCount() == 8);
    assert(judge.getIncorrectCount() == 0);
    assert(judge.getAccuracy() == 1.0);
    
    std::cout << "  PASS" << std::endl;
}

int main() {
    std::cout << "=== Typing Judge Unit Tests ===" << std::endl;
    std::cout << std::endl;

    test_basic_judgment();
    test_incorrect_input();
    test_completion();
    test_accuracy();
    test_remaining_rubi();
    test_case_insensitive();
    test_reset();
    test_getters();
    test_full_sequence();

    std::cout << std::endl;
    std::cout << "All tests passed! ✓" << std::endl;
    return 0;
}

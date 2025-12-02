#include "test_loop_controller.h"

namespace TestLoopController {

    Controller::Controller(const std::string& firstLayoutType, int session)
        : firstLayout(firstLayoutType), sessionNumber(session) {
        state.currentTestNumber = 1;
        state.totalTests = 6;
        state.isComplete = false;
        
        // 最初の配列タイプを設定
        state.currentLayout = getLayoutTypeForTest(firstLayout, state.currentTestNumber);
    }

    bool Controller::next() {
        // すでに完了している場合
        if (state.isComplete) {
            return false;
        }

        // 次のテストに進む
        state.currentTestNumber++;

        // 全テスト完了チェック
        if (state.currentTestNumber > state.totalTests) {
            state.isComplete = true;
            return false;
        }

        // 配列タイプを更新
        state.currentLayout = getLayoutTypeForTest(firstLayout, state.currentTestNumber);

        return true;
    }

    int Controller::getCurrentTestNumber() const {
        return state.currentTestNumber;
    }

    std::string Controller::getCurrentLayout() const {
        return state.currentLayout;
    }

    bool Controller::isComplete() const {
        return state.isComplete;
    }

    int Controller::getRemainingTests() const {
        if (state.isComplete) {
            return 0;
        }
        return state.totalTests - state.currentTestNumber + 1;
    }

    double Controller::getProgress() const {
        if (state.isComplete) {
            return 1.0;
        }
        // 完了したテスト数 / 総テスト数
        int completedTests = state.currentTestNumber - 1;
        return static_cast<double>(completedTests) / state.totalTests;
    }

    void Controller::reset() {
        state.currentTestNumber = 1;
        state.isComplete = false;
        state.currentLayout = getLayoutTypeForTest(firstLayout, state.currentTestNumber);
    }

    const LoopState& Controller::getState() const {
        return state;
    }

} // namespace TestLoopController

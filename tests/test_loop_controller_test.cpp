#include "../core/test_loop_controller.h"
#include <cassert>
#include <iostream>

void test_ControllerConstruction() {
    std::cout << "Testing controller construction..." << std::endl;
    
    // 新配列から開始
    TestLoopController::Controller controller1("new", 1);
    assert(controller1.getCurrentTestNumber() == 1);
    assert(controller1.getCurrentLayout() == "new");
    assert(controller1.isComplete() == false);
    assert(controller1.getRemainingTests() == 6);
    
    // 旧配列から開始
    TestLoopController::Controller controller2("old", 2);
    assert(controller2.getCurrentTestNumber() == 1);
    assert(controller2.getCurrentLayout() == "old");
    assert(controller2.isComplete() == false);
    
    std::cout << "✓ All controller construction tests passed" << std::endl;
}

void test_LayoutAlternation() {
    std::cout << "Testing layout alternation..." << std::endl;
    
    // 新配列から開始（奇数セッション）
    TestLoopController::Controller controller("new", 1);
    
    // Test 1: 新配列
    assert(controller.getCurrentTestNumber() == 1);
    assert(controller.getCurrentLayout() == "new");
    
    // Test 2: 旧配列
    controller.next();
    assert(controller.getCurrentTestNumber() == 2);
    assert(controller.getCurrentLayout() == "old");
    
    // Test 3: 新配列
    controller.next();
    assert(controller.getCurrentTestNumber() == 3);
    assert(controller.getCurrentLayout() == "new");
    
    // Test 4: 旧配列
    controller.next();
    assert(controller.getCurrentTestNumber() == 4);
    assert(controller.getCurrentLayout() == "old");
    
    // Test 5: 新配列
    controller.next();
    assert(controller.getCurrentTestNumber() == 5);
    assert(controller.getCurrentLayout() == "new");
    
    // Test 6: 旧配列
    controller.next();
    assert(controller.getCurrentTestNumber() == 6);
    assert(controller.getCurrentLayout() == "old");
    
    std::cout << "✓ All layout alternation tests passed" << std::endl;
}

void test_LayoutAlternationReverse() {
    std::cout << "Testing layout alternation (reverse)..." << std::endl;
    
    // 旧配列から開始（偶数セッション）
    TestLoopController::Controller controller("old", 2);
    
    // Test 1: 旧配列
    assert(controller.getCurrentTestNumber() == 1);
    assert(controller.getCurrentLayout() == "old");
    
    // Test 2: 新配列
    controller.next();
    assert(controller.getCurrentTestNumber() == 2);
    assert(controller.getCurrentLayout() == "new");
    
    // Test 3: 旧配列
    controller.next();
    assert(controller.getCurrentTestNumber() == 3);
    assert(controller.getCurrentLayout() == "old");
    
    // Test 4: 新配列
    controller.next();
    assert(controller.getCurrentTestNumber() == 4);
    assert(controller.getCurrentLayout() == "new");
    
    // Test 5: 旧配列
    controller.next();
    assert(controller.getCurrentTestNumber() == 5);
    assert(controller.getCurrentLayout() == "old");
    
    // Test 6: 新配列
    controller.next();
    assert(controller.getCurrentTestNumber() == 6);
    assert(controller.getCurrentLayout() == "new");
    
    std::cout << "✓ All layout alternation (reverse) tests passed" << std::endl;
}

void test_LoopCompletion() {
    std::cout << "Testing loop completion..." << std::endl;
    
    TestLoopController::Controller controller("new", 1);
    
    // 最初は未完了
    assert(controller.isComplete() == false);
    assert(controller.getRemainingTests() == 6);
    
    // Test 1-5を進める
    for (int i = 0; i < 5; i++) {
        assert(controller.next() == true);
        assert(controller.isComplete() == false);
    }
    
    // Test 6の時点
    assert(controller.getCurrentTestNumber() == 6);
    assert(controller.getRemainingTests() == 1);
    
    // Test 6を完了
    assert(controller.next() == false);  // もう次がない
    assert(controller.isComplete() == true);
    assert(controller.getRemainingTests() == 0);
    
    // 完了後はnext()がfalseを返す
    assert(controller.next() == false);
    
    std::cout << "✓ All loop completion tests passed" << std::endl;
}

void test_Progress() {
    std::cout << "Testing progress..." << std::endl;
    
    TestLoopController::Controller controller("new", 1);
    
    // Test 1: 0% (まだ1つも完了していない)
    double progress = controller.getProgress();
    std::cout << "  Test 1 progress: " << (progress * 100) << "%" << std::endl;
    assert(progress == 0.0);
    
    // Test 2に進む: 16.67% (1/6完了)
    controller.next();
    progress = controller.getProgress();
    std::cout << "  Test 2 progress: " << (progress * 100) << "%" << std::endl;
    assert(progress > 0.16 && progress < 0.17);
    
    // Test 4に進む: 50% (3/6完了)
    controller.next();
    controller.next();
    progress = controller.getProgress();
    std::cout << "  Test 4 progress: " << (progress * 100) << "%" << std::endl;
    assert(progress == 0.5);
    
    // Test 7に進む（完了）: 100%
    controller.next();
    controller.next();
    controller.next();
    progress = controller.getProgress();
    std::cout << "  Completed progress: " << (progress * 100) << "%" << std::endl;
    assert(progress == 1.0);
    
    std::cout << "✓ All progress tests passed" << std::endl;
}

void test_Reset() {
    std::cout << "Testing reset..." << std::endl;
    
    TestLoopController::Controller controller("new", 1);
    
    // いくつかテストを進める
    controller.next();
    controller.next();
    assert(controller.getCurrentTestNumber() == 3);
    assert(controller.isComplete() == false);
    
    // リセット
    controller.reset();
    
    // 初期状態に戻る
    assert(controller.getCurrentTestNumber() == 1);
    assert(controller.getCurrentLayout() == "new");
    assert(controller.isComplete() == false);
    assert(controller.getRemainingTests() == 6);
    
    std::cout << "✓ All reset tests passed" << std::endl;
}

void test_GetState() {
    std::cout << "Testing getState..." << std::endl;
    
    TestLoopController::Controller controller("new", 1);
    
    const TestLoopController::LoopState& state = controller.getState();
    
    assert(state.currentTestNumber == 1);
    assert(state.totalTests == 6);
    assert(state.currentLayout == "new");
    assert(state.isComplete == false);
    
    // 進める
    controller.next();
    controller.next();
    
    const TestLoopController::LoopState& state2 = controller.getState();
    assert(state2.currentTestNumber == 3);
    assert(state2.currentLayout == "new");
    
    std::cout << "✓ All getState tests passed" << std::endl;
}

int main() {
    std::cout << "=== Test Loop Controller Tests ===" << std::endl;
    
    test_ControllerConstruction();
    test_LayoutAlternation();
    test_LayoutAlternationReverse();
    test_LoopCompletion();
    test_Progress();
    test_Reset();
    test_GetState();
    
    std::cout << "\n✓✓✓ All tests passed! ✓✓✓" << std::endl;
    return 0;
}

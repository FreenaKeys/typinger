#include "../core/experiment_session.h"
#include <cassert>
#include <iostream>

void test_isValidSubjectID() {
    std::cout << "Testing isValidSubjectID..." << std::endl;
    
    // 有効なID
    assert(isValidSubjectID("SK") == true);
    assert(isValidSubjectID("MT") == true);
    assert(isValidSubjectID("IK") == true);
    assert(isValidSubjectID("KS") == true);
    assert(isValidSubjectID("KM") == true);
    
    // 無効なID
    assert(isValidSubjectID("AA") == false);
    assert(isValidSubjectID("sk") == false);  // 小文字
    assert(isValidSubjectID("") == false);
    assert(isValidSubjectID("SKK") == false);
    
    std::cout << "✓ All isValidSubjectID tests passed" << std::endl;
}

void test_isValidSessionNumber() {
    std::cout << "Testing isValidSessionNumber..." << std::endl;
    
    // 有効な範囲
    assert(isValidSessionNumber(1) == true);
    assert(isValidSessionNumber(7) == true);
    assert(isValidSessionNumber(14) == true);
    
    // 無効な範囲
    assert(isValidSessionNumber(0) == false);
    assert(isValidSessionNumber(15) == false);
    assert(isValidSessionNumber(-1) == false);
    assert(isValidSessionNumber(100) == false);
    
    std::cout << "✓ All isValidSessionNumber tests passed" << std::endl;
}

void test_isValidLayoutTypeNumber() {
    std::cout << "Testing isValidLayoutTypeNumber..." << std::endl;
    
    // 有効な番号
    assert(isValidLayoutTypeNumber(1) == true);
    assert(isValidLayoutTypeNumber(2) == true);
    
    // 無効な番号
    assert(isValidLayoutTypeNumber(0) == false);
    assert(isValidLayoutTypeNumber(3) == false);
    assert(isValidLayoutTypeNumber(-1) == false);
    
    std::cout << "✓ All isValidLayoutTypeNumber tests passed" << std::endl;
}

void test_getLayoutTypeForTest() {
    std::cout << "Testing getLayoutTypeForTest..." << std::endl;
    
    // 新配列から開始 (奇数セッション想定)
    // test1(奇数)→新, test2(偶数)→旧, test3(奇数)→新, test4(偶数)→旧, test5(奇数)→新, test6(偶数)→旧
    assert(getLayoutTypeForTest("new", 1) == "new");
    assert(getLayoutTypeForTest("new", 2) == "old");
    assert(getLayoutTypeForTest("new", 3) == "new");
    assert(getLayoutTypeForTest("new", 4) == "old");
    assert(getLayoutTypeForTest("new", 5) == "new");
    assert(getLayoutTypeForTest("new", 6) == "old");
    
    // 旧配列から開始 (偶数セッション想定)
    // test1(奇数)→旧, test2(偶数)→新, test3(奇数)→旧, test4(偶数)→新, test5(奇数)→旧, test6(偶数)→新
    assert(getLayoutTypeForTest("old", 1) == "old");
    assert(getLayoutTypeForTest("old", 2) == "new");
    assert(getLayoutTypeForTest("old", 3) == "old");
    assert(getLayoutTypeForTest("old", 4) == "new");
    assert(getLayoutTypeForTest("old", 5) == "old");
    assert(getLayoutTypeForTest("old", 6) == "new");
    
    std::cout << "✓ All getLayoutTypeForTest tests passed" << std::endl;
}

int main() {
    std::cout << "=== Experiment Session Tests ===" << std::endl;
    
    test_isValidSubjectID();
    test_isValidSessionNumber();
    test_isValidLayoutTypeNumber();
    test_getLayoutTypeForTest();
    
    std::cout << "\n✓✓✓ All tests passed! ✓✓✓" << std::endl;
    return 0;
}

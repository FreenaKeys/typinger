#include "../core/waiting_screen.h"
#include <cassert>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

void test_GetLayoutName() {
    std::cout << "Testing getLayoutName..." << std::endl;
    
    assert(WaitingScreen::getLayoutName("new") == "新配列");
    assert(WaitingScreen::getLayoutName("old") == "旧配列");
    assert(WaitingScreen::getLayoutName("unknown") == "不明");
    assert(WaitingScreen::getLayoutName("") == "不明");
    
    std::cout << "✓ All getLayoutName tests passed" << std::endl;
}

void test_ShowMessage() {
    std::cout << "Testing showMessage..." << std::endl;
    
    // メッセージ表示のテスト（視覚的確認）
    std::cout << "\n--- Visual Test: showMessage ---" << std::endl;
    WaitingScreen::showMessage("これはテストメッセージです");
    
    // 1秒待機して確認
    std::cout << "\n(1秒後に次のテストに進みます...)" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    std::cout << "✓ showMessage test completed (visual check)" << std::endl;
}

void test_ShowCountdown() {
    std::cout << "Testing showCountdown..." << std::endl;
    
    // カウントダウン表示のテスト（視覚的確認）
    std::cout << "\n--- Visual Test: showCountdown (1 second) ---" << std::endl;
    std::cout << "1秒のカウントダウンを表示します..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 1秒のカウントダウン（テスト用に短縮）
    WaitingScreen::showCountdown(1, "new", 1);
    
    std::cout << "\n✓ showCountdown test completed (visual check)" << std::endl;
}

void test_Clear() {
    std::cout << "Testing clear..." << std::endl;
    
    // メッセージを表示
    WaitingScreen::showMessage("画面をクリアします");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // クリア
    WaitingScreen::clear();
    
    // クリア後のメッセージ
    std::cout << "✓ clear test completed" << std::endl;
}

void test_CountdownWithDifferentLayouts() {
    std::cout << "Testing countdown with different layouts..." << std::endl;
    
    std::cout << "\n--- Visual Test: New Layout ---" << std::endl;
    WaitingScreen::showCountdown(2, "new", 1);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    std::cout << "\n--- Visual Test: Old Layout ---" << std::endl;
    WaitingScreen::showCountdown(3, "old", 1);
    
    std::cout << "\n✓ countdown with different layouts test completed" << std::endl;
}

int main() {
    // UTF-8出力を有効化（Windows）
    #ifdef _WIN32
    system("chcp 65001 > nul");
    #endif
    
    std::cout << "=== Waiting Screen Tests ===" << std::endl;
    std::cout << "Note: These tests include visual checks with actual display" << std::endl;
    std::cout << std::endl;
    
    test_GetLayoutName();
    test_ShowMessage();
    test_ShowCountdown();
    test_Clear();
    test_CountdownWithDifferentLayouts();
    
    // 最終メッセージ
    WaitingScreen::clear();
    std::cout << "\n✓✓✓ All tests passed! ✓✓✓" << std::endl;
    std::cout << "\nNote: Visual tests require manual verification" << std::endl;
    
    return 0;
}

#include "../helper/WinAPI/terminal.h"
#include "../core/waiting_screen.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>

void test_ColorConstants() {
    std::cout << "Testing color constants..." << std::endl;
    
    // 色定数が正しく定義されているか確認
    assert(Terminal::BLACK == 0);
    assert(Terminal::BLUE == 1);
    assert(Terminal::GREEN == 2);
    assert(Terminal::LIGHT_GREEN == 10);
    assert(Terminal::LIGHT_BLUE == 9);
    assert(Terminal::WHITE == 7);
    
    std::cout << "✓ Color constants test passed" << std::endl;
}

void test_SetTextColor() {
    std::cout << "Testing setTextColor..." << std::endl;
    
    std::cout << "\n--- Visual Test: Text Colors ---" << std::endl;
    
    // 緑色テスト
    Terminal::setTextColor(Terminal::LIGHT_GREEN);
    std::cout << "これは緑色のテキストです（新配列用）" << std::endl;
    Terminal::resetTextColor();
    
    // 青色テスト
    Terminal::setTextColor(Terminal::LIGHT_BLUE);
    std::cout << "これは青色のテキストです（旧配列用）" << std::endl;
    Terminal::resetTextColor();
    
    // 白色（通常）
    std::cout << "これは通常の白色テキストです" << std::endl;
    
    std::cout << "\n✓ setTextColor test completed (visual check)" << std::endl;
}

void test_ColoredLayoutNames() {
    std::cout << "Testing colored layout names..." << std::endl;
    
    std::cout << "\n--- Visual Test: Layout Names with Colors ---" << std::endl;
    
    // 新配列（緑）
    std::cout << "新配列: ";
    Terminal::setTextColor(Terminal::LIGHT_GREEN);
    std::cout << WaitingScreen::getLayoutName("new");
    Terminal::resetTextColor();
    std::cout << std::endl;
    
    // 旧配列（青）
    std::cout << "旧配列: ";
    Terminal::setTextColor(Terminal::LIGHT_BLUE);
    std::cout << WaitingScreen::getLayoutName("old");
    Terminal::resetTextColor();
    std::cout << std::endl;
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    std::cout << "\n✓ Colored layout names test completed (visual check)" << std::endl;
}

void test_WaitingScreenWithColors() {
    std::cout << "Testing waiting screen with colors..." << std::endl;
    
    std::cout << "\n--- Visual Test: Waiting Screen (New Layout - Green) ---" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 新配列のカウントダウン（緑色で表示されるはず）
    WaitingScreen::showCountdown(1, "new", 2);
    
    std::cout << "\n--- Visual Test: Waiting Screen (Old Layout - Blue) ---" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 旧配列のカウントダウン（青色で表示されるはず）
    WaitingScreen::showCountdown(2, "old", 2);
    
    Terminal::clearScreen();
    std::cout << "\n✓ Waiting screen with colors test completed (visual check)" << std::endl;
}

void test_MultipleColorChanges() {
    std::cout << "Testing multiple color changes..." << std::endl;
    
    std::cout << "\n--- Visual Test: Rapid Color Changes ---" << std::endl;
    
    for (int i = 0; i < 3; i++) {
        Terminal::setTextColor(Terminal::LIGHT_GREEN);
        std::cout << "新配列 ";
        Terminal::setTextColor(Terminal::LIGHT_BLUE);
        std::cout << "旧配列 ";
        Terminal::resetTextColor();
        std::cout << "通常 ";
    }
    std::cout << std::endl;
    
    std::cout << "\n✓ Multiple color changes test completed (visual check)" << std::endl;
}

int main() {
    // UTF-8出力を有効化（Windows）
    #ifdef _WIN32
    system("chcp 65001 > nul");
    #endif
    
    std::cout << "=== Color Display Tests ===" << std::endl;
    std::cout << "Note: These tests include visual checks for color display" << std::endl;
    std::cout << std::endl;
    
    test_ColorConstants();
    test_SetTextColor();
    test_ColoredLayoutNames();
    test_WaitingScreenWithColors();
    test_MultipleColorChanges();
    
    // 最終メッセージ
    Terminal::clearScreen();
    std::cout << "\n";
    Terminal::setTextColor(Terminal::LIGHT_GREEN);
    std::cout << "✓✓✓ All tests passed! ✓✓✓";
    Terminal::resetTextColor();
    std::cout << std::endl;
    std::cout << "\nNote: Visual tests require manual verification of colors:" << std::endl;
    std::cout << "  - 新配列 should appear in GREEN" << std::endl;
    std::cout << "  - 旧配列 should appear in BLUE" << std::endl;
    
    return 0;
}

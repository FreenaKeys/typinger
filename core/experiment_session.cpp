#include "experiment_session.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>

// 有効な被験者IDのリスト
const std::string VALID_IDS[] = {"SK", "MT", "IK", "KS", "KM"};
const int VALID_ID_COUNT = 5;

// セッション番号の範囲
const int MIN_SESSION = 1;
const int MAX_SESSION = 14;

bool isValidSubjectID(const std::string& id) {
    for (int i = 0; i < VALID_ID_COUNT; i++) {
        if (id == VALID_IDS[i]) {
            return true;
        }
    }
    return false;
}

bool isValidSessionNumber(int session_num) {
    return session_num >= MIN_SESSION && session_num <= MAX_SESSION;
}

bool isValidLayoutTypeNumber(int type_num) {
    return type_num == 1 || type_num == 2;
}

std::string inputSubjectID() {
    std::string id;
    
    while (true) {
        std::cout << "\n被験者IDを入力してください (SK/MT/IK/KS/KM): ";
        std::cin >> id;
        
        // 大文字に変換
        for (char& c : id) {
            c = toupper(c);
        }
        
        if (isValidSubjectID(id)) {
            return id;
        }
        
        std::cout << "✗ IDが誤っています。もう一度入力してください。\n";
    }
}

int inputSessionNumber() {
    int session_num;
    
    while (true) {
        std::cout << "セッション番号を入力してください (1-14): ";
        std::cin >> session_num;
        
        // 入力エラーのチェック
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "✗ セッション番号が誤っています。もう一度入力してください。\n";
            continue;
        }
        
        if (isValidSessionNumber(session_num)) {
            return session_num;
        }
        
        std::cout << "✗ セッション番号が誤っています。もう一度入力してください。\n";
    }
}

std::string inputDate() {
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(4) << (local_time->tm_year + 1900)
        << std::setw(2) << (local_time->tm_mon + 1)
        << std::setw(2) << local_time->tm_mday;
    
    return oss.str();
}

std::string inputFirstLayoutType() {
    int type_num;
    
    while (true) {
        std::cout << "最初にテストする配列を選択してください (1: 新配列, 2: 旧配列): ";
        std::cin >> type_num;
        
        // 入力エラーのチェック
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "✗ 配列タイプの番号が誤っています。もう一度入力してください。\n";
            continue;
        }
        
        if (isValidLayoutTypeNumber(type_num)) {
            return (type_num == 1) ? "new" : "old";
        }
        
        std::cout << "✗ 配列タイプの番号が誤っています。もう一度入力してください。\n";
    }
}

std::string getLayoutTypeForTest(const std::string& first_layout, int test_order) {
    // 奇数セッション: 新→旧→新→旧→新→旧
    // 偶数セッション: 旧→新→旧→新→旧→新
    // test_orderが奇数なら最初の配列、偶数なら逆
    
    if (test_order % 2 == 1) {
        return first_layout;
    } else {
        return (first_layout == "new") ? "old" : "new";
    }
}

#include "./windowmaker.h"
#include <windows.h>

/**
 * @brief UTF-8 文字列を UTF-16 (wchar_t) に変換
 * @param utf8 UTF-8 エンコードされた std::string
 * @return UTF-16 エンコードされた std::wstring
 */
std::wstring utf8ToUtf16(const std::string& utf8) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &utf8[0], (int)utf8.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &utf8[0], (int)utf8.size(), &wstr[0], size_needed);
    return wstr;
}

/**
 * @brief Windows API に準拠した確認ダイアログを生成（日本語対応）
 * @details MessageBoxW を使用してモーダルダイアログを表示（Unicode版で日本語対応）
 * @param title ダイアログのタイトル（UTF-8）
 * @param content ダイアログの内容（メッセージ、UTF-8）
 * @return true if OK button pressed, false if Cancel button pressed
 */
bool showConfirmDialog(const std::string& title, const std::string& content) {
    // UTF-8 文字列を UTF-16 に変換
    std::wstring titleW = utf8ToUtf16(title);
    std::wstring contentW = utf8ToUtf16(content);
    
    int result = MessageBoxW(
        NULL,                           // 親ウィンドウハンドル（NULL = デスクトップに相対）
        contentW.c_str(),               // メッセージテキスト（UTF-16）
        titleW.c_str(),                 // タイトルバーテキスト（UTF-16）
        MB_OKCANCEL | MB_ICONQUESTION   // ボタンスタイル: OK/キャンセルボタン + 疑問符アイコン
    );
    
    return result == IDOK;
}
